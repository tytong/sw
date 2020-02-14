#! /usr/bin/python3
import pdb

from infra.common.logging import logger

from apollo.config.store import client as EzAccessStoreClient

from apollo.config.resmgr import client as ResmgrClient
from apollo.config.resmgr import Resmgr

import apollo.config.agent.api as api
import apollo.config.objects.base as base
import apollo.config.utils as utils
import apollo.config.topo as topo
import ipaddress

import service_pb2 as service_pb2
import types_pb2 as types_pb2

class LocalMappingObject(base.ConfigObjectBase):
    def __init__(self, node, parent, spec, ipversion, count):
        super().__init__(api.ObjectTypes.LMAPPING, node)
        parent.AddChild(self)
        if (EzAccessStoreClient[node].IsDeviceLearningEnabled()) or \
                (EzAccessStoreClient[node].IsDeviceOverlayRoutingEnabled()):
            self.SetOrigin(topo.OriginTypes.DISCOVERED)

        self.__is_public = getattr(spec, 'public', False)
        ################# PUBLIC ATTRIBUTES OF LOCAL MAPPING OBJECT ###########
        if (hasattr(spec, 'id')):
            self.MappingId = spec.id
        else:
            self.MappingId = next(ResmgrClient[node].LocalMappingIdAllocator)
        self.GID('LocalMapping%d'%self.MappingId)
        self.UUID = utils.PdsUuid(self.MappingId, self.ObjType)
        self.VNIC = parent
        self.PublicIPAddr = None
        self.SourceGuard = parent.SourceGuard
        self.HasDefaultRoute = False
        if ipversion == utils.IP_VERSION_6:
            self.AddrFamily = 'IPV6'
            self.IPAddr = parent.SUBNET.AllocIPv6Address();
            if self.__is_public:
                self.PublicIPAddr = next(ResmgrClient[node].PublicIpv6AddressAllocator)
            if parent.SUBNET.V6RouteTable:
                self.HasDefaultRoute = parent.SUBNET.V6RouteTable.HasDefaultRoute
            self.SvcIPAddr, self.SvcPort = EzAccessStoreClient[node].GetSvcMapping(utils.IP_VERSION_6)
        else:
            self.AddrFamily = 'IPV4'
            if getattr(spec, 'lipaddr', None) != None:
                logger.info("LocalMapping Object assigned IP address:%s" % spec.lipaddr)
                self.IPAddr = ipaddress.IPv4Address(spec.lipaddr)
            else:
                self.IPAddr = parent.SUBNET.AllocIPv4Address()
                logger.info("LocalMapping Object generated IP address:%s" %(str(self.IPAddr)))
            if self.__is_public:
                self.PublicIPAddr = next(ResmgrClient[node].PublicIpAddressAllocator)
            if parent.SUBNET.V4RouteTable:
                self.HasDefaultRoute = parent.SUBNET.V4RouteTable.HasDefaultRoute
            self.SvcIPAddr, self.SvcPort = EzAccessStoreClient[node].GetSvcMapping(utils.IP_VERSION_4)
        self.Label = 'NETWORKING'
        self.FlType = "MAPPING"
        self.IP = str(self.IPAddr) # for testspec
        # Provider IP can be v4 or v6
        self.ProviderIPAddr, self.TunFamily = EzAccessStoreClient[node].GetProviderIPAddr(count)
        self.ProviderIP = str(self.ProviderIPAddr) # for testspec
        if self.PublicIPAddr is not None:
            self.PublicIP = str(self.PublicIPAddr) # for testspec
        self.SvcIP = str(self.SvcIPAddr) # for testspec
        # We will differentiate the traffic by port (vnet vs internet) and
        # different rules will be applied
        self.AppPort = ResmgrClient[node].TransportSrcPort
        self.LBPort = ResmgrClient[node].TransportSrcLBPort
        self.UnderlayVPCId = EzAccessStoreClient[node].GetUnderlayVPCId()

        ################# PRIVATE ATTRIBUTES OF MAPPING OBJECT #####################
        self.DeriveOperInfo()
        self.Show()
        return

    def __repr__(self):
        return "LocalMappingID:%d|Vnic: %s |Subnet: %s |VPC: %s |Origin:%s" %\
               (self.MappingId, self.VNIC.UUID, self.VNIC.SUBNET.UUID, self.VNIC.SUBNET.VPC.UUID, self.Origin)

    def Show(self):
        logger.info("LocalMapping Object:", self)
        logger.info("- %s" % repr(self))
        logger.info("- IPAddr:%s|PublicIP:%s|PIP:%s|VIP:%s" \
            %(str(self.IPAddr), str(self.PublicIPAddr), str(self.ProviderIPAddr), str(self.SvcIPAddr)))
        return

    def IsFilterMatch(self, selectors):
        return super().IsFilterMatch(selectors.flow.filters)

    def PopulateKey(self, grpcmsg):
        if grpcmsg.__class__.__name__ == 'MappingDeleteRequest' or grpcmsg.__class__.__name__ == 'MappingGetRequest':
            key = grpcmsg.Id.add()
            key.IPKey.VPCId = self.VNIC.SUBNET.VPC.GetKey()
            utils.GetRpcIPAddr(self.IPAddr, key.IPKey.IPAddr)
        else:
            grpcmsg.Id.append(self.GetKey())
        return

    def PopulateSpec(self, grpcmsg):
        spec = grpcmsg.Request.add()
        spec.Id = self.GetKey()
        spec.IPKey.VPCId = self.VNIC.SUBNET.VPC.GetKey()
        utils.GetRpcIPAddr(self.IPAddr, spec.IPKey.IPAddr)
        spec.SubnetId = self.VNIC.SUBNET.GetKey()
        spec.VnicId = self.VNIC.GetKey()
        spec.MACAddr = self.VNIC.MACAddr.getnum()
        utils.GetRpcEncap(self.Node, self.VNIC.MplsSlot, self.VNIC.Vnid, spec.Encap)
        spec.PublicIP.Af = types_pb2.IP_AF_NONE
        if self.PublicIPAddr is not None:
            utils.GetRpcIPAddr(self.PublicIPAddr, spec.PublicIP)
        if utils.IsPipelineArtemis():
            utils.GetRpcIPAddr(self.ProviderIPAddr, spec.ProviderIp)
        return

    def ValidateSpec(self, spec):
        if spec.IPKey.VPCId != self.VNIC.SUBNET.VPC.GetKey():
            return False
        if not utils.ValidateRpcIPAddr(self.IPAddr, spec.IPKey.IPAddr):
            return False
        return True

    def GetGrpcSvcMappingCreateMessage(self, cookie):
        grpcmsg = service_pb2.SvcMappingRequest()
        grpcmsg.BatchCtxt.BatchCookie = cookie
        spec = grpcmsg.Request.add()
        spec.Key.VPCId = self.VNIC.SUBNET.VPC.GetKey()
        utils.GetRpcIPAddr(self.IPAddr, spec.Key.BackendIP)
        spec.Key.BackendPort = self.LBPort
        utils.GetRpcIPAddr(self.SvcIPAddr, spec.IPAddr)
        spec.SvcPort = self.SvcPort
        utils.GetRpcIPAddr(self.ProviderIPAddr, spec.ProviderIP)
        return grpcmsg

    def GetGrpcSvcMappingReadMessage(self):
        grpcmsg = service_pb2.SvcMappingRequest()
        key = grpcmsg.Id.add()
        key.VPCId = self.VNIC.SUBNET.VPC.GetKey()
        utils.GetRpcIPAddr(self.IPAddr, key.BackendIP)
        key.Port = self.LBPort
        return grpcmsg


