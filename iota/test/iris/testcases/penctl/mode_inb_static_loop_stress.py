#! /usr/bin/python3
import time
import iota.harness.api as api
import iota.protos.pygen.topo_svc_pb2 as topo_svc_pb2
import iota.test.iris.testcases.penctl.penctldefs as penctldefs
import iota.test.iris.testcases.penctl.common as common

def Setup(tc):
    tc.Nodes = api.GetNaplesHostnames()
    return api.types.status.SUCCESS

def Trigger(tc):
    if len(tc.Nodes) > 0:
        n = tc.Nodes[0]

        for i in range(0, 6):
            ret = common.SetNaplesModeInband_Static(n, "2.2.2.1", "2.2.2.2/24")
            time.sleep(10)
            mode = common.PenctlGetTransitionPhaseStatus(n)
            if mode != "VENICE_UNREACHABLE":
                return api.types.status.FAILURE

    return api.types.status.SUCCESS

def Verify(tc):
    return api.types.status.SUCCESS

def Teardown(tc):
    return api.types.status.SUCCESS
