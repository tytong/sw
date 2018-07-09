// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package securityGwService is a auto generated package.
Input file: svc_security.proto
*/
package securityGwService

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
	security "github.com/pensando/sw/api/generated/security"
	grpcclient "github.com/pensando/sw/api/generated/security/grpc/client"
	"github.com/pensando/sw/venice/apigw"
	"github.com/pensando/sw/venice/apigw/pkg"
	"github.com/pensando/sw/venice/apiserver"
	"github.com/pensando/sw/venice/globals"
	"github.com/pensando/sw/venice/utils/balancer"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/resolver"
	"github.com/pensando/sw/venice/utils/rpckit"
)

// Dummy vars to suppress import errors
var _ api.TypeMeta

type sSecurityV1GwService struct {
	logger     log.Logger
	defSvcProf apigw.ServiceProfile
	svcProf    map[string]apigw.ServiceProfile
}

type adapterSecurityV1 struct {
	conn    *rpckit.RPCClient
	service security.ServiceSecurityV1Client
	gwSvc   *sSecurityV1GwService
	gw      apigw.APIGateway
}

func (a adapterSecurityV1) AutoAddApp(oldctx oldcontext.Context, t *security.App, options ...grpc.CallOption) (*security.App, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoAddApp")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.App)
		return a.service.AutoAddApp(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.App), err
}

func (a adapterSecurityV1) AutoAddCertificate(oldctx oldcontext.Context, t *security.Certificate, options ...grpc.CallOption) (*security.Certificate, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoAddCertificate")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.Certificate)
		return a.service.AutoAddCertificate(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.Certificate), err
}

func (a adapterSecurityV1) AutoAddSecurityGroup(oldctx oldcontext.Context, t *security.SecurityGroup, options ...grpc.CallOption) (*security.SecurityGroup, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoAddSecurityGroup")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.SecurityGroup)
		return a.service.AutoAddSecurityGroup(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.SecurityGroup), err
}

func (a adapterSecurityV1) AutoAddSgpolicy(oldctx oldcontext.Context, t *security.Sgpolicy, options ...grpc.CallOption) (*security.Sgpolicy, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoAddSgpolicy")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.Sgpolicy)
		return a.service.AutoAddSgpolicy(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.Sgpolicy), err
}

func (a adapterSecurityV1) AutoAddTrafficEncryptionPolicy(oldctx oldcontext.Context, t *security.TrafficEncryptionPolicy, options ...grpc.CallOption) (*security.TrafficEncryptionPolicy, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoAddTrafficEncryptionPolicy")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.TrafficEncryptionPolicy)
		return a.service.AutoAddTrafficEncryptionPolicy(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.TrafficEncryptionPolicy), err
}

func (a adapterSecurityV1) AutoDeleteApp(oldctx oldcontext.Context, t *security.App, options ...grpc.CallOption) (*security.App, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoDeleteApp")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.App)
		return a.service.AutoDeleteApp(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.App), err
}

func (a adapterSecurityV1) AutoDeleteCertificate(oldctx oldcontext.Context, t *security.Certificate, options ...grpc.CallOption) (*security.Certificate, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoDeleteCertificate")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.Certificate)
		return a.service.AutoDeleteCertificate(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.Certificate), err
}

func (a adapterSecurityV1) AutoDeleteSecurityGroup(oldctx oldcontext.Context, t *security.SecurityGroup, options ...grpc.CallOption) (*security.SecurityGroup, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoDeleteSecurityGroup")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.SecurityGroup)
		return a.service.AutoDeleteSecurityGroup(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.SecurityGroup), err
}

func (a adapterSecurityV1) AutoDeleteSgpolicy(oldctx oldcontext.Context, t *security.Sgpolicy, options ...grpc.CallOption) (*security.Sgpolicy, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoDeleteSgpolicy")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.Sgpolicy)
		return a.service.AutoDeleteSgpolicy(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.Sgpolicy), err
}

func (a adapterSecurityV1) AutoDeleteTrafficEncryptionPolicy(oldctx oldcontext.Context, t *security.TrafficEncryptionPolicy, options ...grpc.CallOption) (*security.TrafficEncryptionPolicy, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoDeleteTrafficEncryptionPolicy")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.TrafficEncryptionPolicy)
		return a.service.AutoDeleteTrafficEncryptionPolicy(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.TrafficEncryptionPolicy), err
}

