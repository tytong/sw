// {C} Copyright 2017 Pensando Systems Inc. All rights reserved.

package tlsproviders

import (
	"context"
	"crypto/tls"
	"crypto/x509"
	"fmt"
	"io/ioutil"
	"net"
	"sync"
	"time"

	"github.com/pkg/errors"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials"

	"github.com/pensando/sw/venice/cmd/grpc/server/certificates/certapi"
	"github.com/pensando/sw/venice/utils/certs"
	"github.com/pensando/sw/venice/utils/keymgr"
	"github.com/pensando/sw/venice/utils/log"
)

const (
	retryInterval = 500 * time.Millisecond
	maxRetries    = 5
)

// CMDBasedProvider is a TLS Provider which generates private keys locally and retrieves
// corresponding certificates from the Cluster Management Daemon (CMD)
type CMDBasedProvider struct {
	// the KeyMgr instance used to generate and store keys and certificates
	keyMgr *keymgr.KeyMgr

	// the remote URL of the CMD endpoint
	cmdEndpointURL string

	// conn is the gRPC client connection
	conn *grpc.ClientConn

	// the CMD gRPC client
	cmdClient certapi.CertificatesClient

	// Client/Server Name. Used when asking for certificate
	endpointID string

	// When providing credentials for a client, use the same certificate for all connections
	clientCertificate *tls.Certificate

	// When providing credentials for servers, mint a new certificate for each server
	// so that we can put the correct server name in the subject
	serverCertificates map[string](*tls.Certificate)

	// Lock for serverCertificates map
	srvCertMapMutex sync.Mutex

	// CaTrustChain is used to form the bundles presented to the peer
	caTrustChain []*x509.Certificate

	// Additional trust roots allows cluster endpoints to trust entities outside the cluster
	trustRoots *x509.CertPool

	// User-provided options to control the behavior of the provider
	cmdProviderOptions
}

type cmdProviderOptions struct {
	// A gRPC load-balancer to be used when dialing the CMD endpoint
	// At present it needs to be passed in explicitly.
	// In the future it will be instantiated automatically.
	balancer grpc.Balancer
}

// CMDProviderOption fills the optional params for CMDBasedProvider
type CMDProviderOption func(opt *cmdProviderOptions)

// WithBalancer passes a gRPC load-balancer to be used when dialing CMD
func WithBalancer(b grpc.Balancer) CMDProviderOption {
	return func(o *cmdProviderOptions) {
		o.balancer = b
	}
}

func (p *CMDBasedProvider) getCkmDialOptions() []grpc.DialOption {
	// The CMD API is not authenticated. We cannot use TLS because the API itself is meant to supply TLS certificates.
	dialOptions := []grpc.DialOption{grpc.WithInsecure(), grpc.WithBlock(), grpc.WithTimeout(time.Second * 3)}
	if p.balancer != nil {
		dialOptions = append(dialOptions, grpc.WithBalancer(p.balancer))
	}
	return dialOptions
}

func (p *CMDBasedProvider) fetchCaCertificates() error {
	// Fetch CA trust chain
	tcs, err := p.cmdClient.GetCaTrustChain(context.Background(), &certapi.Empty{})
	if err != nil {
		return errors.Wrap(err, "Error fetching CA trust chain")
	}
	for _, c := range tcs.GetCertificates() {
		c, err := x509.ParseCertificate(c.GetCertificate())
		if err != nil {
			return errors.Wrapf(err, "Error parsing intermediate certificate: %+v", c)
		}
		p.caTrustChain = append(p.caTrustChain, c)
	}

	// Fetch additional trust roots
	rootsResp, err := p.cmdClient.GetTrustRoots(context.Background(), &certapi.Empty{})
	if err != nil {
		return errors.Wrap(err, "Error fetching trust roots")
	}

	for _, r := range rootsResp.GetTrustRoots() {
		c, err := x509.ParseCertificate(r.GetCertificate())
		if err != nil {
			return errors.Wrap(err, "Received malformed trust roots")
		}
		p.trustRoots.AddCert(c)
	}

	return nil
}

