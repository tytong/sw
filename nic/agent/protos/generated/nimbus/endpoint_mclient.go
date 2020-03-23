// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package nimbus is a auto generated package.
Input file: endpoint.proto
*/

package nimbus

import (
	"context"
	"sync"
	"time"

	protoTypes "github.com/gogo/protobuf/types"
	"github.com/pensando/sw/api"
	"github.com/pensando/sw/nic/agent/dscagent/types"
	"github.com/pensando/sw/nic/agent/protos/netproto"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pkg/errors"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/connectivity"
	"google.golang.org/grpc/status"
)

type EndpointReactor interface {
	HandleEndpoint(oper types.Operation, endpointObj netproto.Endpoint) ([]netproto.Endpoint, error)
	GetWatchOptions(cts context.Context, kind string) api.ListWatchOptions
}
type EndpointOStream struct {
	sync.Mutex
	stream netproto.EndpointApiV1_EndpointOperUpdateClient
}

// WatchEndpoints runs Endpoint watcher loop
func (client *NimbusClient) WatchEndpoints(ctx context.Context, reactor EndpointReactor) {
	// setup wait group
	client.waitGrp.Add(1)
	defer client.waitGrp.Done()
	client.debugStats.AddInt("ActiveEndpointWatch", 1)

	// make sure rpc client is good
	if client.rpcClient == nil || client.rpcClient.ClientConn == nil || client.rpcClient.ClientConn.GetState() != connectivity.Ready {
		log.Errorf("RPC client is disconnected. Exiting watch")
		return
	}

	// start the watch
	watchOptions := reactor.GetWatchOptions(ctx, "Endpoint")
	endpointRPCClient := netproto.NewEndpointApiV1Client(client.rpcClient.ClientConn)
	stream, err := endpointRPCClient.WatchEndpoints(ctx, &watchOptions)
	if err != nil {
		log.Errorf("Error watching Endpoint. Err: %v", err)
		return
	}

	// start oper update stream
	opStream, err := endpointRPCClient.EndpointOperUpdate(ctx)
	if err != nil {
		log.Errorf("Error starting Endpoint oper updates. Err: %v", err)
		return
	}

	ostream := &EndpointOStream{stream: opStream}

	// get a list of objects
	objList, err := endpointRPCClient.ListEndpoints(ctx, &watchOptions)
	if err != nil {
		st, ok := status.FromError(err)
		if !ok || st.Code() == codes.Unavailable {
			log.Errorf("Error getting Endpoint list. Err: %v", err)
			return
		}
	} else {
		// perform a diff of the states
		client.diffEndpoints(objList, reactor, ostream)
	}

	// start grpc stream recv
	recvCh := make(chan *netproto.EndpointEvent, evChanLength)
	go client.watchEndpointRecvLoop(stream, recvCh)

	// loop till the end
	for {
		evtWork := func(evt *netproto.EndpointEvent) {
			client.debugStats.AddInt("EndpointWatchEvents", 1)
			log.Infof("Ctrlerif: agent %s got Endpoint watch event: Type: {%+v} Endpoint:{%+v}", client.clientName, evt.EventType, evt.Endpoint.ObjectMeta)
			client.lockObject(evt.Endpoint.GetObjectKind(), evt.Endpoint.ObjectMeta)
			go client.processEndpointEvent(*evt, reactor, ostream)
			//Give it some time to increment waitgrp
			time.Sleep(100 * time.Microsecond)
		}
		//Give priority to evnt work.
		select {
		case evt, ok := <-recvCh:
			if !ok {
				log.Warnf("Endpoint Watch channel closed. Exisint EndpointWatch")
				return
			}
			evtWork(evt)
		// periodic resync (Disabling as we have aggregate watch support)
		case <-time.After(resyncInterval):
			//Give priority to evt work
			//Wait for batch interval for inflight work
			time.Sleep(5 * DefaultWatchHoldInterval)
			select {
			case evt, ok := <-recvCh:
				if !ok {
					log.Warnf("Endpoint Watch channel closed. Exisint EndpointWatch")
					return
				}
				evtWork(evt)
				continue
			default:
			}
			// get a list of objects
			objList, err := endpointRPCClient.ListEndpoints(ctx, &watchOptions)
			if err != nil {
				st, ok := status.FromError(err)
				if !ok || st.Code() == codes.Unavailable {
					log.Errorf("Error getting Endpoint list. Err: %v", err)
					return
				}
			} else {
				client.debugStats.AddInt("EndpointWatchResyncs", 1)
				// perform a diff of the states
				client.diffEndpoints(objList, reactor, ostream)
			}
		}
	}
}