func (a adapterSecurityV1) AutoGetApp(oldctx oldcontext.Context, t *security.App, options ...grpc.CallOption) (*security.App, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoGetApp")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.App)
		return a.service.AutoGetApp(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.App), err
}

func (a adapterSecurityV1) AutoGetCertificate(oldctx oldcontext.Context, t *security.Certificate, options ...grpc.CallOption) (*security.Certificate, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoGetCertificate")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.Certificate)
		return a.service.AutoGetCertificate(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.Certificate), err
}

func (a adapterSecurityV1) AutoGetSecurityGroup(oldctx oldcontext.Context, t *security.SecurityGroup, options ...grpc.CallOption) (*security.SecurityGroup, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoGetSecurityGroup")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.SecurityGroup)
		return a.service.AutoGetSecurityGroup(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.SecurityGroup), err
}

func (a adapterSecurityV1) AutoGetSgpolicy(oldctx oldcontext.Context, t *security.Sgpolicy, options ...grpc.CallOption) (*security.Sgpolicy, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoGetSgpolicy")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.Sgpolicy)
		return a.service.AutoGetSgpolicy(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.Sgpolicy), err
}

func (a adapterSecurityV1) AutoGetTrafficEncryptionPolicy(oldctx oldcontext.Context, t *security.TrafficEncryptionPolicy, options ...grpc.CallOption) (*security.TrafficEncryptionPolicy, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoGetTrafficEncryptionPolicy")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.TrafficEncryptionPolicy)
		return a.service.AutoGetTrafficEncryptionPolicy(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.TrafficEncryptionPolicy), err
}

func (a adapterSecurityV1) AutoListApp(oldctx oldcontext.Context, t *api.ListWatchOptions, options ...grpc.CallOption) (*security.AppList, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoListApp")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*api.ListWatchOptions)
		return a.service.AutoListApp(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.AppList), err
}

func (a adapterSecurityV1) AutoListCertificate(oldctx oldcontext.Context, t *api.ListWatchOptions, options ...grpc.CallOption) (*security.CertificateList, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoListCertificate")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*api.ListWatchOptions)
		return a.service.AutoListCertificate(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.CertificateList), err
}

func (a adapterSecurityV1) AutoListSecurityGroup(oldctx oldcontext.Context, t *api.ListWatchOptions, options ...grpc.CallOption) (*security.SecurityGroupList, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoListSecurityGroup")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*api.ListWatchOptions)
		return a.service.AutoListSecurityGroup(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.SecurityGroupList), err
}

func (a adapterSecurityV1) AutoListSgpolicy(oldctx oldcontext.Context, t *api.ListWatchOptions, options ...grpc.CallOption) (*security.SgpolicyList, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoListSgpolicy")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*api.ListWatchOptions)
		return a.service.AutoListSgpolicy(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.SgpolicyList), err
}

func (a adapterSecurityV1) AutoListTrafficEncryptionPolicy(oldctx oldcontext.Context, t *api.ListWatchOptions, options ...grpc.CallOption) (*security.TrafficEncryptionPolicyList, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoListTrafficEncryptionPolicy")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*api.ListWatchOptions)
		return a.service.AutoListTrafficEncryptionPolicy(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.TrafficEncryptionPolicyList), err
}

func (a adapterSecurityV1) AutoUpdateApp(oldctx oldcontext.Context, t *security.App, options ...grpc.CallOption) (*security.App, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoUpdateApp")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.App)
		return a.service.AutoUpdateApp(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.App), err
}

func (a adapterSecurityV1) AutoUpdateCertificate(oldctx oldcontext.Context, t *security.Certificate, options ...grpc.CallOption) (*security.Certificate, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoUpdateCertificate")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.Certificate)
		return a.service.AutoUpdateCertificate(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.Certificate), err
}

func (a adapterSecurityV1) AutoUpdateSecurityGroup(oldctx oldcontext.Context, t *security.SecurityGroup, options ...grpc.CallOption) (*security.SecurityGroup, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoUpdateSecurityGroup")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.SecurityGroup)
		return a.service.AutoUpdateSecurityGroup(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.SecurityGroup), err
}

