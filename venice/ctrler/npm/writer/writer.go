// {C} Copyright 2017 Pensando Systems Inc. All rights reserved.

package writer

import (
	"context"
	"strings"

	"github.com/pensando/sw/api/generated/apiclient"
	"github.com/pensando/sw/api/generated/cluster"
	"github.com/pensando/sw/api/generated/network"
	"github.com/pensando/sw/api/generated/security"
	"github.com/pensando/sw/api/generated/workload"
	"github.com/pensando/sw/venice/globals"
	"github.com/pensando/sw/venice/utils/balancer"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/resolver"
	"github.com/pensando/sw/venice/utils/rpckit"
)

// Writable is the commong interface for objects that can be written to the
// Writer
type Writable interface {
	Write() error
}

// Writer is the api provided by writer object
type Writer interface {
	WriteNetwork(nw *network.Network) error
	WriteEndpoint(ep *workload.Endpoint, update bool) error
	WriteTenant(tn *cluster.Tenant) error
	WriteSecurityGroup(sg *security.SecurityGroup) error
	WriteSGPolicy(sgp *security.SGPolicy) error
	Close() error
}

// APISrvWriter is the writer instance
type APISrvWriter struct {
	apisrvURL string
	resolver  resolver.Interface
	apicl     apiclient.Services
}

// NewAPISrvWriter returns an API server writer
func NewAPISrvWriter(apiSrvURL string, resolver resolver.Interface) (Writer, error) {
	// create apisrv writer instance
	wr := APISrvWriter{
		apisrvURL: apiSrvURL,
		resolver:  resolver,
	}

	return &wr, nil
}

// getAPIClient gets an rpc client
func (wr *APISrvWriter) getAPIClient() (apiclient.Services, error) {
	// if we already have a client, just return it
	if wr.apicl != nil {
		return wr.apicl, nil
	}

	// create the api client
	l := log.WithContext("Pkg", "NpmApiWriter")
	apicl, err := apiclient.NewGrpcAPIClient(globals.Npm, wr.apisrvURL, l, rpckit.WithBalancer(balancer.New(wr.resolver)))
	if err != nil {
		log.Errorf("Failed to connect to gRPC server [%s]\n", wr.apisrvURL)
		return nil, err
	}

	wr.apicl = apicl
	return apicl, err
}

// WriteNetwork writes network object
func (wr *APISrvWriter) WriteNetwork(nw *network.Network) error {
	// if we have no URL, we are done
	if wr.apisrvURL == "" {
		return nil
	}

	// get the api client
	apicl, err := wr.getAPIClient()
	if err != nil {
		return err
	}

	// FIXME: clear the resource version till we figure out CAS semantics
	nw.ObjectMeta.ResourceVersion = ""

	// write it
	_, err = apicl.NetworkV1().Network().Update(context.Background(), nw)
	if (err != nil) && (strings.Contains(err.Error(), "Object store error")) {
		// retry create
		_, err = apicl.NetworkV1().Network().Create(context.Background(), nw)
	}

	return err
}

// WriteEndpoint writes endpoint object
func (wr *APISrvWriter) WriteEndpoint(ep *workload.Endpoint, update bool) error {
	// if we have no URL, we are done
	if wr.apisrvURL == "" {
		return nil
	}

	// get the api client
	apicl, err := wr.getAPIClient()
	if err != nil {
		return err
	}

	// FIXME: clear the resource version till we figure out CAS semantics
	ep.ObjectMeta.ResourceVersion = ""

	// write it
	if update {
		_, err = apicl.WorkloadV1().Endpoint().Update(context.Background(), ep)
	} else {
		_, err = apicl.WorkloadV1().Endpoint().Create(context.Background(), ep)
		// if create fails, try update instead
		if err != nil {
			_, err = apicl.WorkloadV1().Endpoint().Update(context.Background(), ep)
		}
	}
	return err
}

// WriteTenant writes tenant object
func (wr *APISrvWriter) WriteTenant(tn *cluster.Tenant) error {
	// if we have no URL, we are done
	if wr.apisrvURL == "" {
		return nil
	}

	// get the api client
	apicl, err := wr.getAPIClient()
	if err != nil {
		return err
	}

	// FIXME: clear the resource version till we figure out CAS semantics
	tn.ObjectMeta.ResourceVersion = ""

	// write it
	_, err = apicl.ClusterV1().Tenant().Update(context.Background(), tn)
	return err
}

// WriteSecurityGroup writes security group object
func (wr *APISrvWriter) WriteSecurityGroup(sg *security.SecurityGroup) error {
	// if we have no URL, we are done
	if wr.apisrvURL == "" {
		return nil
	}

	// get the api client
	apicl, err := wr.getAPIClient()
	if err != nil {
		return err
	}

	// FIXME: clear the resource version till we figure out CAS semantics
	sg.ObjectMeta.ResourceVersion = ""

	// write it
	_, err = apicl.SecurityV1().SecurityGroup().Update(context.Background(), sg)
	return err
}

// WriteSGPolicy write sg policy object
func (wr *APISrvWriter) WriteSGPolicy(sgp *security.SGPolicy) error {
	// if we have no URL, we are done
	if wr.apisrvURL == "" {
		return nil
	}

	// get the api client
	apicl, err := wr.getAPIClient()
	if err != nil {
		return err
	}

	// FIXME: clear the resource version till we figure out CAS semantics
	sgp.ObjectMeta.ResourceVersion = ""

	// write it
	_, err = apicl.SecurityV1().SGPolicy().Update(context.Background(), sgp)
	return err
}

// Close stops the client and releases resources
func (wr *APISrvWriter) Close() error {
	if wr.resolver != nil {
		wr.resolver.Stop()
		wr.resolver = nil
	}
	if wr.apicl != nil {
		wr.apicl.Close()
		wr.apicl = nil
	}
	return nil
}
