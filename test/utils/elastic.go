// {C} Copyright 2018 Pensando Systems Inc. All rights reserved.

package utils

import (
	"context"
	"crypto/x509"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"os/exec"
	"path"
	"runtime"
	"strings"
	"time"

	"github.com/pensando/sw/venice/cmd/credentials"
	"github.com/pensando/sw/venice/globals"
	"github.com/pensando/sw/venice/utils/certs"
	"github.com/pensando/sw/venice/utils/elastic"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/resolver"
)

var (
	elasticImage        = "docker.elastic.co/elasticsearch/elasticsearch:6.3.2"
	elasticClusterImage = "registry.test.pensando.io:5000/elasticsearch-cluster:v0.6"
	elasticHost         = "127.0.0.1"
)

// CreateElasticClient helper function to create elastic client
func CreateElasticClient(elasticsearchAddr string, resolverClient resolver.Interface, logger log.Logger, signer certs.CSRSigner, trustRoots []*x509.Certificate) (elastic.ESClient, error) {
	esClient, err := createElasticClient(elasticsearchAddr, resolverClient, logger, signer, trustRoots)
	if err != nil {
		return nil, err
	}

	// check cluster health
	if !IsElasticClusterHealthy(esClient) {
		return nil, fmt.Errorf("elastic cluster not healthy")
	}

	return esClient, nil
}

// StartElasticsearch starts elasticsearch service
func StartElasticsearch(name string, signer certs.CSRSigner, trustRoots []*x509.Certificate) (string, error) {
	log.Info("starting elasticsearch ..")

	// set max_map_count; this is a must requirement to run elasticsearch
	// https://www.elastic.co/guide/en/elasticsearch/reference/current/vm-max-map-count.html
	//
	// this need to be set manually for docker for mac using the below commands:
	// $ screen ~/Library/Containers/com.docker.docker/Data/vms/0/tty
	// $ sysctl -w vm.max_map_count=262144
	//
	if runtime.GOOS != "darwin" {
		if out, err := exec.Command("sysctl", "-w", "vm.max_map_count=262144").CombinedOutput(); err != nil {
			log.Errorf("failed to set max_map_count %s, err: %v", out, err)
			return "", err
		}
	} else {
		fmt.Println("\n++++++ run this one time setup commands from your mac if you haven't done yet +++++++\n" +
			"screen ~/Library/Containers/com.docker.docker/Data/vms/0/tty\n" +
			"on the blank screen, press return and run: sysctl -w vm.max_map_count=262144")
		fmt.Println()
	}

	var authDir string
	if signer != nil {
		authDir = path.Join("/tmp", fmt.Sprintf("%s-elastic-test", name))
		os.MkdirAll(authDir, 0777) // pre-create to override default permissions
		err := credentials.GenElasticHTTPSAuth(authDir, signer, trustRoots)
		if err != nil {
			return "", fmt.Errorf("error creating credentials in dir %s: err: %v", authDir, err)
		}
	}

	// same port needs to be exposed outside as inside to make sure underlying sniffer works given that the
	// test is run ousite the elasticsearch container.
	for port := 6000; port < 7000; port++ {
		// If we have a CSRSigner we generate credentials and start the Venice-specific container with Elastic + TLS plugin
		// otherwise we just start the stock Elastic container without auth
		var cmd []string
		if signer != nil {
			cmd = []string{
				"run", "--rm", "-d", "-p", fmt.Sprintf("%d:%d", port, port),
				fmt.Sprintf("--name=%s", name),
				"-e", fmt.Sprintf("cluster.name=%s", name),
				"-e", "ES_JAVA_OPTS=-Xms512m -Xmx512m",
				"-e", fmt.Sprintf("http.port=%d", port),
				"-e", fmt.Sprintf("http.publish_host=%s", elasticHost),
				"-v", fmt.Sprintf("%s:/usr/share/elasticsearch/config/auth-node:ro", authDir),
				"-v", fmt.Sprintf("%s:/usr/share/elasticsearch/config/auth-https:ro", authDir),
				elasticClusterImage}
		} else {
			cmd = []string{
				"run", "--rm", "-d", "-p", fmt.Sprintf("%d:%d", port, port),
				fmt.Sprintf("--name=%s", name),
				"-e", fmt.Sprintf("cluster.name=%s", name),
				"-e", "xpack.security.enabled=false",
				"-e", "xpack.monitoring.enabled=false",
				"-e", "xpack.graph.enabled=false",
				"-e", "xpack.watcher.enabled=false",
				"-e", "xpack.logstash.enabled=false",
				"-e", "xpack.ml.enabled=false",
				"-e", "ES_JAVA_OPTS=-Xms512m -Xmx512m",
				"-e", fmt.Sprintf("http.port=%d", port),
				"-e", fmt.Sprintf("http.publish_host=%s", elasticHost),
				elasticImage}
		}

		// run the command
		out, err := exec.Command("docker", cmd...).CombinedOutput()

		// stop and retry if a container with the same name exists already
		if strings.Contains(string(out), "Conflict") {
			log.Errorf("conflicting names, retrying")
			StopElasticsearch(name)
			continue
		}

		// retry with a different port
		if strings.Contains(string(out), "port is already allocated") {
			log.Errorf("port already allocated, retrying")
			continue
		}

		if err != nil {
			return "", fmt.Errorf("%s, err: %v", out, err)
		}

		elasticAddr := fmt.Sprintf("%s:%d", elasticHost, port)
		log.Infof("started elasticsearch: %s", elasticAddr)

		return elasticAddr, nil
	}

	return "", fmt.Errorf("exhausted all the ports from 6000-6999, failed to start elasticsearch")
}

