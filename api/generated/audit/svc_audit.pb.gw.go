// Code generated by protoc-gen-grpc-gateway
// source: svc_audit.proto
// DO NOT EDIT!

/*
Package audit is a reverse proxy.

It translates gRPC into RESTful JSON APIs.
*/
package audit

import (
	"bytes"
	"io"
	"net/http"

	"github.com/gogo/protobuf/proto"
	"github.com/pensando/grpc-gateway/runtime"
	"github.com/pensando/grpc-gateway/utilities"
	"golang.org/x/net/context"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/grpclog"

	"github.com/pensando/sw/api/utils"
)

var _ codes.Code
var _ io.Reader
var _ = runtime.String
var _ = utilities.NewDoubleArray
var _ = apiutils.CtxKeyObjKind

func request_AuditV1_GetEvent_0(ctx context.Context, marshaler runtime.Marshaler, client AuditV1Client, req *http.Request, pathParams map[string]string) (proto.Message, runtime.ServerMetadata, error) {
	protoReq := &AuditEventRequest{}
	var smetadata runtime.ServerMetadata

	ver := req.Header.Get("Grpc-Metadata-Req-Version")
	if ver == "" {
		ver = "all"
	}
	if req.ContentLength != 0 {
		var buf bytes.Buffer
		tee := io.TeeReader(req.Body, &buf)
		if err := marshaler.NewDecoder(tee).Decode(protoReq); err != nil {
			return nil, smetadata, grpc.Errorf(codes.InvalidArgument, "%v", err)
		}
		changed := protoReq.Defaults(ver)
		if changed {
			if err := marshaler.NewDecoder(&buf).Decode(protoReq); err != nil {
				return nil, smetadata, grpc.Errorf(codes.InvalidArgument, "%v", err)
			}
		}
	} else {
		protoReq.Defaults(ver)
	}

	var (
		val   string
		ok    bool
		err   error
		_                       = err
		kvMap map[string]string = make(map[string]string)
	)

	val, ok = pathParams["UUID"]
	if !ok {
		return nil, smetadata, grpc.Errorf(codes.InvalidArgument, "missing parameter %s", "UUID")
	}

	protoReq.UUID, err = runtime.String(val)

	if err != nil {
		return nil, smetadata, err
	}

	ctx = runtime.PopulateContextKV(ctx, kvMap)

	msg, err := client.GetEvent(ctx, protoReq, grpc.Header(&smetadata.HeaderMD), grpc.Trailer(&smetadata.TrailerMD))
	return msg, smetadata, err

}

// RegisterAuditV1HandlerFromEndpoint is same as RegisterAuditV1Handler but
// automatically dials to "endpoint" and closes the connection when "ctx" gets done.
func RegisterAuditV1HandlerFromEndpoint(ctx context.Context, mux *runtime.ServeMux, endpoint string, opts []grpc.DialOption) (err error) {
	conn, err := grpc.Dial(endpoint, opts...)
	if err != nil {
		return err
	}
	defer func() {
		if err != nil {
			if cerr := conn.Close(); cerr != nil {
				grpclog.Printf("Failed to close conn to %s: %v", endpoint, cerr)
			}
			return
		}
		go func() {
			<-ctx.Done()
			if cerr := conn.Close(); cerr != nil {
				grpclog.Printf("Failed to close conn to %s: %v", endpoint, cerr)
			}
		}()
	}()

	return RegisterAuditV1Handler(ctx, mux, conn)
}

// RegisterAuditV1Handler registers the http handlers for service AuditV1 to "mux".
// The handlers forward requests to the grpc endpoint over "conn".
func RegisterAuditV1Handler(ctx context.Context, mux *runtime.ServeMux, conn *grpc.ClientConn) error {
	client := NewAuditV1Client(conn)
	return RegisterAuditV1HandlerWithClient(ctx, mux, client)
}

// RegisterAuditV1HandlerClient registers the http handlers for service AuditV1 to "mux".
// The handlers forward requests to the grpc endpoint using client provided.
func RegisterAuditV1HandlerWithClient(ctx context.Context, mux *runtime.ServeMux, client AuditV1Client) error {

	mux.Handle("GET", pattern_AuditV1_GetEvent_0, func(w http.ResponseWriter, req *http.Request, pathParams map[string]string) {
		ctx, cancel := context.WithCancel(req.Context())
		defer cancel()
		if cn, ok := w.(http.CloseNotifier); ok {
			go func(done <-chan struct{}, closed <-chan bool) {
				select {
				case <-done:
				case <-closed:
					cancel()
				}
			}(ctx.Done(), cn.CloseNotify())
		}
		inboundMarshaler, outboundMarshaler := runtime.MarshalerForRequest(mux, req)
		rctx, err := runtime.AnnotateContext(ctx, req)
		if err != nil {
			runtime.HTTPError(ctx, outboundMarshaler, w, req, err)
		}
		resp, md, err := request_AuditV1_GetEvent_0(rctx, inboundMarshaler, client, req, pathParams)
		ctx = runtime.NewServerMetadataContext(ctx, md)
		if err != nil {
			runtime.HTTPError(ctx, outboundMarshaler, w, req, err)
			return
		}

		forward_AuditV1_GetEvent_0(ctx, outboundMarshaler, w, req, resp, mux.GetForwardResponseOptions()...)

	})

	return nil
}

var (
	pattern_AuditV1_GetEvent_0 = runtime.MustPattern(runtime.NewPattern(1, []int{2, 0, 1, 0, 4, 1, 5, 1}, []string{"events", "UUID"}, ""))
)

var (
	forward_AuditV1_GetEvent_0 = runtime.ForwardResponseMessage
)
