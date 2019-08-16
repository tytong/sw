// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package tokenauthGwService is a auto generated package.
Input file: svc_tokenauth.proto
*/
package tokenauthGwService

import (
	"context"
	"net/http"
	"sync"
	"time"

	"github.com/pkg/errors"
	oldcontext "golang.org/x/net/context"
	"google.golang.org/grpc"

	"github.com/pensando/grpc-gateway/runtime"

	"github.com/pensando/sw/api"
	tokenauth "github.com/pensando/sw/api/generated/tokenauth"
	grpcclient "github.com/pensando/sw/api/generated/tokenauth/grpc/client"
	"github.com/pensando/sw/api/interfaces"
	"github.com/pensando/sw/venice/apigw"
	"github.com/pensando/sw/venice/apigw/pkg"
	"github.com/pensando/sw/venice/apiserver"
	"github.com/pensando/sw/venice/globals"
	"github.com/pensando/sw/venice/utils/authz"
	"github.com/pensando/sw/venice/utils/balancer"
	hdr "github.com/pensando/sw/venice/utils/histogram"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/resolver"
	"github.com/pensando/sw/venice/utils/rpckit"
)

// Dummy vars to suppress import errors
var _ api.TypeMeta
var _ authz.Authorizer

type sTokenAuthV1GwService struct {
	logger     log.Logger
	defSvcProf apigw.ServiceProfile
	svcProf    map[string]apigw.ServiceProfile
}

type adapterTokenAuthV1 struct {
	conn    *rpckit.RPCClient
	service tokenauth.ServiceTokenAuthV1Client
	gwSvc   *sTokenAuthV1GwService
	gw      apigw.APIGateway
}

func (a adapterTokenAuthV1) GenerateNodeToken(oldctx oldcontext.Context, t *tokenauth.NodeTokenRequest, options ...grpc.CallOption) (*tokenauth.NodeTokenResponse, error) {
	// Not using options for now. Will be passed through context as needed.
	trackTime := time.Now()
	defer func() {
		hdr.Record("apigw.TokenAuthV1GenerateNodeToken", time.Since(trackTime))
	}()
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("GenerateNodeToken")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}

	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*tokenauth.NodeTokenRequest)
		return a.service.GenerateNodeToken(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*tokenauth.NodeTokenResponse), err
}

func (a adapterTokenAuthV1) AutoWatchSvcTokenAuthV1(oldctx oldcontext.Context, in *api.ListWatchOptions, options ...grpc.CallOption) (tokenauth.TokenAuthV1_AutoWatchSvcTokenAuthV1Client, error) {
	return nil, errors.New("not implemented")
}

func (e *sTokenAuthV1GwService) setupSvcProfile() {
	e.defSvcProf = apigwpkg.NewServiceProfile(nil, "", "", apiintf.UnknownOper)
	e.defSvcProf.SetDefaults()
	e.svcProf = make(map[string]apigw.ServiceProfile)

	e.svcProf["GenerateNodeToken"] = apigwpkg.NewServiceProfile(e.defSvcProf, "", "", apiintf.UnknownOper)
}

// GetDefaultServiceProfile returns the default fallback service profile for this service
func (e *sTokenAuthV1GwService) GetDefaultServiceProfile() (apigw.ServiceProfile, error) {
	if e.defSvcProf == nil {
		return nil, errors.New("not found")
	}
	return e.defSvcProf, nil
}

// GetServiceProfile returns the service profile for a given method in this service
func (e *sTokenAuthV1GwService) GetServiceProfile(method string) (apigw.ServiceProfile, error) {
	if ret, ok := e.svcProf[method]; ok {
		return ret, nil
	}
	return nil, errors.New("not found")
}

// GetCrudServiceProfile returns the service profile for a auto generated crud operation
func (e *sTokenAuthV1GwService) GetCrudServiceProfile(obj string, oper apiintf.APIOperType) (apigw.ServiceProfile, error) {
	name := apiserver.GetCrudServiceName(obj, oper)
	if name != "" {
		return e.GetServiceProfile(name)
	}
	return nil, errors.New("not found")
}

