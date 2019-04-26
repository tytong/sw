package installer

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"os/exec"
	"strings"
	"time"

	"github.com/pensando/sw/venice/cmd/env"
	"github.com/pensando/sw/venice/globals"
	"github.com/pensando/sw/venice/utils/imagestore"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/systemd"
)

func copyFileContents(src, dst string) (err error) {
	in, err := os.Open(src)
	if err != nil {
		return err
	}
	defer in.Close()
	out, err := os.Create(dst)
	if err != nil {
		return err
	}
	defer func() {
		cerr := out.Close()
		if err == nil {
			err = cerr
		}
	}()
	if _, err = io.Copy(out, in); err != nil {
		return err
	}
	err = out.Sync()
	return err
}

const (
	installerTmpDir       = globals.RuntimeDir + "/installer"
	tmpImageFileName      = "venice.tgz"
	installerMetaFileName = "venice-install.json"
	maxIters              = 10
)

// PreCheck installation of given version
func PreCheck(version string) error {
	log.Debugf("About to download image")
	imageName, err := DownloadImage(version)
	if err != nil {
		return fmt.Errorf("DownloadImage version %s name %s returned %v", version, imageName, err)
	}
	log.Debugf("About to Extract image")
	err = ExtractImage(imageName)
	if err != nil {
		return fmt.Errorf("ExtractImage version %s name %s returned %v", version, imageName, err)
	}
	log.Debugf("About to remove downloaded image")
	os.Remove(imageName)
	log.Debugf("About to Preload image")
	err = PreLoadImage()
	if err != nil {
		return fmt.Errorf("PreloadImage version %s name  %s returned  %v", version, imageName, err)
	}
	return nil
}

// RunVersion is called after PreCheck to actually load and run a given version
func RunVersion(version string) error {
	// TODO: check the version as needed

	err := LoadAndInstallImage()
	Cleanup()
	return err
}

// DownloadImage downloads an image from minio and returns the local filename
func DownloadImage(version string) (string, error) {

	if err := imagestore.DownloadVeniceImage(context.Background(), env.ResolverClient, version); err != nil {
		return "", fmt.Errorf("Error %s during image download of version %s", err, version)
	}

	if err := os.RemoveAll(installerTmpDir); err != nil {
		return "", fmt.Errorf("Error %s during removeAll of %s", err, installerTmpDir)
	}
	if err := os.MkdirAll(installerTmpDir, 0700); err != nil {
		return "", fmt.Errorf("Error %s during mkdirAll of %s", err, installerTmpDir)
	}
	if err := copyFileContents(tmpImageFileName, installerTmpDir+"/"+tmpImageFileName); err != nil {
		return "", fmt.Errorf("Error %s during DownloadImage.copyFileContents", err)
	}
	return installerTmpDir + "/" + tmpImageFileName, nil
}

// ExtractImage takes a locally downloaded image and extracts the contents
func ExtractImage(filename string) error {
	var err error
	if _, err = exec.LookPath("tar"); err != nil {
		log.Errorf("LookPath failed during extract err %v", err)
		return err
	}
	cmd := exec.Command("tar", "-C", installerTmpDir, "-zxvf", filename)
	output, err := cmd.CombinedOutput()
	if err != nil {
		log.Errorf("ExtractImage failed during extract of %s with output:%s errcode %v", filename, string(output), err)
	}
	return err
}

type installationStep struct {
	Data        string `json:"data,omitempty"`
	InstallType string `json:"installType,omitempty"`
}

type installationSteps struct {
	PreLoad        []installationStep
	LoadAndInstall []installationStep
}