// watchEndpointRecvLoop receives from stream and write it to a channel
func (client *NimbusClient) watchEndpointRecvLoop(stream netproto.EndpointApiV1_WatchEndpointsClient, recvch chan<- *netproto.EndpointEvent) {
	defer close(recvch)
	client.waitGrp.Add(1)
	defer client.waitGrp.Done()

	// loop till the end
	for {
		// receive from stream
		objList, err := stream.Recv()
		if err != nil {
			log.Errorf("Error receiving from watch channel. Exiting Endpoint watch. Err: %v", err)
			return
		}
		for _, evt := range objList.EndpointEvents {
			recvch <- evt
		}
	}
}

// diffEndpoint diffs local state with controller state
// FIXME: this is not handling deletes today
func (client *NimbusClient) diffEndpoints(objList *netproto.EndpointList, reactor EndpointReactor, ostream *EndpointOStream) {
	// build a map of objects
	objmap := make(map[string]*netproto.Endpoint)
	for _, obj := range objList.Endpoints {
		key := obj.ObjectMeta.GetKey()
		objmap[key] = obj
	}

	// see if we need to delete any locally found object
	o := netproto.Endpoint{
		TypeMeta: api.TypeMeta{Kind: "Endpoint"},
	}

	localObjs, err := reactor.HandleEndpoint(types.List, o)
	if err != nil {
		log.Error(errors.Wrapf(types.ErrNimbusHandling, "Op: %s | Kind: Endpoint | Err: %v", types.Operation(types.List), err))
	}
	for _, lobj := range localObjs {
		ctby, ok := lobj.ObjectMeta.Labels["CreatedBy"]
		if ok && ctby == "Venice" {
			key := lobj.ObjectMeta.GetKey()
			if nobj, ok := objmap[key]; !ok {
				evt := netproto.EndpointEvent{
					EventType: api.EventType_DeleteEvent,

					Endpoint: lobj,
				}
				log.Infof("diffEndpoints(): Deleting object %+v", lobj.ObjectMeta)
				client.lockObject(evt.Endpoint.GetObjectKind(), evt.Endpoint.ObjectMeta)
				client.processEndpointEvent(evt, reactor, ostream)
			} else if ok && (nobj.GenerationID == lobj.GenerationID) {
				//Delete it so that we don't add/update
				delete(objmap, key)
			}
		} else {
			log.Infof("Not deleting non-venice object %+v", lobj.ObjectMeta)
		}
	}

	// add/update all new objects
	for _, obj := range objmap {
		evt := netproto.EndpointEvent{
			EventType: api.EventType_UpdateEvent,

			Endpoint: *obj,
		}
		client.lockObject(evt.Endpoint.GetObjectKind(), evt.Endpoint.ObjectMeta)
		client.processEndpointEvent(evt, reactor, ostream)
	}
}

