package ipif

import (
	"bytes"
	"math/rand"
	"net"
	"os"
	"strings"
	"testing"
	"time"

	"github.com/pensando/sw/api/generated/cluster"

	"github.com/pensando/sw/nic/agent/nmd/mock"

	dhcp "github.com/krolaw/dhcp4"
	"github.com/krolaw/dhcp4/conn"
	"github.com/vishvananda/netlink"

	"github.com/pensando/sw/venice/utils/log"
	. "github.com/pensando/sw/venice/utils/testutils"
)

const (
	dhcpServerIntf = "srv-dhcp"
	ipAddrStart    = "172.16.10.2"
	testSubnet     = "172.16.10.0/28"
	leaseDuration  = time.Second * 10
	noDHCP         = iota
	configureNoVendorAtrrs
	configureEmptyVendorAtrs
	configureValidVendorAttrs
	configureMalformedVendorAttrs
	configureMalformedVendorAttrsMismatchOption43And60
	configureValidVendorAttrs241
	configureValidVendorAttrs241Multiple
)

var (
	staticIPConfig = cluster.IPConfig{
		IPAddress:  "172.16.10.10/28",
		DNSServers: []string{"172.16.10.1", "172.16.10.2"},
	}
	_, allocSubnet, _          = net.ParseCIDR(testSubnet)
	veniceIPs                  = "42.42.42.42,84.84.84.84"
	option241VeniceIPs         = "1.1.1.1"
	option241VeniceIPsMultiple = "2.2.2.2,3.3.3.3"
)

type dhcpSrv struct {
	conn net.PacketConn
}

func TestMain(m *testing.M) {
	exitCode := m.Run()
	os.Exit(exitCode)
}

//++++++++++++++++++++++++++++ Happy Path Test Cases ++++++++++++++++++++++++++++++++++++++++

func TestIPClient_DoStaticConfig(t *testing.T) {
	var d dhcpSrv
	err := d.setup(noDHCP)
	AssertOk(t, err, "Setup Failed")
	defer d.tearDown()
	mockNMD := mock.CreateMockNMD(t.Name())
	ipClient, err := NewIPClient(mockNMD, NaplesMockInterface)
	AssertOk(t, err, "IPClient creates must succeed")
	// Set the IP Config
	mockNMD.SetIPConfig(&staticIPConfig)
	ipAddr, err := ipClient.DoStaticConfig()
	AssertOk(t, err, "Failed to assign ip address statically to a mock interface")
	Assert(t, ipAddr == mockNMD.GetIPConfig().IPAddress, "Got in correct assigned IP Address")
}

func TestDHCPSpecControllers(t *testing.T) {
	var d dhcpSrv
	err := d.setup(configureNoVendorAtrrs)
	AssertOk(t, err, "Setup Failed")
	defer d.tearDown()
	AssertOk(t, err, "Failed to start DHCP Server for testing")
	mockNMD := mock.CreateMockNMD(t.Name())
	ipClient, err := NewIPClient(mockNMD, NaplesMockInterface)
	AssertOk(t, err, "IPClient creates must succeed")
	err = ipClient.DoDHCPConfig()
	// Check DHCP Config should succeed
	AssertOk(t, err, "Failed to perform DHCP")

	// Ensure obtained IP Addr is in the allocated subnet
	AssertEquals(t, true, allocSubnet.Contains(ipClient.dhcpState.IPNet.IP), "Obtained a YIADDR is not in the expected subnet")

	// Ensure dhcp state is missing vendor attributes
	AssertEquals(t, dhcpDone.String(), ipClient.dhcpState.CurState, "DHCP State must be done as the vendor options are specified via spec")

	// Ensure that there are no VeniceIPs
	AssertEquals(t, true, ipClient.dhcpState.VeniceIPs["1.1.1.1"], " dhcp venice IPs must match static controllers")

	// Ensure the IP Assigned on the interface is indeed the YIADDR
	ipAddr, err := netlink.AddrList(ipClient.intf, netlink.FAMILY_V4)
	AssertOk(t, err, "Must be able to look up IP Address for mock interface")
	var found bool
	for _, a := range ipAddr {
		if a.IP.Equal(ipClient.dhcpState.IPNet.IP) {
			found = true
			break
		}
	}
	AssertEquals(t, true, found, "The interface IP Address should match YIADDR")
}