func (a adapterSecurityV1) AutoUpdateSgpolicy(oldctx oldcontext.Context, t *security.Sgpolicy, options ...grpc.CallOption) (*security.Sgpolicy, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoUpdateSgpolicy")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.Sgpolicy)
		return a.service.AutoUpdateSgpolicy(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.Sgpolicy), err
}

func (a adapterSecurityV1) AutoUpdateTrafficEncryptionPolicy(oldctx oldcontext.Context, t *security.TrafficEncryptionPolicy, options ...grpc.CallOption) (*security.TrafficEncryptionPolicy, error) {
	// Not using options for now. Will be passed through context as needed.
	ctx := context.Context(oldctx)
	prof, err := a.gwSvc.GetServiceProfile("AutoUpdateTrafficEncryptionPolicy")
	if err != nil {
		return nil, errors.New("unknown service profile")
	}
	fn := func(ctx context.Context, i interface{}) (interface{}, error) {
		in := i.(*security.TrafficEncryptionPolicy)
		return a.service.AutoUpdateTrafficEncryptionPolicy(ctx, in)
	}
	ret, err := a.gw.HandleRequest(ctx, t, prof, fn)
	if ret == nil {
		return nil, err
	}
	return ret.(*security.TrafficEncryptionPolicy), err
}

func (a adapterSecurityV1) AutoWatchSvcSecurityV1(oldctx oldcontext.Context, in *api.ListWatchOptions, options ...grpc.CallOption) (security.SecurityV1_AutoWatchSvcSecurityV1Client, error) {
	ctx := context.Context(oldctx)
	return a.service.AutoWatchSvcSecurityV1(ctx, in)
}

func (a adapterSecurityV1) AutoWatchSecurityGroup(oldctx oldcontext.Context, in *api.ListWatchOptions, options ...grpc.CallOption) (security.SecurityV1_AutoWatchSecurityGroupClient, error) {
	ctx := context.Context(oldctx)
	return a.service.AutoWatchSecurityGroup(ctx, in)
}

func (a adapterSecurityV1) AutoWatchSgpolicy(oldctx oldcontext.Context, in *api.ListWatchOptions, options ...grpc.CallOption) (security.SecurityV1_AutoWatchSgpolicyClient, error) {
	ctx := context.Context(oldctx)
	return a.service.AutoWatchSgpolicy(ctx, in)
}

func (a adapterSecurityV1) AutoWatchApp(oldctx oldcontext.Context, in *api.ListWatchOptions, options ...grpc.CallOption) (security.SecurityV1_AutoWatchAppClient, error) {
	ctx := context.Context(oldctx)
	return a.service.AutoWatchApp(ctx, in)
}

func (a adapterSecurityV1) AutoWatchCertificate(oldctx oldcontext.Context, in *api.ListWatchOptions, options ...grpc.CallOption) (security.SecurityV1_AutoWatchCertificateClient, error) {
	ctx := context.Context(oldctx)
	return a.service.AutoWatchCertificate(ctx, in)
}

func (a adapterSecurityV1) AutoWatchTrafficEncryptionPolicy(oldctx oldcontext.Context, in *api.ListWatchOptions, options ...grpc.CallOption) (security.SecurityV1_AutoWatchTrafficEncryptionPolicyClient, error) {
	ctx := context.Context(oldctx)
	return a.service.AutoWatchTrafficEncryptionPolicy(ctx, in)
}

