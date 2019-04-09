// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package auth is a auto generated package.
Input file: auth.proto
*/
package auth

import (
	"context"
	"encoding/json"
	"net/http"

	"github.com/pensando/sw/api"
)

// Dummy definitions to suppress nonused warnings
var _ api.ObjectMeta

func encodeHTTPAuthenticationPolicy(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPAuthenticationPolicy(_ context.Context, r *http.Request) (interface{}, error) {
	var req AuthenticationPolicy
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqAuthenticationPolicy encodes GRPC request
func EncodeGrpcReqAuthenticationPolicy(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*AuthenticationPolicy)
	return req, nil
}

// DecodeGrpcReqAuthenticationPolicy decodes GRPC request
func DecodeGrpcReqAuthenticationPolicy(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*AuthenticationPolicy)
	return req, nil
}

// EncodeGrpcRespAuthenticationPolicy encodes GRC response
func EncodeGrpcRespAuthenticationPolicy(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespAuthenticationPolicy decodes GRPC response
func DecodeGrpcRespAuthenticationPolicy(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPAuthenticationPolicySpec(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPAuthenticationPolicySpec(_ context.Context, r *http.Request) (interface{}, error) {
	var req AuthenticationPolicySpec
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqAuthenticationPolicySpec encodes GRPC request
func EncodeGrpcReqAuthenticationPolicySpec(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*AuthenticationPolicySpec)
	return req, nil
}

// DecodeGrpcReqAuthenticationPolicySpec decodes GRPC request
func DecodeGrpcReqAuthenticationPolicySpec(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*AuthenticationPolicySpec)
	return req, nil
}

// EncodeGrpcRespAuthenticationPolicySpec encodes GRC response
func EncodeGrpcRespAuthenticationPolicySpec(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespAuthenticationPolicySpec decodes GRPC response
func DecodeGrpcRespAuthenticationPolicySpec(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPAuthenticationPolicyStatus(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPAuthenticationPolicyStatus(_ context.Context, r *http.Request) (interface{}, error) {
	var req AuthenticationPolicyStatus
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqAuthenticationPolicyStatus encodes GRPC request
func EncodeGrpcReqAuthenticationPolicyStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*AuthenticationPolicyStatus)
	return req, nil
}

// DecodeGrpcReqAuthenticationPolicyStatus decodes GRPC request
func DecodeGrpcReqAuthenticationPolicyStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*AuthenticationPolicyStatus)
	return req, nil
}

// EncodeGrpcRespAuthenticationPolicyStatus encodes GRC response
func EncodeGrpcRespAuthenticationPolicyStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespAuthenticationPolicyStatus decodes GRPC response
func DecodeGrpcRespAuthenticationPolicyStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPAuthenticators(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPAuthenticators(_ context.Context, r *http.Request) (interface{}, error) {
	var req Authenticators
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqAuthenticators encodes GRPC request
func EncodeGrpcReqAuthenticators(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Authenticators)
	return req, nil
}

// DecodeGrpcReqAuthenticators decodes GRPC request
func DecodeGrpcReqAuthenticators(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Authenticators)
	return req, nil
}

// EncodeGrpcRespAuthenticators encodes GRC response
func EncodeGrpcRespAuthenticators(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespAuthenticators decodes GRPC response
func DecodeGrpcRespAuthenticators(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPLdap(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPLdap(_ context.Context, r *http.Request) (interface{}, error) {
	var req Ldap
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqLdap encodes GRPC request
func EncodeGrpcReqLdap(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Ldap)
	return req, nil
}

// DecodeGrpcReqLdap decodes GRPC request
func DecodeGrpcReqLdap(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Ldap)
	return req, nil
}

// EncodeGrpcRespLdap encodes GRC response
func EncodeGrpcRespLdap(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespLdap decodes GRPC response
func DecodeGrpcRespLdap(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPLdapAttributeMapping(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPLdapAttributeMapping(_ context.Context, r *http.Request) (interface{}, error) {
	var req LdapAttributeMapping
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqLdapAttributeMapping encodes GRPC request
func EncodeGrpcReqLdapAttributeMapping(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*LdapAttributeMapping)
	return req, nil
}

// DecodeGrpcReqLdapAttributeMapping decodes GRPC request
func DecodeGrpcReqLdapAttributeMapping(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*LdapAttributeMapping)
	return req, nil
}

// EncodeGrpcRespLdapAttributeMapping encodes GRC response
func EncodeGrpcRespLdapAttributeMapping(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespLdapAttributeMapping decodes GRPC response
func DecodeGrpcRespLdapAttributeMapping(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPLdapServer(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPLdapServer(_ context.Context, r *http.Request) (interface{}, error) {
	var req LdapServer
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqLdapServer encodes GRPC request
func EncodeGrpcReqLdapServer(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*LdapServer)
	return req, nil
}

// DecodeGrpcReqLdapServer decodes GRPC request
func DecodeGrpcReqLdapServer(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*LdapServer)
	return req, nil
}

// EncodeGrpcRespLdapServer encodes GRC response
func EncodeGrpcRespLdapServer(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespLdapServer decodes GRPC response
func DecodeGrpcRespLdapServer(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPLdapServerStatus(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPLdapServerStatus(_ context.Context, r *http.Request) (interface{}, error) {
	var req LdapServerStatus
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqLdapServerStatus encodes GRPC request
func EncodeGrpcReqLdapServerStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*LdapServerStatus)
	return req, nil
}

// DecodeGrpcReqLdapServerStatus decodes GRPC request
func DecodeGrpcReqLdapServerStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*LdapServerStatus)
	return req, nil
}

// EncodeGrpcRespLdapServerStatus encodes GRC response
func EncodeGrpcRespLdapServerStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespLdapServerStatus decodes GRPC response
func DecodeGrpcRespLdapServerStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPLocal(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPLocal(_ context.Context, r *http.Request) (interface{}, error) {
	var req Local
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqLocal encodes GRPC request
func EncodeGrpcReqLocal(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Local)
	return req, nil
}

// DecodeGrpcReqLocal decodes GRPC request
func DecodeGrpcReqLocal(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Local)
	return req, nil
}

// EncodeGrpcRespLocal encodes GRC response
func EncodeGrpcRespLocal(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespLocal decodes GRPC response
func DecodeGrpcRespLocal(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPOperation(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPOperation(_ context.Context, r *http.Request) (interface{}, error) {
	var req Operation
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqOperation encodes GRPC request
func EncodeGrpcReqOperation(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Operation)
	return req, nil
}

// DecodeGrpcReqOperation decodes GRPC request
func DecodeGrpcReqOperation(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Operation)
	return req, nil
}

// EncodeGrpcRespOperation encodes GRC response
func EncodeGrpcRespOperation(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespOperation decodes GRPC response
func DecodeGrpcRespOperation(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPOperationStatus(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPOperationStatus(_ context.Context, r *http.Request) (interface{}, error) {
	var req OperationStatus
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqOperationStatus encodes GRPC request
func EncodeGrpcReqOperationStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*OperationStatus)
	return req, nil
}

// DecodeGrpcReqOperationStatus decodes GRPC request
func DecodeGrpcReqOperationStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*OperationStatus)
	return req, nil
}

// EncodeGrpcRespOperationStatus encodes GRC response
func EncodeGrpcRespOperationStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespOperationStatus decodes GRPC response
func DecodeGrpcRespOperationStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPPasswordChangeRequest(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPPasswordChangeRequest(_ context.Context, r *http.Request) (interface{}, error) {
	var req PasswordChangeRequest
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqPasswordChangeRequest encodes GRPC request
func EncodeGrpcReqPasswordChangeRequest(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*PasswordChangeRequest)
	return req, nil
}

// DecodeGrpcReqPasswordChangeRequest decodes GRPC request
func DecodeGrpcReqPasswordChangeRequest(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*PasswordChangeRequest)
	return req, nil
}

// EncodeGrpcRespPasswordChangeRequest encodes GRC response
func EncodeGrpcRespPasswordChangeRequest(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespPasswordChangeRequest decodes GRPC response
func DecodeGrpcRespPasswordChangeRequest(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPPasswordCredential(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPPasswordCredential(_ context.Context, r *http.Request) (interface{}, error) {
	var req PasswordCredential
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqPasswordCredential encodes GRPC request
func EncodeGrpcReqPasswordCredential(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*PasswordCredential)
	return req, nil
}

// DecodeGrpcReqPasswordCredential decodes GRPC request
func DecodeGrpcReqPasswordCredential(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*PasswordCredential)
	return req, nil
}

// EncodeGrpcRespPasswordCredential encodes GRC response
func EncodeGrpcRespPasswordCredential(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespPasswordCredential decodes GRPC response
func DecodeGrpcRespPasswordCredential(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPPasswordResetRequest(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPPasswordResetRequest(_ context.Context, r *http.Request) (interface{}, error) {
	var req PasswordResetRequest
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqPasswordResetRequest encodes GRPC request
func EncodeGrpcReqPasswordResetRequest(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*PasswordResetRequest)
	return req, nil
}

// DecodeGrpcReqPasswordResetRequest decodes GRPC request
func DecodeGrpcReqPasswordResetRequest(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*PasswordResetRequest)
	return req, nil
}

// EncodeGrpcRespPasswordResetRequest encodes GRC response
func EncodeGrpcRespPasswordResetRequest(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespPasswordResetRequest decodes GRPC response
func DecodeGrpcRespPasswordResetRequest(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPPermission(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPPermission(_ context.Context, r *http.Request) (interface{}, error) {
	var req Permission
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqPermission encodes GRPC request
func EncodeGrpcReqPermission(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Permission)
	return req, nil
}

// DecodeGrpcReqPermission decodes GRPC request
func DecodeGrpcReqPermission(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Permission)
	return req, nil
}

// EncodeGrpcRespPermission encodes GRC response
func EncodeGrpcRespPermission(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespPermission decodes GRPC response
func DecodeGrpcRespPermission(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPRadius(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPRadius(_ context.Context, r *http.Request) (interface{}, error) {
	var req Radius
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqRadius encodes GRPC request
func EncodeGrpcReqRadius(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Radius)
	return req, nil
}

// DecodeGrpcReqRadius decodes GRPC request
func DecodeGrpcReqRadius(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Radius)
	return req, nil
}

// EncodeGrpcRespRadius encodes GRC response
func EncodeGrpcRespRadius(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespRadius decodes GRPC response
func DecodeGrpcRespRadius(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPRadiusServer(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPRadiusServer(_ context.Context, r *http.Request) (interface{}, error) {
	var req RadiusServer
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqRadiusServer encodes GRPC request
func EncodeGrpcReqRadiusServer(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RadiusServer)
	return req, nil
}

// DecodeGrpcReqRadiusServer decodes GRPC request
func DecodeGrpcReqRadiusServer(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RadiusServer)
	return req, nil
}

// EncodeGrpcRespRadiusServer encodes GRC response
func EncodeGrpcRespRadiusServer(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespRadiusServer decodes GRPC response
func DecodeGrpcRespRadiusServer(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPRadiusServerStatus(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPRadiusServerStatus(_ context.Context, r *http.Request) (interface{}, error) {
	var req RadiusServerStatus
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqRadiusServerStatus encodes GRPC request
func EncodeGrpcReqRadiusServerStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RadiusServerStatus)
	return req, nil
}

// DecodeGrpcReqRadiusServerStatus decodes GRPC request
func DecodeGrpcReqRadiusServerStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RadiusServerStatus)
	return req, nil
}

// EncodeGrpcRespRadiusServerStatus encodes GRC response
func EncodeGrpcRespRadiusServerStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespRadiusServerStatus decodes GRPC response
func DecodeGrpcRespRadiusServerStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPResource(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPResource(_ context.Context, r *http.Request) (interface{}, error) {
	var req Resource
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqResource encodes GRPC request
func EncodeGrpcReqResource(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Resource)
	return req, nil
}

// DecodeGrpcReqResource decodes GRPC request
func DecodeGrpcReqResource(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Resource)
	return req, nil
}

// EncodeGrpcRespResource encodes GRC response
func EncodeGrpcRespResource(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespResource decodes GRPC response
func DecodeGrpcRespResource(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPRole(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPRole(_ context.Context, r *http.Request) (interface{}, error) {
	var req Role
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqRole encodes GRPC request
func EncodeGrpcReqRole(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Role)
	return req, nil
}

// DecodeGrpcReqRole decodes GRPC request
func DecodeGrpcReqRole(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*Role)
	return req, nil
}

// EncodeGrpcRespRole encodes GRC response
func EncodeGrpcRespRole(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespRole decodes GRPC response
func DecodeGrpcRespRole(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPRoleBinding(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPRoleBinding(_ context.Context, r *http.Request) (interface{}, error) {
	var req RoleBinding
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqRoleBinding encodes GRPC request
func EncodeGrpcReqRoleBinding(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RoleBinding)
	return req, nil
}

// DecodeGrpcReqRoleBinding decodes GRPC request
func DecodeGrpcReqRoleBinding(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RoleBinding)
	return req, nil
}

// EncodeGrpcRespRoleBinding encodes GRC response
func EncodeGrpcRespRoleBinding(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespRoleBinding decodes GRPC response
func DecodeGrpcRespRoleBinding(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPRoleBindingSpec(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPRoleBindingSpec(_ context.Context, r *http.Request) (interface{}, error) {
	var req RoleBindingSpec
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqRoleBindingSpec encodes GRPC request
func EncodeGrpcReqRoleBindingSpec(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RoleBindingSpec)
	return req, nil
}

// DecodeGrpcReqRoleBindingSpec decodes GRPC request
func DecodeGrpcReqRoleBindingSpec(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RoleBindingSpec)
	return req, nil
}

// EncodeGrpcRespRoleBindingSpec encodes GRC response
func EncodeGrpcRespRoleBindingSpec(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespRoleBindingSpec decodes GRPC response
func DecodeGrpcRespRoleBindingSpec(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPRoleBindingStatus(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPRoleBindingStatus(_ context.Context, r *http.Request) (interface{}, error) {
	var req RoleBindingStatus
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqRoleBindingStatus encodes GRPC request
func EncodeGrpcReqRoleBindingStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RoleBindingStatus)
	return req, nil
}

// DecodeGrpcReqRoleBindingStatus decodes GRPC request
func DecodeGrpcReqRoleBindingStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RoleBindingStatus)
	return req, nil
}

// EncodeGrpcRespRoleBindingStatus encodes GRC response
func EncodeGrpcRespRoleBindingStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespRoleBindingStatus decodes GRPC response
func DecodeGrpcRespRoleBindingStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPRoleSpec(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPRoleSpec(_ context.Context, r *http.Request) (interface{}, error) {
	var req RoleSpec
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqRoleSpec encodes GRPC request
func EncodeGrpcReqRoleSpec(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RoleSpec)
	return req, nil
}

// DecodeGrpcReqRoleSpec decodes GRPC request
func DecodeGrpcReqRoleSpec(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RoleSpec)
	return req, nil
}

// EncodeGrpcRespRoleSpec encodes GRC response
func EncodeGrpcRespRoleSpec(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespRoleSpec decodes GRPC response
func DecodeGrpcRespRoleSpec(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPRoleStatus(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPRoleStatus(_ context.Context, r *http.Request) (interface{}, error) {
	var req RoleStatus
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqRoleStatus encodes GRPC request
func EncodeGrpcReqRoleStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RoleStatus)
	return req, nil
}

// DecodeGrpcReqRoleStatus decodes GRPC request
func DecodeGrpcReqRoleStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RoleStatus)
	return req, nil
}

// EncodeGrpcRespRoleStatus encodes GRC response
func EncodeGrpcRespRoleStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespRoleStatus decodes GRPC response
func DecodeGrpcRespRoleStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPSubjectAccessReviewRequest(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPSubjectAccessReviewRequest(_ context.Context, r *http.Request) (interface{}, error) {
	var req SubjectAccessReviewRequest
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqSubjectAccessReviewRequest encodes GRPC request
func EncodeGrpcReqSubjectAccessReviewRequest(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*SubjectAccessReviewRequest)
	return req, nil
}

// DecodeGrpcReqSubjectAccessReviewRequest decodes GRPC request
func DecodeGrpcReqSubjectAccessReviewRequest(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*SubjectAccessReviewRequest)
	return req, nil
}

// EncodeGrpcRespSubjectAccessReviewRequest encodes GRC response
func EncodeGrpcRespSubjectAccessReviewRequest(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespSubjectAccessReviewRequest decodes GRPC response
func DecodeGrpcRespSubjectAccessReviewRequest(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPTLSOptions(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPTLSOptions(_ context.Context, r *http.Request) (interface{}, error) {
	var req TLSOptions
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqTLSOptions encodes GRPC request
func EncodeGrpcReqTLSOptions(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*TLSOptions)
	return req, nil
}

// DecodeGrpcReqTLSOptions decodes GRPC request
func DecodeGrpcReqTLSOptions(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*TLSOptions)
	return req, nil
}

// EncodeGrpcRespTLSOptions encodes GRC response
func EncodeGrpcRespTLSOptions(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespTLSOptions decodes GRPC response
func DecodeGrpcRespTLSOptions(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPUser(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPUser(_ context.Context, r *http.Request) (interface{}, error) {
	var req User
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqUser encodes GRPC request
func EncodeGrpcReqUser(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*User)
	return req, nil
}

// DecodeGrpcReqUser decodes GRPC request
func DecodeGrpcReqUser(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*User)
	return req, nil
}

// EncodeGrpcRespUser encodes GRC response
func EncodeGrpcRespUser(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespUser decodes GRPC response
func DecodeGrpcRespUser(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPUserSpec(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPUserSpec(_ context.Context, r *http.Request) (interface{}, error) {
	var req UserSpec
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqUserSpec encodes GRPC request
func EncodeGrpcReqUserSpec(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*UserSpec)
	return req, nil
}

// DecodeGrpcReqUserSpec decodes GRPC request
func DecodeGrpcReqUserSpec(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*UserSpec)
	return req, nil
}

// EncodeGrpcRespUserSpec encodes GRC response
func EncodeGrpcRespUserSpec(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespUserSpec decodes GRPC response
func DecodeGrpcRespUserSpec(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPUserStatus(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPUserStatus(_ context.Context, r *http.Request) (interface{}, error) {
	var req UserStatus
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqUserStatus encodes GRPC request
func EncodeGrpcReqUserStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*UserStatus)
	return req, nil
}

// DecodeGrpcReqUserStatus decodes GRPC request
func DecodeGrpcReqUserStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*UserStatus)
	return req, nil
}

// EncodeGrpcRespUserStatus encodes GRC response
func EncodeGrpcRespUserStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespUserStatus decodes GRPC response
func DecodeGrpcRespUserStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}