func TestDHCPValidVendorAttributes241Code(t *testing.T) {
	var d dhcpSrv
	err := d.setup(configureValidVendorAttrs241)
	AssertOk(t, err, "Setup Failed")
	defer d.tearDown()
	AssertOk(t, err, "Failed to start DHCP Server for testing")
	mockNMD := mock.CreateMockNMD(t.Name())
	ipClient, err := NewIPClient(mockNMD, NaplesMockInterface)
	AssertOk(t, err, "IPClient creates must succeed")

	// Clear spec controllers
	mockNMD.Naples.Spec.Controllers = []string{}
	err = ipClient.DoDHCPConfig()
	// Check DHCP Config should succeed
	AssertOk(t, err, "Failed to perform DHCP")

	// Ensure obtained IP Addr is in the allocated subnet
	AssertEquals(t, true, allocSubnet.Contains(ipClient.dhcpState.IPNet.IP), "Obtained a YIADDR is not in the expected subnet")

	// Ensure dhcp state is missing vendor attributes
	AssertEquals(t, dhcpDone.String(), ipClient.dhcpState.CurState, "DHCP State must reflect DHCP Done")

	// Ensure that there are expected Venice IPs
	veniceIPs := strings.Split(option241VeniceIPs, ",")
	for _, v := range veniceIPs {
		AssertEquals(t, true, ipClient.dhcpState.VeniceIPs[v], "Failed to find a Venice IP. %v", v)
	}

	// Ensure the IP Assigned on the interface is indeed the YIADDR
	ipAddr, err := netlink.AddrList(ipClient.intf, netlink.FAMILY_V4)
	AssertOk(t, err, "Must be able to look up IP Address for mock interface")
	var found bool
	for _, a := range ipAddr {
		if a.IP.Equal(ipClient.dhcpState.IPNet.IP) {
			found = true
			break
		}
	}
	AssertEquals(t, true, found, "The interface IP Address should match YIADDR")
}

func TestDHCPValidVendorAttributes241CodeMultiple(t *testing.T) {
	var d dhcpSrv
	err := d.setup(configureValidVendorAttrs241Multiple)
	AssertOk(t, err, "Setup Failed")
	defer d.tearDown()
	AssertOk(t, err, "Failed to start DHCP Server for testing")
	mockNMD := mock.CreateMockNMD(t.Name())
	ipClient, err := NewIPClient(mockNMD, NaplesMockInterface)
	AssertOk(t, err, "IPClient creates must succeed")

	// Clear spec controllers
	mockNMD.Naples.Spec.Controllers = []string{}
	err = ipClient.DoDHCPConfig()
	// Check DHCP Config should succeed
	AssertOk(t, err, "Failed to perform DHCP")

	// Ensure obtained IP Addr is in the allocated subnet
	AssertEquals(t, true, allocSubnet.Contains(ipClient.dhcpState.IPNet.IP), "Obtained a YIADDR is not in the expected subnet")

	// Ensure dhcp state is missing vendor attributes
	AssertEquals(t, dhcpDone.String(), ipClient.dhcpState.CurState, "DHCP State must reflect DHCP Done")

	// Ensure that there are expected Venice IPs
	veniceIPs := strings.Split(option241VeniceIPsMultiple, ",")
	for _, v := range veniceIPs {
		AssertEquals(t, true, ipClient.dhcpState.VeniceIPs[v], "Failed to find a Venice IP. %v", v)
	}

	// Ensure the IP Assigned on the interface is indeed the YIADDR
	ipAddr, err := netlink.AddrList(ipClient.intf, netlink.FAMILY_V4)
	AssertOk(t, err, "Must be able to look up IP Address for mock interface")
	var found bool
	for _, a := range ipAddr {
		if a.IP.Equal(ipClient.dhcpState.IPNet.IP) {
			found = true
			break
		}
	}
	AssertEquals(t, true, found, "The interface IP Address should match YIADDR")
}