class LocalMappingObjectClient(base.ConfigClientBase):
    def __init__(self):
        super().__init__(api.ObjectTypes.LMAPPING, Resmgr.MAX_LMAPPING)
        return

    def PdsctlRead(self, node):
        # pdsctl show not supported for local mapping
        return True

    def GenerateObjects(self, node, parent, vnic_spec_obj):
        isV4Stack = utils.IsV4Stack(parent.SUBNET.VPC.Stack)
        isV6Stack = utils.IsV6Stack(parent.SUBNET.VPC.Stack)
        c = 0
        v6c = 0
        v4c = 0
        if hasattr(vnic_spec_obj, 'lmap'):
            lmap_spec = vnic_spec_obj.lmap[0]
            lmap_count = lmap_spec.count
        else:
            lmap_spec = vnic_spec_obj
            lmap_count = vnic_spec_obj.ipcount
        while c < lmap_count:
            if isV6Stack:
                obj = LocalMappingObject(node, parent, lmap_spec, utils.IP_VERSION_6, v6c)
                self.Objs[node].update({obj.MappingId: obj})
                c = c + 1
                v6c = v6c + 1
            if c < lmap_count and isV4Stack:
                obj = LocalMappingObject(node, parent, lmap_spec, utils.IP_VERSION_4, v4c)
                self.Objs[node].update({obj.MappingId: obj})
                c = c + 1
                v4c = v4c + 1
        return

    def CreateObjects(self, node):
        # TODO: Add check to see if 32 IPs per vnic
        super().CreateObjects(node)

        if utils.IsServiceMappingSupported():
            cookie = utils.GetBatchCookie(node)
            msgs = list(map(lambda x: x.GetGrpcSvcMappingCreateMessage(cookie), self.Objects(node)))
            api.client[node].Create(api.ObjectTypes.SVCMAPPING, msgs)
        return True

    def ReadObjects(self, node):
        # read all not supported for local mapping - so do one by one
        cfgObjects = self.Objects(node)
        logger.info(f"Reading {len(cfgObjects)} {self.ObjType.name} Objects in {node}")
        result = list(map(lambda x: x.Read(), cfgObjects))
        if not all(result):
            logger.info(f"Reading {len(cfgObjects)} {self.ObjType.name} Objects FAILED in {node}")
            return False

        if utils.IsServiceMappingSupported():
            logger.info(f"Creating {len(self.Objects(node))} SVC {self.ObjType.name} Objects in {node}")
            msgs = list(map(lambda x: x.GetGrpcSvcMappingReadMessage(), self.Objects(node)))
            api.client[node].Get(api.ObjectTypes.SVCMAPPING, msgs)
        return True

    def GetVnicAddresses(self, vnic):
        ip_addresses = []
        for mapping in self.Objects(vnic.Node):
            if hasattr(mapping, "VNIC") and mapping.VNIC.GID() == vnic.GID():
                if mapping.AddrFamily == 'IPV6':
                    ip_addresses.append(str(mapping.IPAddr) + "/" + str(mapping.VNIC.SUBNET.IPPrefix[0].prefixlen))
                else:
                    ip_addresses.append(str(mapping.IPAddr) + "/" + str(mapping.VNIC.SUBNET.IPPrefix[1].prefixlen))

        return ip_addresses

client = LocalMappingObjectClient()

def GetMatchingObjects(selectors, node):
    objs = []
    for obj in client.Objects(node):
        if obj.IsFilterMatch(selectors):
            objs.append(obj)
    return objs
