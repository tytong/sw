#! /usr/bin/python3
import grpc
import copy

from iota.harness.infra.utils.logger import Logger as Logger

import iota.protos.pygen.types_pb2 as types_pb2
import iota.protos.pygen.cfg_svc_pb2 as cfg_svc
import iota.protos.pygen.topo_svc_pb2 as topo_svc

import iota.harness.infra.store as store
import iota.harness.infra.types as types
import iota.harness.infra.utils.utils as utils
import iota.harness.infra.utils.parser as parser

from iota.harness.infra.glopts import GlobalOptions

gl_iota_svc_channel = None
gl_topo_svc_stub = None
gl_cfg_svc_stub = None
gl_workloads = {}

def Init():
    server = 'localhost:' + str(GlobalOptions.svcport)
    Logger.info("Creating GRPC Channel to IOTA Service %s" % server)
    gl_iota_svc_channel = grpc.insecure_channel(server)
    Logger.debug("Waiting for IOTA Service to be UP")
    grpc.channel_ready_future(gl_iota_svc_channel).result()
    Logger.info("Connected to IOTA Service")

    global gl_topo_svc_stub
    gl_topo_svc_stub = topo_svc.TopologyApiStub(gl_iota_svc_channel)

    global gl_cfg_svc_stub
    gl_cfg_svc_stub = cfg_svc.ConfigMgmtApiStub(gl_iota_svc_channel)
    return

def __rpc(req, rpcfn):
    utils.LogMessageContents("Request", req, Logger.debug)
    resp = rpcfn(req)
    if resp.api_response.api_status != types_pb2.API_STATUS_OK:
        Logger.error("Error: ",
                     types_pb2.APIResponseType.Name(resp.api_response.api_status),
                     resp.api_response.error_msg)
        return None
    utils.LogMessageContents("Response", resp, Logger.debug)
    return resp

def CleanupTestbed(req):
    global gl_topo_svc_stub
    Logger.debug("Cleaning up Testbed:")
    return __rpc(req, gl_topo_svc_stub.CleanUpTestBed)

def InitTestbed(req):
    global gl_topo_svc_stub
    Logger.debug("Initializing Testbed:")
    return __rpc(req, gl_topo_svc_stub.InitTestBed)

def AddNodes(req):
    global gl_topo_svc_stub
    Logger.debug("Add Nodes:")
    return __rpc(req, gl_topo_svc_stub.AddNodes)

def AddWorkloads(req):
    global gl_workloads
    for wl in req.workloads:
        wl_copy = copy.deepcopy(wl)
        gl_workloads[wl.workload_name] = wl_copy

    global gl_topo_svc_stub
    Logger.debug("Add Workloads:")
    return __rpc(req, gl_topo_svc_stub.AddWorkloads)

def DeleteWorkloads(req):
    global gl_workloads
    for wl in req.workloads:
        del gl_workloads[wl.workload_name]

    global gl_topo_svc_stub
    Logger.debug("Delete Workloads:")
    return __rpc(req, gl_topo_svc_stub.DeleteWorkloads)

def GetWorkloads():
    global gl_workloads
    return gl_workloads.values()

def Trigger(req):
    global gl_topo_svc_stub
    Logger.debug("Trigger Message:")
    return __rpc(req, gl_topo_svc_stub.Trigger)

def PushConfig(req):
    global gl_cfg_svc_stub
    Logger.debug("Push Config:")
    return __rpc(req, gl_cfg_svc_stub.PushConfig)

def QueryConfig(req):
    global gl_cfg_svc_stub
    Logger.debug("Query Config:")
    return __rpc(req, gl_cfg_svc_stub.QueryConfig)

def InitCfgService(req):
    global gl_cfg_svc_stub
    Logger.debug("Init Config Service:")
    return __rpc(req, gl_cfg_svc_stub.InitCfgService)

def GenerateConfigs(req):
    global gl_cfg_svc_stub
    Logger.debug("Generate Configs:")
    return __rpc(req, gl_cfg_svc_stub.GenerateConfigs)

def MakeCluster(req):
    global gl_cfg_svc_stub
    Logger.debug("Make Cluster:")
    return __rpc(req, gl_cfg_svc_stub.MakeCluster)

def GetVeniceMgmtIpAddresses():
    return store.GetTestbed().GetCurrentTestsuite().GetTopology().GetVeniceMgmtIpAddresses()

def GetNaplesMgmtIpAddresses():
    return store.GetTestbed().GetCurrentTestsuite().GetTopology().GetNaplesMgmtIpAddresses()

def GetNaplesNodeUuidMap():
    return store.GetTestbed().GetCurrentTestsuite().GetTopology().GetNaplesUuidMap()

def GetDataVlans():
    return store.GetTestbed().GetDataVlans()

def GetVeniceHostnames():
    return store.GetTestbed().GetCurrentTestsuite().GetTopology().GetVeniceHostnames()

def GetNaplesHostnames():
    return store.GetTestbed().GetCurrentTestsuite().GetTopology().GetNaplesHostnames()

def PrintCommandResults(cmd):
    Logger.header('COMMAND')
    Logger.info("%s" % cmd.command)
    Logger.header('STDOUT')
    for line in cmd.stdout.split('\n'):
        Logger.info(line)
    Logger.header('STDERR')
    for line in cmd.stderr.split('\n'):
        Logger.info(line)