func (p *CMDBasedProvider) getTLSCertificate(subjAltName string) (*tls.Certificate, error) {
	privateKey, err := p.keyMgr.GetObject(subjAltName, keymgr.ObjectTypeKeyPair)
	if err != nil {
		return nil, errors.Wrap(err, "Error reading key pair from keymgr")
	}
	if privateKey == nil {
		privateKey, err = p.keyMgr.CreateKeyPair(subjAltName, keymgr.ECDSA256)
		if err != nil {
			return nil, errors.Wrap(err, "Error generating private key")
		}
	}
	csr, err := certs.CreateCSR(privateKey, []string{subjAltName}, nil)
	if err != nil {
		return nil, errors.Wrap(err, "Error generating CSR")
	}

	// Get the CSR signed
	csrResp, err := p.cmdClient.SignCertificateRequest(context.Background(), &certapi.CertificateSignReq{Csr: csr.Raw})
	if err != nil {
		return nil, errors.Wrap(err, "Error issuing sign request")
	}

	// Parse certificate and create bundle
	cert, err := x509.ParseCertificate(csrResp.GetCertificate().GetCertificate())
	if err != nil {
		return nil, errors.Wrapf(err, "Error parsing certificate: %+v", csrResp.GetCertificate())
	}
	bundle := [][]byte{cert.Raw}
	for _, c := range p.caTrustChain {
		bundle = append(bundle, c.Raw)
	}

	return &tls.Certificate{
		PrivateKey:  privateKey,
		Certificate: bundle,
	}, nil
}

// NewCMDBasedProvider instantiates a new CMD-based TLS provider
func NewCMDBasedProvider(cmdEpNameOrURL, endpointID string, km *keymgr.KeyMgr, opts ...CMDProviderOption) (*CMDBasedProvider, error) {
	if cmdEpNameOrURL == "" {
		return nil, fmt.Errorf("Requires CMD endpoint name or URL in form hostname:port")
	}
	if endpointID == "" {
		return nil, fmt.Errorf("endpointID is required")
	}
	if km == nil {
		return nil, fmt.Errorf("Requires valid instance of KeyMgr")
	}

	provider := &CMDBasedProvider{
		keyMgr:             km,
		cmdEndpointURL:     cmdEpNameOrURL,
		endpointID:         endpointID,
		serverCertificates: make(map[string](*tls.Certificate)),
		trustRoots:         x509.NewCertPool(),
	}

	// add custom options
	for _, o := range opts {
		if o != nil {
			o(&provider.cmdProviderOptions)
		}
	}
	_, _, err := net.SplitHostPort(cmdEpNameOrURL)
	if err != nil && provider.balancer == nil {
		return nil, fmt.Errorf("Require a balancer to resolve %v", cmdEpNameOrURL)
	}

	// Connect to CMD Endpoint and create RPC client
	var success bool
	var conn *grpc.ClientConn
	for i := 0; i < maxRetries; i++ {
		log.Infof("Connecting to CMD Endpoint: %v", provider.cmdEndpointURL)
		conn, err = grpc.Dial(provider.cmdEndpointURL, provider.getCkmDialOptions()...)
		if err == nil {
			success = true
			provider.cmdClient = certapi.NewCertificatesClient(conn)
			break
		}
		time.Sleep(retryInterval)
	}
	if !success {
		return nil, errors.Wrapf(err, "Failed to dial CMD Endpoint %s", provider.cmdEndpointURL)
	}
	provider.conn = conn

	err = provider.fetchCaCertificates()
	if err != nil {
		log.Fatalf("Error fetching trust roots from %s: %v", cmdEpNameOrURL, err)
	}

	return provider, nil
}

