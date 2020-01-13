#! /usr/bin/python3
import pdb
import enum

from infra.common.logging import logger

from iota.test.apulu.config.store import Store

import iota.test.apulu.config.resmgr as resmgr
import iota.test.apulu.config.agent.api as api
import iota.test.apulu.config.utils as utils
import iota.test.apulu.config.objects.base as base

class UplinkPorts(enum.IntEnum):
    # In DOL, it starts with 1
    UplinkPort0 = 1
    UplinkPort1 = 2

class PortObject(base.ConfigObjectBase):
    def __init__(self, port, mode, state='UP'):
        ################# PUBLIC ATTRIBUTES OF PORT OBJECT #####################
        self.PortId = next(resmgr.PortIdAllocator)
        self.GID("Port ID:%s"%self.PortId)
        self.Port = port
        self.Mode = mode
        self.AdminState = state
        ################# PRIVATE ATTRIBUTES OF PORT OBJECT #####################
        self.Show()
        return

    def __repr__(self):
        return "PortId:%d|Port:%d|AdminState:%s|Mode:%s" % \
                (self.PortId, self.Port, self.AdminState, self.Mode)

    def Show(self):
        logger.info("PortObject:")
        logger.info("- %s" % repr(self))
        return

    def IsHostPort(self):
        return self.Mode == utils.PortTypes.HOST

    def IsSwitchPort(self):
        return self.Mode == utils.PortTypes.SWITCH

class PortObjectClient:
    def __init__(self):
        self.__objs = dict()
        return

    def Objects(self):
        return self.__objs.values()

    def GenerateObjects(self, node, parent, topospec):
        def __get_port_mode(port, mode='auto'):
            if mode == 'switch':
                return utils.PortTypes.SWITCH
            elif mode == 'host':
                return utils.PortTypes.HOST
            if Store.IsHostMode():
                return utils.PortTypes.SWITCH
            elif Store.IsBitwMode():
                if port == UplinkPorts.UplinkPort0:
                    return utils.PortTypes.HOST
                elif port == UplinkPorts.UplinkPort1:
                    return utils.PortTypes.SWITCH
            return utils.PortTypes.NONE

        portlist = getattr(topospec, 'uplink', None)
        if portlist is None:
            return
        for spec in portlist:
            entryspec = spec.entry
            port = getattr(entryspec, 'port')
            mode = __get_port_mode(port, getattr(entryspec, 'mode', 'auto'))
            obj = PortObject(port, mode)
            self.__objs.update({obj.PortId: obj})
            if obj.IsHostPort():
                Store.SetHostPort(obj.Port)
            elif obj.IsSwitchPort():
                Store.SetSwitchPort(obj.Port)
        return

client = PortObjectClient()
def GetMatchingObjects(selectors):
    return client.Objects()
