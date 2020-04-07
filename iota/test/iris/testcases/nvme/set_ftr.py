#! /usr/bin/python3
import iota.harness.api as api

def Setup(tc):
    tc.nodes = api.GetNaplesHostnames()

    return api.types.status.SUCCESS

def Trigger(tc):
    req = api.Trigger_CreateExecuteCommandsRequest(serial = True)

    api.Logger.info("Trying to set number of queues supported by controller0 at host {0}".format(tc.nodes[1]))
    api.Trigger_AddHostCommand(req, tc.nodes[1], "nvme set-feature /dev/{} -f 7 -v 0x16 | cut -d ':' -f 3".format(tc.iterators.ctrl))

    tc.resp = api.Trigger(req)

    return api.types.status.SUCCESS

def Verify(tc):
    if tc.resp is None:
        return api.types.status.FAILURE

    result = api.types.status.SUCCESS

    api.Logger.info("nvme_get_ftr results for the following nodes: {0}".format(tc.nodes))

    for cmd in tc.resp.commands:
        api.PrintCommandResults(cmd)
        if cmd.exit_code != 0 and  api.Trigger_IsBackgroundCommand(cmd): # Expecting set feature to fail
            result = api.types.status.FAILURE

    return result

def Teardown(tc):
    return api.types.status.SUCCESS