// NewDefaultCMDBasedProvider instantiates a new CMD-based TLS provider using a keymgr with default backend
func NewDefaultCMDBasedProvider(cmdEpNameOrURL, endpointID string, opts ...CMDProviderOption) (*CMDBasedProvider, error) {
	workDir, err := ioutil.TempDir("", "tlsprovider-"+endpointID+"-")
	if err != nil {
		return nil, errors.Wrapf(err, "Error creating workdir for GoCrypto backend")
	}
	be, err := keymgr.NewGoCryptoBackend(workDir)
	if err != nil {
		return nil, errors.Wrapf(err, "Error instantiating GoCrypto backend")
	}
	km, err := keymgr.NewKeyMgr(be)
	if err != nil {
		be.Close()
		return nil, errors.Wrapf(err, "Error instantiating keymgr")
	}
	prov, err := NewCMDBasedProvider(cmdEpNameOrURL, endpointID, km, opts...)
	if err != nil {
		km.Close()
		return nil, errors.Wrapf(err, "Error instantiating keymgr")
	}
	return prov, nil
}

// getServerCertificate is the callback that returns server certificates
func (p *CMDBasedProvider) getServerCertificate(clientHelloInfo *tls.ClientHelloInfo) (*tls.Certificate, error) {
	// FIXME: we should not mint certificates based on what client is asking but
	// have server explicitly declare which names are allowed and provide default
	// if client is asking for something we don't have

	serverName := clientHelloInfo.ServerName
	if serverName == "" {
		serverName = p.endpointID
	}

	p.srvCertMapMutex.Lock()
	tlsCert := p.serverCertificates[serverName]
	p.srvCertMapMutex.Unlock()
	if tlsCert == nil {
		var err error
		tlsCert, err = p.getTLSCertificate(serverName)
		if err != nil {
			return nil, fmt.Errorf("Error getting dial options for server %s: %v", serverName, err)
		}
		p.srvCertMapMutex.Lock()
		p.serverCertificates[serverName] = tlsCert
		p.srvCertMapMutex.Unlock()
	}

	return tlsCert, nil
}

// GetServerOptions returns server options to be passed to grpc.NewServer()
func (p *CMDBasedProvider) GetServerOptions(serverName string) (grpc.ServerOption, error) {
	tlsConfig := getTLSServerConfig(serverName, nil, p.trustRoots)
	// Set callback to be invoked whenever a new connection is established
	// This enables certificate rotation
	tlsConfig.GetCertificate = p.getServerCertificate
	return grpc.Creds(credentials.NewTLS(tlsConfig)), nil
}

// GetDialOptions returns dial options to be passed to grpc.Dial()
func (p *CMDBasedProvider) GetDialOptions(serverName string) (grpc.DialOption, error) {
	// Golang 1.8.3 and subsequent support a GetClientCertificate option to allow a client to
	// pick a certificate based on the server's certificate request. However, with gRPC this
	// doesn't work, because gRPC currently copies tls.Config structures using a naive,
	// outdated implementation that discards GetClientCertificate
	// (see https://github.com/golang/go/issues/15771)
	// It is not a big problem because DialOptions are associated to a single outgoing connection
	// (as opposed to ServerOptions, which are used for all incoming connections) and rpckit
	// reinvokes GetDialOptions() if client performs a Reconnect() call on an existing RPCClient
	if p.clientCertificate == nil {
		cert, err := p.getTLSCertificate(p.endpointID)
		if err != nil {
			return nil, errors.Wrap(err, "Error retrieving client certificate")
		}
		p.clientCertificate = cert
	}

	tlsConfig := getTLSClientConfig(serverName, p.clientCertificate, p.trustRoots)
	return grpc.WithTransportCredentials(credentials.NewTLS(tlsConfig)), nil
}

// Close closes the client.
func (p *CMDBasedProvider) Close() {
	if p.conn != nil {
		log.Infof("Closing client conn: %+v", p.conn)
		p.conn.Close()
		p.conn = nil
	}
	if p.keyMgr != nil {
		p.keyMgr.Close()
		p.keyMgr = nil
	}
}