//++++++++++++++++++++++++++++ Corner Test Cases ++++++++++++++++++++++++++++++++++++++++

func TestDHCPValidVendorAttributes(t *testing.T) {
	var d dhcpSrv
	err := d.setup(configureValidVendorAttrs)
	AssertOk(t, err, "Setup Failed")
	defer d.tearDown()
	AssertOk(t, err, "Failed to start DHCP Server for testing")
	mockNMD := mock.CreateMockNMD(t.Name())
	ipClient, err := NewIPClient(mockNMD, NaplesMockInterface)
	AssertOk(t, err, "IPClient creates must succeed")

	// Clear spec controllers
	mockNMD.Naples.Spec.Controllers = []string{}
	err = ipClient.DoDHCPConfig()
	// Check DHCP Config should succeed
	AssertOk(t, err, "Failed to perform DHCP")

	// Ensure obtained IP Addr is in the allocated subnet
	AssertEquals(t, true, allocSubnet.Contains(ipClient.dhcpState.IPNet.IP), "Obtained a YIADDR is not in the expected subnet")

	// Ensure dhcp state is missing vendor attributes
	AssertEquals(t, dhcpDone.String(), ipClient.dhcpState.CurState, "DHCP State must reflect DHCP Done")

	// Ensure that there are expected Venice IPs
	veniceIPs := strings.Split(veniceIPs, ",")
	for _, v := range veniceIPs {
		AssertEquals(t, true, ipClient.dhcpState.VeniceIPs[v], "Failed to find a Venice IP. %v", v)
	}

	// Ensure the IP Assigned on the interface is indeed the YIADDR
	ipAddr, err := netlink.AddrList(ipClient.intf, netlink.FAMILY_V4)
	AssertOk(t, err, "Must be able to look up IP Address for mock interface")
	var found bool
	for _, a := range ipAddr {
		if a.IP.Equal(ipClient.dhcpState.IPNet.IP) {
			found = true
			break
		}
	}
	AssertEquals(t, true, found, "The interface IP Address should match YIADDR")
}

func TestDHCPRenewal(t *testing.T) {
	var d dhcpSrv
	err := d.setup(configureValidVendorAttrs)
	AssertOk(t, err, "Setup Failed")
	defer d.tearDown()
	AssertOk(t, err, "Failed to start DHCP Server for testing")
	mockNMD := mock.CreateMockNMD(t.Name())
	ipClient, err := NewIPClient(mockNMD, NaplesMockInterface)
	AssertOk(t, err, "IPClient creates must succeed")
	// Clear spec controllers
	mockNMD.Naples.Spec.Controllers = []string{}

	err = ipClient.DoDHCPConfig()
	// Check DHCP Config should succeed
	AssertOk(t, err, "Failed to perform DHCP")

	// Ensure obtained IP Addr is in the allocated subnet
	AssertEquals(t, true, allocSubnet.Contains(ipClient.dhcpState.IPNet.IP), "Obtained a YIADDR is not in the expected subnet")

	// Ensure dhcp state is missing vendor attributes
	AssertEquals(t, dhcpDone.String(), ipClient.dhcpState.CurState, "DHCP State must reflect DHCP Done")

	// Ensure that there are expected Venice IPs
	veniceIPs := strings.Split(veniceIPs, ",")
	for _, v := range veniceIPs {
		AssertEquals(t, true, ipClient.dhcpState.VeniceIPs[v], "Failed to find a Venice IP. %v", v)
	}
	// Ensure the IP Assigned on the interface is indeed the YIADDR
	ipAddr, err := netlink.AddrList(ipClient.intf, netlink.FAMILY_V4)
	AssertOk(t, err, "Must be able to look up IP Address for mock interface")
	var found bool
	for _, a := range ipAddr {
		if a.IP.Equal(ipClient.dhcpState.IPNet.IP) {
			found = true
			break
		}
	}
	AssertEquals(t, true, found, "The interface IP Address should match YIADDR")
	ackPktBeforeRenewal := ipClient.dhcpState.AckPacket
	time.Sleep(leaseDuration)
	ackPktAfterRenewal := ipClient.dhcpState.AckPacket
	if bytes.Equal(ackPktBeforeRenewal, ackPktAfterRenewal) {

	}
}