// StopElasticsearch stops elasticsearch service
func StopElasticsearch(name string) error {
	if len(strings.TrimSpace(name)) == 0 {
		return nil
	}

	log.Info("stopping elasticsearch ..")

	authDir := path.Join(os.TempDir(), fmt.Sprintf("%s-elastic-test", name))
	defer certs.DeleteTLSCredentials(authDir)
	defer os.RemoveAll(authDir)

	cmd := []string{"rm", "-f", name}

	// run the command
	out, err := exec.Command("docker", cmd...).CombinedOutput()

	if err != nil && !strings.Contains(string(out), "No such container") {
		log.Infof("docker run cmd failed, err: %+v", err)
		return fmt.Errorf("%s, err: %v", out, err)
	}

	return err
}

// GetElasticsearchAddress returns the address of elasticsearch server
func GetElasticsearchAddress(name string) (string, error) {
	if len(strings.TrimSpace(name)) == 0 {
		return "", nil
	}

	cmd := []string{"inspect", "-f", "{{range $p, $conf := .HostConfig.PortBindings}}{{range $conf}}{{println .HostPort}}{{end}}{{end}}", name}
	ports, err := exec.Command("docker", cmd...).CombinedOutput()
	if err != nil {
		return "", err
	}

	if len(strings.TrimSpace(string(ports))) == 0 {
		return "", fmt.Errorf("no ports exposed")
	}

	// it takes the first exposed port
	port := strings.Split(string(ports), "\n")[0]
	addr := fmt.Sprintf("%s:%s", elasticHost, strings.TrimSpace(port))

	log.Infof("elasticsearch address: %v", addr)

	return addr, nil
}

// IsElasticClusterHealthy checks if the cluster is healthy or not
func IsElasticClusterHealthy(esClient elastic.ESClient) bool {
	healthy, err := esClient.IsClusterHealthy(context.Background())
	if err != nil {
		return false
	}

	return healthy
}

// helper function to create the client
func createElasticClient(elasticsearchAddr string, resolverClient resolver.Interface, logger log.Logger, signer certs.CSRSigner, trustRoots []*x509.Certificate) (elastic.ESClient, error) {
	var err error
	var esClient elastic.ESClient

	opts := []elastic.Option{}
	if signer != nil {
		authDir, err := ioutil.TempDir("", "elastic-client")
		if err != nil {
			return nil, fmt.Errorf("Error creating temp dir for credentials: %v", err)
		}
		defer os.RemoveAll(authDir)
		err = credentials.GenElasticClientsAuth(authDir, signer, trustRoots)
		if err != nil {
			return nil, fmt.Errorf("Error generating Elastic client TLS credentials: %v", err)
		}
		tlsConfig, err := certs.LoadTLSCredentials(authDir)
		if err != nil {
			return nil, fmt.Errorf("Error accessing client credentials: %v", err)
		}

		tlsConfig.ServerName = globals.ElasticSearch + "-https"
		transport := &http.Transport{TLSClientConfig: tlsConfig}
		opts = append(opts, elastic.WithHTTPClient(&http.Client{Transport: transport}))
	}

	log.Infof("creating elasticsearch client using address: %v", elasticsearchAddr)

	retryInterval := 10 * time.Millisecond
	timeout := 2 * time.Minute
	for {
		select {
		case <-time.After(retryInterval):
			if esClient == nil {
				esClient, err = elastic.NewClient(elasticsearchAddr, resolverClient, logger, opts...)
			}

			// if the client is created, make sure the cluster is healthy
			if esClient != nil {
				log.Infof("created elasticsearch client")
				return esClient, nil
			}

			log.Infof("failed to create elasticsearch client, err: %v, retrying", err)
		case <-time.After(timeout):
			if err != nil {
				return nil, fmt.Errorf("failed to create elasticsearch client, err: %v", err)
			}
			return esClient, nil
		}
	}
}
