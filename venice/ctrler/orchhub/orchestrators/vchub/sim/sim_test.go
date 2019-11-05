package sim

import (
	"context"
	"strings"
	"testing"
	"time"

	"github.com/vmware/govmomi"
	"github.com/vmware/govmomi/property"
	"github.com/vmware/govmomi/view"
	"github.com/vmware/govmomi/vim25/mo"
	"github.com/vmware/govmomi/vim25/types"

	. "github.com/pensando/sw/venice/utils/testutils"
)

type watchObj struct {
	Name    string
	Changes []types.PropertyChange
}

func TestList(t *testing.T) {
	vcID := "user:pass@127.0.0.1:8990"
	s, err := NewVcSim(Config{Addr: vcID})
	AssertOk(t, err, "Failed to create vcsim")
	defer s.Destroy()

	dc, err := s.AddDC("dc1")
	AssertOk(t, err, "failed to create DC")
	host, err := dc.AddHost("host1")
	AssertOk(t, err, "failed to create host1")
	err = host.AddNic("testNIC", "aaaa:bbbb:cccc")
	AssertOk(t, err, "failed to add nic")
	_, err = dc.AddVM("testVM", "host1")
	AssertOk(t, err, "failed to create vm")

	ctx := context.Background()

	u := s.GetURL()

	c, err := govmomi.NewClient(ctx, u, true)
	if err != nil {
		t.Fatalf("err not nil %v", err)
	}

	// Check DCs
	var dcs []mo.Datacenter
	getKind(t, c, "Datacenter", []string{"name"}, &dcs)
	AssertEquals(t, 1, len(dcs), "Recieved incorrect amount of dcs")
	AssertEquals(t, "dc1", dcs[0].Name, "DC had incorret name")

	// Check Hosts
	var hss []mo.HostSystem
	getKind(t, c, "HostSystem", []string{"name", "config"}, &hss)
	AssertEquals(t, 1, len(hss), "Recieved incorrect amount of dcs")
	AssertEquals(t, "host1", hss[0].Name, "host had incorrect name")
	hasNic := false
	for _, pnic := range hss[0].Config.Network.Pnic {
		if strings.Contains(pnic.Key, "testNIC") {
			hasNic = true
		}
	}
	Assert(t, hasNic == true, "Failed to find testNIC")

	// Remove nic
	host.RemoveNic("testNIC")
	var hss2 []mo.HostSystem
	getKind(t, c, "HostSystem", []string{"name", "config"}, &hss2)
	AssertEquals(t, 1, len(hss), "Recieved incorrect amount of dcs")
	AssertEquals(t, "host1", hss[0].Name, "host had incorrect name")
	hasNic = false
	for _, pnic := range hss2[0].Config.Network.Pnic {
		if strings.Contains(pnic.Key, "testNIC") {
			hasNic = true
		}
	}
	Assert(t, hasNic == false, "testNIC was not deleted")

	// Check VMs
	var vms []mo.VirtualMachine
	getKind(t, c, "VirtualMachine", []string{"name"}, &vms)
	AssertEquals(t, 1, len(vms), "Recieved incorrect amount of dcs")
	AssertEquals(t, "testVM", vms[0].Name, "VM had incorrect name")
}

func getKind(t *testing.T, client *govmomi.Client, kind string, props []string, dst interface{}) {
	ctx := context.Background()
	viewMgr := view.NewManager(client.Client)
	v, err := viewMgr.CreateContainerView(ctx, client.Client.ServiceContent.RootFolder, []string{kind}, true)
	if err != nil {
		t.Fatalf("err not nil")
	}

	defer v.Destroy(ctx)

	err = v.Retrieve(ctx, []string{kind}, props, dst)
	if err != nil {
		t.Fatalf("err not nil %v", err)
	}
}

func TestWatch(t *testing.T) {
	t.Skip("Events for objects created after the watch has started are not being received. Still debugging this issue.")

	vcID := "user:pass@127.0.0.1:8990"
	s, err := NewVcSim(Config{Addr: vcID})
	AssertOk(t, err, "Failed to create vcsim")
	defer s.Destroy()

	ctx1 := context.Background()
	cancelCtx, cancelWatch := context.WithCancel(ctx1)

	u := s.GetURL()
	c, err := govmomi.NewClient(cancelCtx, u, true)
	if err != nil {
		t.Fatalf("err not nil %v", err)
	}
	client := c.Client
	viewMgr := view.NewManager(client)

	v, err := viewMgr.CreateContainerView(cancelCtx, c.Client.ServiceContent.RootFolder, []string{"Datacenter"}, true)
	if err != nil {
		t.Fatalf("err not nil")
	}

	defer v.Destroy(cancelCtx)

	hostProps := []string{"name"}
	hostRef := types.ManagedObjectReference{Type: "Datacenter"}
	filter := new(property.WaitFilter)
	filter = filter.Add(v.Reference(), hostRef.Type, hostProps)

	resCh := make(chan watchObj)

	updFunc := func(updates []types.ObjectUpdate) bool {
		t.Logf("call")
		for _, update := range updates {
			resCh <- watchObj{
				Name:    update.Obj.Value,
				Changes: update.ChangeSet,
			}
		}
		// Must return false, returning true will cause waitForUpdates to exit.
		return false
	}

	go func() {
		err = property.WaitForUpdates(cancelCtx, property.DefaultCollector(client), filter, updFunc)
	}()

	_, err = s.AddDC("test-dc-1")
	AssertOk(t, err, "failed to create DC")

	items := []watchObj{}
	for len(items) != 1 {
		select {
		case obj := <-resCh:
			items = append(items, obj)
		case <-time.After(5 * time.Second):
			cancelWatch()
			t.Fatalf("Failed to receive all messages. Only received %d items. %v", len(items), items)
		}
	}
	cancelWatch()

}