// GetProxyServiceProfile returns the service Profile for a reverse proxy path
func (e *sTokenAuthV1GwService) GetProxyServiceProfile(path string) (apigw.ServiceProfile, error) {
	name := "_RProxy_" + path
	return e.GetServiceProfile(name)
}

func (e *sTokenAuthV1GwService) CompleteRegistration(ctx context.Context,
	logger log.Logger,
	grpcserver *grpc.Server,
	m *http.ServeMux,
	rslvr resolver.Interface,
	wg *sync.WaitGroup) error {
	apigw := apigwpkg.MustGetAPIGateway()
	// IP:port destination or service discovery key.

	grpcaddr := "pen-cmd"
	grpcaddr = apigw.GetAPIServerAddr(grpcaddr)
	e.logger = logger

	marshaller := runtime.JSONBuiltin{}
	opts := runtime.WithMarshalerOption("*", &marshaller)
	muxMutex.Lock()
	if mux == nil {
		mux = runtime.NewServeMux(opts)
	}
	muxMutex.Unlock()
	e.setupSvcProfile()

	err := registerSwaggerDef(m, logger)
	if err != nil {
		logger.ErrorLog("msg", "failed to register swagger spec", "service", "tokenauth.TokenAuthV1", "err", err)
	}
	wg.Add(1)
	go func() {
		defer func() {
			muxMutex.Lock()
			mux = nil
			muxMutex.Unlock()
		}()
		defer wg.Done()
		for {
			nctx, cancel := context.WithCancel(ctx)
			cl, err := e.newClient(nctx, grpcaddr, rslvr, apigw.GetDevMode())
			if err == nil {
				muxMutex.Lock()
				err = tokenauth.RegisterTokenAuthV1HandlerWithClient(ctx, mux, cl)
				muxMutex.Unlock()
				if err == nil {
					logger.InfoLog("msg", "registered service tokenauth.TokenAuthV1")
					m.Handle("/tokenauth/v1/", http.StripPrefix("/tokenauth/v1", mux))
					return
				} else {
					err = errors.Wrap(err, "failed to register")
				}
			} else {
				err = errors.Wrap(err, "failed to create client")
			}
			cancel()
			logger.ErrorLog("msg", "failed to register", "service", "tokenauth.TokenAuthV1", "err", err)
			select {
			case <-ctx.Done():
				return
			case <-time.After(5 * time.Second):
			}
		}
	}()
	return nil
}

func (e *sTokenAuthV1GwService) newClient(ctx context.Context, grpcAddr string, rslvr resolver.Interface, devmode bool) (*adapterTokenAuthV1, error) {
	var opts []rpckit.Option
	opts = append(opts, rpckit.WithTLSClientIdentity(globals.APIGw))
	if rslvr != nil {
		opts = append(opts, rpckit.WithBalancer(balancer.New(rslvr)))
	} else {

		opts = append(opts, rpckit.WithRemoteServerName("pen-cmd"))
	}

	if !devmode {
		opts = append(opts, rpckit.WithTracerEnabled(false))
		opts = append(opts, rpckit.WithLoggerEnabled(false))
		opts = append(opts, rpckit.WithStatsEnabled(false))
	}

	client, err := rpckit.NewRPCClient(globals.APIGw, grpcAddr, opts...)
	if err != nil {
		return nil, errors.Wrap(err, "create rpc client")
	}

	e.logger.Infof("Connected to GRPC Server %s", grpcAddr)
	defer func() {
		go func() {
			<-ctx.Done()
			if cerr := client.Close(); cerr != nil {
				e.logger.ErrorLog("msg", "Failed to close conn on Done()", "addr", grpcAddr, "err", cerr)
			}
		}()
	}()

	cl := &adapterTokenAuthV1{conn: client, gw: apigwpkg.MustGetAPIGateway(), gwSvc: e, service: grpcclient.NewTokenAuthV1Backend(client.ClientConn, e.logger)}
	return cl, nil
}

func init() {

	apigw := apigwpkg.MustGetAPIGateway()

	svcTokenAuthV1 := sTokenAuthV1GwService{}
	apigw.Register("tokenauth.TokenAuthV1", "/", &svcTokenAuthV1)
}