func (e *sSecurityV1GwService) setupSvcProfile() {
	e.defSvcProf = apigwpkg.NewServiceProfile(nil)
	e.defSvcProf.SetDefaults()
	e.svcProf = make(map[string]apigw.ServiceProfile)

	e.svcProf["AutoAddApp"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoAddCertificate"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoAddSecurityGroup"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoAddSgpolicy"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoAddTrafficEncryptionPolicy"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoDeleteApp"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoDeleteCertificate"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoDeleteSecurityGroup"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoDeleteSgpolicy"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoDeleteTrafficEncryptionPolicy"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoGetApp"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoGetCertificate"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoGetSecurityGroup"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoGetSgpolicy"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoGetTrafficEncryptionPolicy"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoListApp"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoListCertificate"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoListSecurityGroup"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoListSgpolicy"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoUpdateApp"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoUpdateCertificate"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoUpdateSecurityGroup"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoUpdateSgpolicy"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoUpdateTrafficEncryptionPolicy"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoWatchApp"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoWatchCertificate"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoWatchSecurityGroup"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoWatchSgpolicy"] = apigwpkg.NewServiceProfile(e.defSvcProf)
	e.svcProf["AutoWatchTrafficEncryptionPolicy"] = apigwpkg.NewServiceProfile(e.defSvcProf)
}

// GetDefaultServiceProfile returns the default fallback service profile for this service
func (e *sSecurityV1GwService) GetDefaultServiceProfile() (apigw.ServiceProfile, error) {
	if e.defSvcProf == nil {
		return nil, errors.New("not found")
	}
	return e.defSvcProf, nil
}

// GetServiceProfile returns the service profile for a given method in this service
func (e *sSecurityV1GwService) GetServiceProfile(method string) (apigw.ServiceProfile, error) {
	if ret, ok := e.svcProf[method]; ok {
		return ret, nil
	}
	return nil, errors.New("not found")
}

// GetCrudServiceProfile returns the service profile for a auto generated crud operation
func (e *sSecurityV1GwService) GetCrudServiceProfile(obj string, oper apiserver.APIOperType) (apigw.ServiceProfile, error) {
	name := apiserver.GetCrudServiceName(obj, oper)
	if name != "" {
		return e.GetServiceProfile(name)
	}
	return nil, errors.New("not found")
}

func (e *sSecurityV1GwService) CompleteRegistration(ctx context.Context,
	logger log.Logger,
	grpcserver *grpc.Server,
	m *http.ServeMux,
	rslvr resolver.Interface,
	wg *sync.WaitGroup) error {
	apigw := apigwpkg.MustGetAPIGateway()
	// IP:port destination or service discovery key.
	grpcaddr := "pen-apiserver"
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
		logger.ErrorLog("msg", "failed to register swagger spec", "service", "security.SecurityV1", "error", err)
	}
	wg.Add(1)
	go func() {
		defer wg.Done()
		for {
			nctx, cancel := context.WithCancel(ctx)
			cl, err := e.newClient(nctx, grpcaddr, rslvr, apigw.GetDevMode())
			if err == nil {
				muxMutex.Lock()
				err = security.RegisterSecurityV1HandlerWithClient(ctx, mux, cl)
				muxMutex.Unlock()
				if err == nil {
					logger.InfoLog("msg", "registered service security.SecurityV1")
					m.Handle("/configs/security/v1/", http.StripPrefix("/configs/security/v1", mux))
					return
				} else {
					err = errors.Wrap(err, "failed to register")
				}
			} else {
				err = errors.Wrap(err, "failed to create client")
			}
			cancel()
			logger.ErrorLog("msg", "failed to register", "service", "security.SecurityV1", "error", err)
			select {
			case <-ctx.Done():
				return
			case <-time.After(5 * time.Second):
			}
		}
	}()
	return nil
}

func (e *sSecurityV1GwService) newClient(ctx context.Context, grpcAddr string, rslvr resolver.Interface, devmode bool) (*adapterSecurityV1, error) {
	var opts []rpckit.Option
	if rslvr != nil {
		opts = append(opts, rpckit.WithBalancer(balancer.New(rslvr)))
	} else {
		opts = append(opts, rpckit.WithRemoteServerName("pen-apiserver"))
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
				e.logger.ErrorLog("msg", "Failed to close conn on Done()", "addr", grpcAddr, "error", cerr)
			}
		}()
	}()

	cl := &adapterSecurityV1{conn: client, gw: apigwpkg.MustGetAPIGateway(), gwSvc: e, service: grpcclient.NewSecurityV1Backend(client.ClientConn, e.logger)}
	return cl, nil
}

func init() {

	apigw := apigwpkg.MustGetAPIGateway()

	svcSecurityV1 := sSecurityV1GwService{}
	apigw.Register("security.SecurityV1", "security/", &svcSecurityV1)
}