func TestDHCPEmptyVendorAttributes(t *testing.T) {
	var d dhcpSrv
	err := d.setup(configureEmptyVendorAtrs)
	AssertOk(t, err, "Setup Failed")
	defer d.tearDown()
	AssertOk(t, err, "Failed to start DHCP Server for testing")
	mockNMD := mock.CreateMockNMD(t.Name())
	ipClient, err := NewIPClient(mockNMD, NaplesMockInterface)
	AssertOk(t, err, "IPClient creates must succeed")

	// Clear spec controllers
	mockNMD.Naples.Spec.Controllers = []string{}
	err = ipClient.DoDHCPConfig()
	// Check DHCP Config should succeed
	AssertOk(t, err, "Failed to perform DHCP")

	// Ensure obtained IP Addr is in the allocated subnet
	AssertEquals(t, true, allocSubnet.Contains(ipClient.dhcpState.IPNet.IP), "Obtained a YIADDR is not in the expected subnet")

	// Ensure dhcp state is missing vendor attributes
	AssertEquals(t, missingVendorAttributes.String(), ipClient.dhcpState.CurState, "DHCP State must reflect Missing Vendor Attributes")

	// Ensure that there are no VeniceIPs
	AssertEquals(t, 0, len(ipClient.dhcpState.VeniceIPs), "On Missing Vendor Attributes VeniceIPs in dhcp state should be empty")

	// Ensure the IP Assigned on the interface is indeed the YIADDR
	ipAddr, err := netlink.AddrList(ipClient.intf, netlink.FAMILY_V4)
	AssertOk(t, err, "Must be able to look up IP Address for mock interface")
	var found bool
	for _, a := range ipAddr {
		if a.IP.Equal(ipClient.dhcpState.IPNet.IP) {
			found = true
			break
		}
	}
	AssertEquals(t, true, found, "The interface IP Address should match YIADDR")
}

func TestDHCPMalformedVendorAttributesOption43And60(t *testing.T) {
	var d dhcpSrv
	err := d.setup(configureMalformedVendorAttrsMismatchOption43And60)
	AssertOk(t, err, "Setup Failed")
	defer d.tearDown()
	AssertOk(t, err, "Failed to start DHCP Server for testing")
	mockNMD := mock.CreateMockNMD(t.Name())
	ipClient, err := NewIPClient(mockNMD, NaplesMockInterface)
	AssertOk(t, err, "IPClient creates must succeed")
	// Clear spec controllers
	mockNMD.Naples.Spec.Controllers = []string{}
	err = ipClient.DoDHCPConfig()
	// Check DHCP Config should succeed
	AssertOk(t, err, "Failed to perform DHCP")

	// Ensure obtained IP Addr is in the allocated subnet
	AssertEquals(t, true, allocSubnet.Contains(ipClient.dhcpState.IPNet.IP), "Obtained a YIADDR is not in the expected subnet")

	// Ensure dhcp state is missing vendor attributes
	AssertEquals(t, missingVendorAttributes.String(), ipClient.dhcpState.CurState, "DHCP State must reflect Missing Vendor Attributes")

	// Ensure that there are no VeniceIPs
	AssertEquals(t, 0, len(ipClient.dhcpState.VeniceIPs), "On Missing Vendor Attributes VeniceIPs in dhcp state should be empty")

	// Ensure the IP Assigned on the interface is indeed the YIADDR
	ipAddr, err := netlink.AddrList(ipClient.intf, netlink.FAMILY_V4)
	AssertOk(t, err, "Must be able to look up IP Address for mock interface")
	var found bool
	for _, a := range ipAddr {
		if a.IP.Equal(ipClient.dhcpState.IPNet.IP) {
			found = true
			break
		}
	}
	AssertEquals(t, true, found, "The interface IP Address should match YIADDR")
}

