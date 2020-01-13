#! /usr/bin/python3
from infra.common.objects   import ObjectDatabase as ObjectDatabase
from infra.common.logging   import logger as logger
from infra.config.store     import ConfigStore as ConfigStore

class ApuluConfigStore:
    def __init__(self):
        self.objects    = ConfigStore.objects
        self.templates  = ConfigStore.templates
        self.specs      = ConfigStore.specs

        # Custom Database for easy access.
        self.trunks = ObjectDatabase()
        self.tunnels = ObjectDatabase()
        self.nexthops = ObjectDatabase()
        self.nexthopgroups = ObjectDatabase()
        self.device = None
        self.underlay_vpc = None
        # Batch client
        self.batchClient = None
        self.hostport = None
        self.switchport = None
        return

    def reset(self):
        self.__init__()
        
    def SetBatchClient(self, obj):
        self.batchClient = obj

    def GetBatchClient(self):
        return self.batchClient

    def SetTunnels(self, objs):
        self.tunnels.db.clear()
        return self.tunnels.SetAll(objs)

    def SetNexthops(self, objs):
        self.nexthops.db.clear()
        return self.nexthops.SetAll(objs)

    def SetNexthopgroups(self, objs):
        self.nexthopgroups.db.clear()
        return self.nexthopgroups.SetAll(objs)

    def SetDevice(self,obj):
        self.device = obj

    def GetDevice(self):
        return self.device

    def SetHostPort(self, port):
        self.hostport = port

    def GetHostPort(self):
        return self.hostport

    def SetSwitchPort(self, port):
        self.switchport = port

    def GetSwitchPort(self):
        return self.switchport

    def SetUnderlayVPC(self, obj):
        self.underlay_vpc = obj

    def GetUnderlayVPCId(self):
        if self.underlay_vpc:
            return self.underlay_vpc.VPCId
        else:
            return -1

    def GetProviderIPAddr(self, count):
        if self.underlay_vpc:
            return self.underlay_vpc.GetProviderIPAddr(count)
        else:
            return None,-1

    def GetSvcMapping(self, ipversion):
        if self.underlay_vpc:
            return self.underlay_vpc.GetSvcMapping(ipversion)
        else:
            return None,-1

    def IsBitwMode(self):
        return self.device.IsBitwMode()

    def IsHostMode(self):
        return self.device.IsHostMode()

    def IsDeviceEncapTypeMPLS(self):
        return self.device.IsEncapTypeMPLS()

    def IsDeviceEncapTypeVXLAN(self):
        return self.device.IsEncapTypeVXLAN()

    def GetDeviceEncapType(self):
        return self.device.EncapType

    def GetWorkloadTunnels(self):
        tunnels = []
        for tun in self.tunnels.GetAllInList():
            if tun.IsWorkload(): tunnels.append(tun)
        return tunnels

    def GetIgwNonNatTunnels(self):
        tunnels = []
        for tun in self.tunnels.GetAllInList():
            if tun.IsIgw() and tun.IsNat() is False:
                 tunnels.append(tun)
        return tunnels

    def GetIgwNatTunnels(self):
        tunnels = []
        for tun in self.tunnels.GetAllInList():
            if tun.IsIgw() and tun.IsNat(): tunnels.append(tun)
        return tunnels

    def GetSvcTunnels(self, remote=False):
        tunnels = []
        for tun in self.tunnels.GetAllInList():
            if tun.IsSvc() and tun.Remote is remote:
                tunnels.append(tun)
        return tunnels

    def GetUnderlayTunnels(self, ecmp=False):
        tunnels = []
        for tun in self.tunnels.GetAllInList():
            if ecmp is False:
                if tun.IsUnderlay(): tunnels.append(tun)
            elif ecmp is True:
                if tun.IsUnderlayEcmp(): tunnels.append(tun)
        return tunnels

    def GetOverlayTunnels(self):
        tunnels = []
        for tun in self.tunnels.GetAllInList():
            if tun.IsOverlay(): tunnels.append(tun)
        return tunnels

    def GetUnderlayNexthops(self, ecmp=False):
        nhops = []
        for nh in self.nexthops.GetAllInList():
            if ecmp is False:
                if nh.IsUnderlay(): nhops.append(nh)
            if ecmp is True:
                if nh.IsUnderlayEcmp(): nhops.append(nh)
        return nhops

    def GetUnderlayNhGroups(self):
        nhgs = []
        for nhg in self.nexthopgroups.GetAllInList():
            if nhg.IsUnderlay():
                nhgs.append(nhg)
        return nhgs

    def GetOverlayNexthops(self, ecmp=False):
        nhops = []
        for nh in self.nexthops.GetAllInList():
            if nh.IsOverlay(): nhops.append(nh)
        return nhops

    def GetDualEcmpNexthops(self):
        nhops = []
        for nh in self.nexthops.GetAllInList():
            if nh.IsOverlay() and nh.DualEcmp: nhops.append(nh)
        return nhops

    def GetOverlayNhGroups(self):
        nhgs = []
        for nhg in self.nexthopgroups.GetAllInList():
            if nhg.IsOverlay(): nhgs.append(nhg)
        return nhgs

    def GetDualEcmpNhGroups(self):
        nhgs = []
        for nhg in self.nexthopgroups.GetAllInList():
            if nhg.IsOverlay() and nhg.DualEcmp: nhgs.append(nhg)
        return nhgs

    def GetTrunkingUplinks(self):
        return self.trunks.GetAllInList()

    def SetTrunkingUplinks(self, objs):
        self.trunks.db.clear()
        return self.trunks.SetAll(objs)

Store = ApuluConfigStore()