// processEndpointEvent handles Endpoint event
func (client *NimbusClient) processEndpointEvent(evt netproto.EndpointEvent, reactor EndpointReactor, ostream *EndpointOStream) error {
	var err error
	client.waitGrp.Add(1)
	defer client.waitGrp.Done()

	// add venice label to the object
	evt.Endpoint.ObjectMeta.Labels = make(map[string]string)
	evt.Endpoint.ObjectMeta.Labels["CreatedBy"] = "Venice"

	log.Infof("Endpoint: processEndpointEvent | Evt: %+v", evt)
	// unlock the object once we are done
	defer client.unlockObject(evt.Endpoint.GetObjectKind(), evt.Endpoint.ObjectMeta)

	// retry till successful
	for iter := 0; iter < maxOpretry; iter++ {
		switch evt.EventType {
		case api.EventType_CreateEvent:
			fallthrough
		case api.EventType_UpdateEvent:

			_, err = reactor.HandleEndpoint(types.Get, evt.Endpoint)

			if err != nil {
				// create the Endpoint

				_, err = reactor.HandleEndpoint(types.Create, evt.Endpoint)

				if err != nil {
					log.Error(errors.Wrapf(types.ErrNimbusHandling, "Op: %s | Kind: Endpoint | Key: %s | Err: %v", types.Operation(types.Create), evt.Endpoint.GetKey(), err))
					client.debugStats.AddInt("CreateEndpointError", 1)
				} else {
					client.debugStats.AddInt("CreateEndpoint", 1)
				}
			} else {
				// update the Endpoint

				_, err = reactor.HandleEndpoint(types.Update, evt.Endpoint)

				if err != nil {
					log.Error(errors.Wrapf(types.ErrNimbusHandling, "Op: %s | Kind: Endpoint | Key: %s | Err: %v", types.Operation(types.Update), evt.Endpoint.GetKey(), err))
					client.debugStats.AddInt("UpdateEndpointError", 1)
				} else {
					client.debugStats.AddInt("UpdateEndpoint", 1)
				}
			}

		case api.EventType_DeleteEvent:
			// update the Endpoint

			_, err = reactor.HandleEndpoint(types.Delete, evt.Endpoint)

			if err != nil {
				log.Error(errors.Wrapf(types.ErrNimbusHandling, "Op: %s | Kind: Endpoint | Key: %s | Err: %v", types.Operation(types.Delete), evt.Endpoint.GetKey(), err))
				client.debugStats.AddInt("DeleteEndpointError", 1)
			} else {
				client.debugStats.AddInt("DeleteEndpoint", 1)
			}
		}

		if ostream == nil {
			return err
		}
		// send oper status and return if there is no error
		if err == nil {

			robj := netproto.EndpointEvent{
				EventType: evt.EventType,
				Endpoint: netproto.Endpoint{

					TypeMeta:   evt.Endpoint.TypeMeta,
					ObjectMeta: evt.Endpoint.ObjectMeta,
					Status:     evt.Endpoint.Status,
				},
			}

			// send oper status
			ostream.Lock()
			modificationTime, _ := protoTypes.TimestampProto(time.Now())
			robj.Endpoint.ObjectMeta.ModTime = api.Timestamp{Timestamp: *modificationTime}

			err := ostream.stream.Send(&robj)

			if err != nil {
				log.Errorf("failed to send Endpoint oper Status, %s", err)
				client.debugStats.AddInt("EndpointOperSendError", 1)
			} else {
				client.debugStats.AddInt("EndpointOperSent", 1)
			}
			ostream.Unlock()

			return err
		}

		// else, retry after some time, with backoff
		time.Sleep(time.Second * time.Duration(2*iter))
	}

	return nil
}

func (client *NimbusClient) processEndpointDynamic(evt api.EventType,
	object *netproto.Endpoint, reactor EndpointReactor) error {

	endpointEvt := netproto.EndpointEvent{
		EventType: evt,

		Endpoint: *object,
	}

	// add venice label to the object
	endpointEvt.Endpoint.ObjectMeta.Labels = make(map[string]string)
	endpointEvt.Endpoint.ObjectMeta.Labels["CreatedBy"] = "Venice"

	client.lockObject(endpointEvt.Endpoint.GetObjectKind(), endpointEvt.Endpoint.ObjectMeta)

	err := client.processEndpointEvent(endpointEvt, reactor, nil)
	modificationTime, _ := protoTypes.TimestampProto(time.Now())
	object.ObjectMeta.ModTime = api.Timestamp{Timestamp: *modificationTime}

	return err
}