func TestDHCPMalformedVendorAttributes(t *testing.T) {
	var d dhcpSrv
	err := d.setup(configureMalformedVendorAttrs)
	AssertOk(t, err, "Setup Failed")
	defer d.tearDown()
	AssertOk(t, err, "Failed to start DHCP Server for testing")
	mockNMD := mock.CreateMockNMD(t.Name())
	ipClient, err := NewIPClient(mockNMD, NaplesMockInterface)
	AssertOk(t, err, "IPClient creates must succeed")
	// Clear spec controllers
	mockNMD.Naples.Spec.Controllers = []string{}
	err = ipClient.DoDHCPConfig()
	// Check DHCP Config should succeed
	AssertOk(t, err, "Failed to perform DHCP")

	// Ensure obtained IP Addr is in the allocated subnet
	AssertEquals(t, true, allocSubnet.Contains(ipClient.dhcpState.IPNet.IP), "Obtained a YIADDR is not in the expected subnet")

	// Ensure dhcp state is missing vendor attributes
	AssertEquals(t, missingVendorAttributes.String(), ipClient.dhcpState.CurState, "DHCP State must reflect Missing Vendor Attributes")

	// Ensure that there are no VeniceIPs
	AssertEquals(t, 0, len(ipClient.dhcpState.VeniceIPs), "On Missing Vendor Attributes VeniceIPs in dhcp state should be empty")

	// Ensure the IP Assigned on the interface is indeed the YIADDR
	ipAddr, err := netlink.AddrList(ipClient.intf, netlink.FAMILY_V4)
	AssertOk(t, err, "Must be able to look up IP Address for mock interface")
	var found bool
	for _, a := range ipAddr {
		if a.IP.Equal(ipClient.dhcpState.IPNet.IP) {
			found = true
			break
		}
	}
	AssertEquals(t, true, found, "The interface IP Address should match YIADDR")
}

func TestDHCPTimedout(t *testing.T) {
	var d dhcpSrv
	err := d.setup(noDHCP)
	AssertOk(t, err, "Setup Failed")
	defer d.tearDown()
	AssertOk(t, err, "Failed to start DHCP Server for testing")
	mockNMD := mock.CreateMockNMD(t.Name())
	ipClient, err := NewIPClient(mockNMD, NaplesMockInterface)
	AssertOk(t, err, "IPClient creates must succeed")
	err = ipClient.DoDHCPConfig()
	// Check DHCP Config should fail
	Assert(t, err != nil, "DHCP Config must fail")
	AssertEquals(t, ipClient.dhcpState.CurState, dhcpTimedout.String(), "DHCP should timeout when there is no dhcp server configured")
}

func TestInvalidInterface(t *testing.T) {
	mockNMD := mock.CreateMockNMD(t.Name())
	_, err := NewIPClient(mockNMD, "Some Invalid Interface")
	Assert(t, err != nil, "IPClient creates on non existent interfaces must fail")
}

