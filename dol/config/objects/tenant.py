# /usr/bin/python3
import pdb

import infra.common.defs        as defs
import infra.common.objects     as objects
import infra.config.base        as base

import config.resmgr            as resmgr
import config.objects.segment   as segment
import config.objects.lif       as lif
import config.objects.tunnel    as tunnel

from config.store               import Store
from infra.common.logging       import cfglogger

import config.hal.defs          as haldefs
import config.hal.api           as halapi

class TenantObject(base.ConfigObjectBase):
    def __init__(self):
        super().__init__()
        self.Clone(Store.templates.Get('TENANT'))
        return
        
    def Init(self, spec, lifns = None):
        self.id = resmgr.TenIdAllocator.get()
        gid = "Ten%04d" % self.id
        self.GID(gid)

        self.spec = spec
        self.type = spec.type.upper()
        self.security_profile = spec.security_profile.Get(Store)
        if self.IsInfra():
            self.subnet             = resmgr.TepIpSubnetAllocator.get()
            self.ip_addr_allocator  = resmgr.CreateIpAddrAllocator(self.subnet.get())
            self.local_tep          = self.ip_addr_allocator.get()
        self.Show()
        
        # Process LIFs
        self.lifns = lifns  
        self.obj_helper_lif = lif.LifObjectHelper()
        self.__create_lifs()
        # Process Segments 
        self.obj_helper_segment = segment.SegmentObjectHelper()
        self.__create_segments()

        # Process Tunnels
        if self.IsInfra():
            self.obj_helper_tunnel = tunnel.TunnelObjectHelper()
            self.__create_tunnels()
        return

    def Show(self):
        cfglogger.info("Tenant  : %s" % self.GID())
        cfglogger.info("- Type      : %s" % self.type)
        if self.IsInfra():
            cfglogger.info("- LocalTep  : %s" % self.local_tep.get())
        return

    def IsInfra(self):
        return self.type == 'INFRA'

    def __create_segments(self):
        for entry in self.spec.segments:
            spec = entry.spec.Get(Store)
            self.obj_helper_segment.Generate(self, spec, entry.count)
        self.obj_helper_segment.AddToStore()
        return

    def __create_lifs(self):
        self.spec.lif.spec = self.spec.lif.spec.Get(Store)
        self.obj_helper_lif.Generate(self, self.spec.lif.spec,
                                     self.spec.lif.count, self.lifns)
        self.obj_helper_lif.Configure()
        return

    def __create_tunnels(self):
        for entry in self.spec.tunnels:
            spec = entry.spec.Get(Store)
            self.obj_helper_tunnel.Generate(self, spec, self.GetEps())
        self.obj_helper_tunnel.AddToStore()
        return

    def AllocLif(self):
        return self.obj_helper_lif.Alloc()

    def ConfigureSegments(self):
        return self.obj_helper_segment.Configure()

    def ConfigureTunnels(self):
        if self.IsInfra():
            self.obj_helper_tunnel.Configure()
        return

    def GetEps(self):
        return self.obj_helper_segment.GetEps()

    def PrepareHALRequestSpec(self, reqspec):
        reqspec.meta.tenant_id          = self.id
        reqspec.key_or_handle.tenant_id = self.id
        reqspec.security_profile_handle = self.security_profile.hal_handle
        return

    def ProcessHALResponse(self, req_spec, resp_spec):
        cfglogger.info("  - Tenant %s = %s" %\
                       (self.GID(), \
                        haldefs.common.ApiStatus.Name(resp_spec.api_status)))
        return

    def IsFilterMatch(self, spec):
        return super().IsFilterMatch(spec.filters)

# Helper Class to Generate/Configure/Manage Tenant Objects.
class TenantObjectHelper:
    def __init__(self):
        self.tens = []
        return

    def Configure(self):
        cfglogger.info("Configuring %d Tenants." % len(self.tens)) 
        halapi.ConfigureTenants(self.tens)
        for ten in self.tens:
            ten.ConfigureSegments()
            ten.ConfigureTunnels()
        return
        
    def Generate(self, topospec):
        for entry in topospec.tenants:
            spec = entry.spec.Get(Store)
            cfglogger.info("Creating %d Tenants" % entry.count)
            for c in range(entry.count):
                ten = TenantObject()
                ten.Init(spec, entry.lifns)
                self.tens.append(ten)
        Store.objects.SetAll(self.tens)
        return

    def main(self, topospec):
        self.Generate(topospec)
        self.Configure()
        return

TenantHelper = TenantObjectHelper()
