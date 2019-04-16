//-----------------------------------------------------------------------------
// {C} Copyright 2017 Pensando Systems Inc. All rights reserved
//-----------------------------------------------------------------------------

package cmd

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"os"
	"os/exec"
	"strings"
	"time"

	"gopkg.in/yaml.v2"

	"github.com/spf13/cobra"

	nmd "github.com/pensando/sw/nic/agent/nmd/protos"
)

var showTechCmd = &cobra.Command{
	Use:   "tech-support",
	Short: "Get Tech Support from Naples",
	Long:  "\n------------------------------\n Get Tech Support from Naples \n------------------------------\n",
	RunE:  showTechCmdHandler,
}

var destDir string
var cmdFile string
var tarFile string

func init() {
	sysCmd.AddCommand(showTechCmd)

	showTechCmd.Flags().StringVarP(&tarFile, "tarball", "b", "", "Name of tarball to create (without .tar.gz)")
}

var cmdToExecute = `
Cmds:
 -
   cmd: /nic/tools/fwupdate -l
   outputfile: fw_version.out
 -
   cmd: halctl show techsupport
   outputfile: hal-cmds.txt
`

// NaplesCmds is the format of the yaml file used to run commands on Naples for tech-support
type NaplesCmds struct {
	Cmds []struct {
		Cmd        string `yaml:"cmd"`
		Outputfile string `yaml:"outputfile"`
	} `yaml:"Cmds"`
}

func copyFileToDest(destDir string, url string, file string) error {
	//fmt.Println("Copying file: " + file + " to: " + destDir)
	resp, err := restGetResp(url + file)
	if err != nil {
		fmt.Println(err)
		return err
	}
	defer resp.Body.Close()
	file = destDir + "/" + file
	out, err := os.Create(file)
	if err != nil {
		fmt.Println(err)
		return err
	}
	defer out.Close()
	// Write the body to file
	_, err = io.Copy(out, resp.Body)
	if err != nil {
		fmt.Println(err)
		return err
	}
	return err
}

func createDestDir(destDir string) {
	if _, err := os.Stat(destDir); os.IsNotExist(err) {
		os.MkdirAll(destDir, os.ModePerm)
	}
}

func getLogs(subDir string, url string) error {
	//Copy out log files from /var/log recursively
	resp, _ := restGetResp(url)
	retS, err := parseFiles(resp)
	if err != nil {
		return err
	}
	logDestDir := destDir + "/" + subDir + "/"
	createDestDir(logDestDir)
	for _, file = range retS {
		if strings.HasSuffix(file, "/") {
			napDir := file
			logSubDestDir := logDestDir + napDir
			createDestDir(logSubDestDir)
			resp, _ = restGetResp(url + napDir)
			retSlice = nil
			retS, err = parseFiles(resp)
			if err != nil {
				return err
			}
			for _, subfile := range retS {
				fmt.Printf(".")
				copyFileToDest(logSubDestDir, "monitoring/v1/naples/logs/"+napDir, subfile)
			}
			retSlice = nil
		} else {
			fmt.Printf(".")
			copyFileToDest(logDestDir, url, file)
		}
	}
	return nil
}