func TestInvalidStaticIPAssignment(t *testing.T) {
	var d dhcpSrv
	err := d.setup(noDHCP)
	AssertOk(t, err, "Setup Failed")
	defer d.tearDown()
	mockNMD := mock.CreateMockNMD(t.Name())
	ipClient, err := NewIPClient(mockNMD, NaplesMockInterface)
	// override mock nmd with bad ip config
	badIPConfig := &cluster.IPConfig{
		IPAddress: "0.0.0.256/33",
	}
	mockNMD.SetIPConfig(badIPConfig)
	AssertOk(t, err, "IPClient creates must succeed")
	ipAddr, err := ipClient.DoStaticConfig()
	Assert(t, err != nil, "Static IP Assignment with bad config must fail")
	AssertEquals(t, "", ipAddr, "Static IP Address with bad ip config must not return a valid ip address")
}

func TestRenewalLoopPanics(t *testing.T) {
	var d dhcpSrv
	err := d.setup(configureNoVendorAtrrs)
	AssertOk(t, err, "Setup Failed")
	defer d.tearDown()
	AssertOk(t, err, "Failed to start DHCP Server for testing")
	mockNMD := mock.CreateMockNMD(t.Name())
	ipClient, err := NewIPClient(mockNMD, NaplesMockInterface)
	AssertOk(t, err, "IPClient creates must succeed")
	// Clear spec controllers
	mockNMD.Naples.Spec.Controllers = []string{}
	err = ipClient.DoDHCPConfig()
	// Check DHCP Config should succeed
	AssertOk(t, err, "Failed to perform DHCP")

	// Ensure obtained IP Addr is in the allocated subnet
	AssertEquals(t, true, allocSubnet.Contains(ipClient.dhcpState.IPNet.IP), "Obtained a YIADDR is not in the expected subnet")

	// Ensure dhcp state is missing vendor attributes
	AssertEquals(t, missingVendorAttributes.String(), ipClient.dhcpState.CurState, "DHCP State must reflect Missing Vendor Attributes")

	// Ensure that there are no VeniceIPs
	AssertEquals(t, 0, len(ipClient.dhcpState.VeniceIPs), "On Missing Vendor Attributes VeniceIPs in dhcp state should be empty")

	// Ensure the IP Assigned on the interface is indeed the YIADDR
	ipAddr, err := netlink.AddrList(ipClient.intf, netlink.FAMILY_V4)
	AssertOk(t, err, "Must be able to look up IP Address for mock interface")
	var found bool
	for _, a := range ipAddr {
		if a.IP.Equal(ipClient.dhcpState.IPNet.IP) {
			found = true
			break
		}
	}
	AssertEquals(t, true, found, "The interface IP Address should match YIADDR")
	d.conn.Close()
	time.Sleep(leaseDuration)
}

//++++++++++++++++++++++++++++ Test Utility Functions ++++++++++++++++++++++++++++++++++++++++

func (d *dhcpSrv) setup(configureVendorAttrs int) error {
	clientMAC, _ := net.ParseMAC("42:42:42:42:42:42")

	dhcpClientMock := &netlink.Veth{
		LinkAttrs: netlink.LinkAttrs{
			Name:         NaplesMockInterface,
			TxQLen:       1000,
			HardwareAddr: clientMAC,
		},
		PeerName: dhcpServerIntf,
	}

	// Create the veth pair
	if err := netlink.LinkAdd(dhcpClientMock); err != nil {
		return err
	}
	if err := netlink.LinkSetARPOn(dhcpClientMock); err != nil {
		return err
	}

	netlink.LinkSetUp(dhcpClientMock)
	// Assign IP Address statically for the server
	srvIntf, err := netlink.LinkByName(dhcpServerIntf)
	if err != nil {
		log.Errorf("Failed to find the server interface")
		return err
	}
	addr, _ := netlink.ParseAddr("172.16.10.1/28")

	if err := netlink.AddrAdd(srvIntf, addr); err != nil {
		log.Errorf("Failed to assign ip address %v to interface dhcpmock. Err: %v", addr.IP.String(), err)
		return err
	}

	if err := netlink.LinkSetUp(srvIntf); err != nil {
		log.Errorf("Failed to bring up the interface. Err: %v", err)
		return err
	}

	if configureVendorAttrs != noDHCP {
		go d.startDHCPServer(configureVendorAttrs)
	}
	return nil
}