func runSteps(steps []installationStep) error {
	syst := systemd.New()
	for _, step := range steps {
		log.Infof("Starting execution of step %#v", step)
		switch step.InstallType {
		case "container":
			cmd := exec.Command("docker", "load", "-i", installerTmpDir+"/tars/"+step.Data)
			var err error
			if err = cmd.Run(); err != nil {
				errStr := fmt.Sprintf("Installation failed at step %#v with err %s", step, err)
				return errors.New(errStr)
			}
		case "inline-script":
			cmd := exec.Command("sh", "-c", step.Data)
			var err error
			if err = cmd.Run(); err != nil {
				errStr := fmt.Sprintf("Installation failed at step %#v with err %s", step, err)
				return errors.New(errStr)
			}
		case "systemctl-daemon-reload":
			if err := syst.DaemonReload(); err != nil {
				log.Errorf("Error %v while issuing systemd.DaemonReload", err)
				return err
			}

		case "systemctl-reload-running":
			oldpid, _ := syst.GetServiceProperty(step.Data, "MainPID")
			if oldpid == "@u 0" {
				log.Infof("Process %v is not active pid [%s]", step.Data, oldpid)
				continue
			}
			log.Infof("restarting %s pid [%s]", step.Data, oldpid)
			if err := syst.RestartTargetIfRunning(step.Data); err != nil {
				log.Errorf("Error %v while issuing systemctl-reload-running %v", err, step.Data)
				return err
			}

			if strings.Contains(step.Data, "pen-etcd") {
				//Just wait 20 seconds for etcd to comeup. etcd restart is usually followed by a leader election
				//so we wait for 20 seconds during rollout for etcd to stabilize
				log.Infof("Waiting for pen-etcd to comeup..")
				time.Sleep(20 * time.Second)
			}
			//wait for the process to be up
			log.Infof("Checking the status of %s", step.Data)
			pid := getServiceProperty(step.Data, "MainPID")
			if pid == "@u 0" {
				errStr := fmt.Sprintf("Failed to rollout process %#v (oldpid: %v newpid: %v)", step.Data, oldpid, pid)
				return errors.New(errStr)
			}
			activeState := getUnitProperty(step.Data, "ActiveState", "\"active\"")
			if activeState != "\"active\"" {
				errStr := fmt.Sprintf("Failed to rollout process %#v (ActiveState %s)", step.Data, activeState)
				return errors.New(errStr)
			}

			subState := getUnitProperty(step.Data, "SubState", "\"running\"")
			if subState != "\"running\"" {
				errStr := fmt.Sprintf("Failed to rollout process %#v (SubState %s)", step.Data, subState)
				return errors.New(errStr)
			}

			log.Infof("Process %s : oldpid(%s) newpid(%s) activeState(%s) subState(%s)", step.Data, oldpid, pid, activeState, subState)
			if oldpid != pid && activeState == "\"active\"" && subState == "\"running\"" {
				log.Infof("Rollout of %s successful.", step.Data)
			}
		default:
			log.Errorf("unknown installType while executing step %#v", step)
		}
		log.Infof("Completed execution of step %#v", step)
	}
	return nil
}

// PreLoadImage is run after images is extracted locally. This reads the venice-install.json
//	and goes through all the steps mentioned to load (but not run) the various sub-images
//	Other than diskspace usage, this is not supposed to modify any existing behavior
//	This is part of pre-check
func PreLoadImage() error {
	fileName := installerTmpDir + "/" + installerMetaFileName
	content, err := ioutil.ReadFile(fileName)
	if err != nil {
		log.Errorf("Error %v reading metadata file %s", err, fileName)
		return err
	}
	var installSteps installationSteps
	err = json.Unmarshal(content, &installSteps)
	if err != nil {
		log.Errorf("Error %v unmarshalling metadata. input: %s", err, content)
		return err
	}

	err = runSteps(installSteps.PreLoad)
	if err != nil {
		log.Errorf("Error %v during preLoad", err)
		return err
	}

	return err
}

// LoadAndInstallImage does the loading/installation of various components as mentioned in venice-install.json
func LoadAndInstallImage() error {
	fileName := installerTmpDir + "/" + installerMetaFileName
	content, err := ioutil.ReadFile(fileName)
	if err != nil {
		log.Errorf("Error %v reading metadata file %s", err, fileName)
		return err
	}
	var installSteps installationSteps
	err = json.Unmarshal(content, &installSteps)
	if err != nil {
		log.Errorf("Error %v unmarshalling metadata. input: %s", err, content)
		return err
	}

	err = runSteps(installSteps.LoadAndInstall)
	if err != nil {
		log.Errorf("Error %v during preLoad", err)
		return err
	}

	return err
}
func getServiceProperty(service string, property string) string {
	syst := systemd.New()
	var ii int
	var pid string
	var err error
	for ; ii < maxIters; ii++ {
		pid, err = syst.GetServiceProperty(service, property)
		log.Infof("%s of %s [%s]", property, service, pid)
		if pid == "@u 0" || err != nil {
			log.Infof("Waiting to get [%s] of %s err %s", property, service, err)
			time.Sleep(time.Second)
			continue
		}
		break
	}
	return pid
}
func getUnitProperty(unit string, property string, expectedVal string) string {
	syst := systemd.New()
	var ii int
	var propVal string
	var err error
	for ; ii < maxIters; ii++ {
		propVal, err = syst.GetUnitProperty(unit, property)
		log.Infof("%s of %s [%s]", property, unit, propVal)
		if propVal != expectedVal || err != nil {
			log.Infof("Waiting to get [%s] of %s: err[%s] currVal[%s]", property, unit, err, propVal)
			time.Sleep(time.Second)
			continue
		}
		break
	}
	return propVal
}

// Cleanup removes all downloaded/extracted image  contents
func Cleanup() error {
	if err := os.RemoveAll(installerTmpDir); err != nil {
		return fmt.Errorf("Error %s during removeAll of %s", err, installerTmpDir)
	}
	return nil
}