func showTechCmdHandler(cmd *cobra.Command, args []string) error {
	jsonFormat = false
	timeStr := time.Now().Format(time.UnixDate)
	timeStr = strings.Replace(timeStr, " ", "-", -1)

	destDir = "/tmp"
	if val, ok := os.LookupEnv("TMPDIR"); ok {
		destDir = val
		if verbose {
			fmt.Printf("$TMPDIR set to %s\n", val)
		}
	}

	destDir = destDir + "/NaplesTechSupport-" + timeStr + "/"
	fmt.Println("Collecting tech-support from Naples")

	if _, err := os.Stat(destDir); os.IsNotExist(err) {
		os.MkdirAll(destDir, os.ModePerm)
	}

	fmt.Printf("Fetching cores")
	//Copy out core files from /data/core
	coreDestDir := destDir + "/cores/"
	createDestDir(coreDestDir)
	resp, _ := restGetResp("cores/v1/naples/")
	retS, err := parseFiles(resp)
	if err != nil {
		return err
	}
	for _, file := range retS {
		fmt.Printf(".")
		copyFileToDest(coreDestDir, "cores/v1/naples/", file)
	}
	fmt.Printf("\nCores fetched\n")
	retSlice = nil

	fmt.Printf("Fetching events")
	//Copy out all event files from /var/lib/pensando/events/ dir
	eventsDestDir := destDir + "/events/"
	createDestDir(eventsDestDir)
	evresp, _ := restGetResp("monitoring/v1/naples/events/")
	retS, err = parseFiles(evresp)
	if err != nil {
		return err
	}
	fmt.Println(retS)
	for _, file = range retS {
		if file == "events" || strings.HasSuffix(file, "/") {
			continue
		}
		copyFileToDest(eventsDestDir, "monitoring/v1/naples/events/", file)
	}
	fmt.Printf("\nEvents fetched\n")
	retSlice = nil

	fmt.Printf("Fetching logs")
	err = getLogs("logs", "monitoring/v1/naples/logs/")
	if err != nil {
		return err
	}
	err = getLogs("obfl", "monitoring/v1/naples/obfl/")
	if err != nil {
		return err
	}
	fmt.Printf("\nLogs fetched\n")

	fmt.Printf("Executing commands")
	//Execute cmds pointed to by YML file
	cmdDestDir := destDir + "/cmd_out/"
	createDestDir(cmdDestDir)

	var naplesCmds NaplesCmds
	err = yaml.UnmarshalStrict([]byte(cmdToExecute), &naplesCmds)
	if err != nil {
		fmt.Println(err)
		return err
	}
	for _, naplesCmd := range naplesCmds.Cmds {
		if naplesCmd.Outputfile == "" || naplesCmd.Cmd == "" {
			fmt.Printf("\nMissing command attributes %+v\n", naplesCmd)
			continue
		}
		cmd := strings.Fields(naplesCmd.Cmd)
		opts := strings.Join(cmd[1:], " ")
		v := &nmd.NaplesCmdExecute{
			Executable: cmd[0],
			Opts:       opts,
		}
		resp, err := restGetWithBody(v, "cmd/v1/naples/")
		if err != nil {
			fmt.Println(err)
		}
		if len(resp) > 3 {
			fmt.Printf(".")
			s := strings.Replace(string(resp), `\n`, "\n", -1)
			s = strings.Replace(s, "\\", "", -1)
			file = cmdDestDir + "/" + naplesCmd.Outputfile
			out, err := os.Create(file)
			if err != nil {
				fmt.Println(err)
			}
			defer out.Close()
			w := bufio.NewWriter(out)
			w.WriteString("===" + cmd[0] + " " + opts + "===\n" + s)
			w.Flush()
		}
	}
	fmt.Printf("\nCommands executed\n")

	file = destDir + "/penctl.ver"
	out, err := os.Create(file)
	if err != nil {
		fmt.Println(err)
	}
	defer out.Close()
	w := bufio.NewWriter(out)
	w.WriteString(getPenctlVer())
	w.Flush()

	if !cmd.Flags().Changed("tarball") {
		tarFile = "naples-tech-support"
	}
	fmt.Println("Creating tarball: " + tarFile + ".tar.gz")
	tarcmd := exec.Command("tar", "-zcvf", tarFile+".tar.gz", destDir)
	tarcmd.Stdin = strings.NewReader("tar naples-tech-support")
	var tarout bytes.Buffer
	tarcmd.Stdout = &tarout
	err = tarcmd.Run()
	if err != nil {
		return err
	}
	fmt.Println(tarFile + ".tar.gz generated")

	rmdestdircmd := exec.Command("rm", "-rf", destDir)
	rmdestdircmd.Stdin = strings.NewReader("rm -rf " + destDir)
	var rmout bytes.Buffer
	rmdestdircmd.Stdout = &rmout
	err = rmdestdircmd.Run()
	if err != nil {
		return err
	}

	return nil
}