func (d *dhcpSrv) tearDown() error {
	mockIntf, err := netlink.LinkByName(NaplesMockInterface)
	if err != nil {
		log.Errorf("TearDown Failed to look up the interfaces. Err: %v", err)
		return err
	}

	if err := netlink.LinkDel(mockIntf); err != nil {
		log.Errorf("TearDown Failed to delete the interfaces. Err: %v", err)
		return err
	}
	if d.conn != nil {
		return d.conn.Close()
	}
	return nil
}

type lease struct {
	nic    string    // Client's CHAddr
	expiry time.Time // When the lease expires
}

type DHCPHandler struct {
	ip            net.IP        // Server IP to use
	options       dhcp.Options  // Options to send to DHCP Clients
	start         net.IP        // Start of IP range to distribute
	leaseRange    int           // Number of IPs to distribute (starting from start)
	leaseDuration time.Duration // Lease period
	leases        map[int]lease // Map to keep track of leases
}

func (h *DHCPHandler) ServeDHCP(p dhcp.Packet, msgType dhcp.MessageType, options dhcp.Options) (d dhcp.Packet) {
	switch msgType {

	case dhcp.Discover:
		free, nic := -1, p.CHAddr().String()
		log.Infof("DISCOVER FOR: %v", nic)
		for i, v := range h.leases { // Find previous lease
			if v.nic == nic {
				free = i
				goto reply
			}
		}
		if free = h.freeLease(); free == -1 {
			return
		}
	reply:
		return dhcp.ReplyPacket(p, dhcp.Offer, h.ip, dhcp.IPAdd(h.start, free), h.leaseDuration,
			h.options.SelectOrderOrAll(options[dhcp.OptionParameterRequestList]))

	case dhcp.Request:
		if server, ok := options[dhcp.OptionServerIdentifier]; ok && !net.IP(server).Equal(h.ip) {
			return nil // Message not for this dhcp server
		}
		reqIP := net.IP(options[dhcp.OptionRequestedIPAddress])
		if reqIP == nil {
			reqIP = net.IP(p.CIAddr())
		}

		if len(reqIP) == 4 && !reqIP.Equal(net.IPv4zero) {
			if leaseNum := dhcp.IPRange(h.start, reqIP) - 1; leaseNum >= 0 && leaseNum < h.leaseRange {
				if l, exists := h.leases[leaseNum]; !exists || l.nic == p.CHAddr().String() {
					h.leases[leaseNum] = lease{nic: p.CHAddr().String(), expiry: time.Now().Add(h.leaseDuration)}
					return dhcp.ReplyPacket(p, dhcp.ACK, h.ip, reqIP, h.leaseDuration,
						h.options.SelectOrderOrAll(options[dhcp.OptionParameterRequestList]))
				}
			}
		}
		return dhcp.ReplyPacket(p, dhcp.NAK, h.ip, nil, 0, nil)

	case dhcp.Release, dhcp.Decline:
		nic := p.CHAddr().String()
		for i, v := range h.leases {
			if v.nic == nic {
				delete(h.leases, i)
				break
			}
		}
	}
	return nil
}

func (h *DHCPHandler) freeLease() int {
	now := time.Now()
	b := rand.Intn(h.leaseRange) // Try random first
	for _, v := range [][]int{[]int{b, h.leaseRange}, []int{0, b}} {
		for i := v[0]; i < v[1]; i++ {
			if l, ok := h.leases[i]; !ok || l.expiry.Before(now) {
				return i
			}
		}
	}
	return -1
}

func (d *dhcpSrv) startDHCPServer(configureVendorAttrs int) error {
	var opts dhcp.Options
	serverIP := net.IP{172, 16, 10, 1}

	switch configureVendorAttrs {
	case configureNoVendorAtrrs:
		opts = dhcp.Options{
			dhcp.OptionSubnetMask:       []byte{255, 255, 255, 240},
			dhcp.OptionRouter:           []byte(serverIP), // Presuming Server is also your router
			dhcp.OptionDomainNameServer: []byte(serverIP), // Presuming Server is also your DNS server
		}
	case configureEmptyVendorAtrs:
		opts = dhcp.Options{
			dhcp.OptionSubnetMask:            []byte{255, 255, 255, 240},
			dhcp.OptionRouter:                []byte(serverIP), // Presuming Server is also your router
			dhcp.OptionDomainNameServer:      []byte(serverIP), // Presuming Server is also your DNS server
			dhcp.OptionVendorClassIdentifier: []byte(PensandoIdentifier),
		}
	case configureMalformedVendorAttrsMismatchOption43And60:
		opts = dhcp.Options{
			dhcp.OptionSubnetMask:                []byte{255, 255, 255, 240},
			dhcp.OptionRouter:                    []byte(serverIP), // Presuming Server is also your router
			dhcp.OptionDomainNameServer:          []byte(serverIP), // Presuming Server is also your DNS server
			dhcp.OptionVendorSpecificInformation: []byte(PensandoIdentifier),
		}
	case configureMalformedVendorAttrs:
		opts = dhcp.Options{
			dhcp.OptionSubnetMask:                []byte{255, 255, 255, 240},
			dhcp.OptionRouter:                    []byte(serverIP), // Presuming Server is also your router
			dhcp.OptionDomainNameServer:          []byte(serverIP), // Presuming Server is also your DNS server
			dhcp.OptionVendorClassIdentifier:     []byte(PensandoIdentifier),
			dhcp.OptionVendorSpecificInformation: []byte("ř"),
		}
	case configureValidVendorAttrs:
		opts = dhcp.Options{
			dhcp.OptionSubnetMask:                []byte{255, 255, 255, 240},
			dhcp.OptionRouter:                    []byte(serverIP), // Presuming Server is also your router
			dhcp.OptionDomainNameServer:          []byte(serverIP), // Presuming Server is also your DNS server
			dhcp.OptionVendorClassIdentifier:     PensandoDHCPRequestOption.Value,
			dhcp.OptionVendorSpecificInformation: []byte(veniceIPs),
		}
	case configureValidVendorAttrs241:
		opts = dhcp.Options{
			dhcp.OptionSubnetMask:                []byte{255, 255, 255, 240},
			dhcp.OptionRouter:                    []byte(serverIP), // Presuming Server is also your router
			dhcp.OptionDomainNameServer:          []byte(serverIP), // Presuming Server is also your DNS server
			dhcp.OptionVendorClassIdentifier:     PensandoDHCPRequestOption.Value,
			dhcp.OptionVendorSpecificInformation: []byte{241, 4, 1, 1, 1, 1},
		}
	case configureValidVendorAttrs241Multiple:
		opts = dhcp.Options{
			dhcp.OptionSubnetMask:                []byte{255, 255, 255, 240},
			dhcp.OptionRouter:                    []byte(serverIP), // Presuming Server is also your router
			dhcp.OptionDomainNameServer:          []byte(serverIP), // Presuming Server is also your DNS server
			dhcp.OptionVendorClassIdentifier:     PensandoDHCPRequestOption.Value,
			dhcp.OptionVendorSpecificInformation: []byte{241, 8, 2, 2, 2, 2, 3, 3, 3, 3},
		}
	}

	handler := &DHCPHandler{
		ip:            serverIP,
		leaseDuration: leaseDuration,
		start:         net.ParseIP(ipAddrStart),
		leaseRange:    10,
		leases:        make(map[int]lease, 10),
		options:       opts,
	}
	c, err := conn.NewUDP4BoundListener(dhcpServerIntf, ":67")
	if err != nil {
		log.Errorf("Failed to start DHCP Server. Err: %v", err)
		return err
	}
	d.conn = c
	go dhcp.Serve(d.conn, handler)
	return nil
}
