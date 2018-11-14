#! /usr/bin/python3
import time
import iota.harness.api as api
def Setup(tc):
    return api.types.status.SUCCESS

def Trigger(tc):
    pairs = api.GetRemoteWorkloadPairs()
    w1 = pairs[0][0]
    w2 = pairs[0][1]
    req = api.Trigger_CreateExecuteCommandsRequest(serial = True)
    tc.cmd_descr = "Server: %s(%s) <--> Client: %s(%s)" %\
            (w1.workload_name, w1.ip_address, w2.workload_name, w2.ip_address)
    api.Logger.info("Starting If_Up test from %s" % (tc.cmd_descr))

    #oob interface
    api.Trigger_AddNaplesCommand(req, w1.node_name, "taskset 0x8 ifconfig oob_mnic0 10.10.10.10 netmask 255.255.255.0 up")
    api.Trigger_AddNaplesCommand(req, w2.node_name, "taskset 0x8 ifconfig oob_mnic0 10.10.10.11 netmask 255.255.255.0 up")

    #inb0 interface
    api.Trigger_AddNaplesCommand(req, w1.node_name, "taskset 0x8 ifconfig inb_mnic0 20.20.20.20 netmask 255.255.255.0 up")
    api.Trigger_AddNaplesCommand(req, w2.node_name, "taskset 0x8 ifconfig inb_mnic0 20.20.20.21 netmask 255.255.255.0 up")

    #inb1 interface
    api.Trigger_AddNaplesCommand(req, w1.node_name, "taskset 0x8 ifconfig inb_mnic1 30.30.30.30 netmask 255.255.255.0 up")
    api.Trigger_AddNaplesCommand(req, w2.node_name, "taskset 0x8 ifconfig inb_mnic1 30.30.30.31 netmask 255.255.255.0 up")

    #int_mnic0 interface
    api.Trigger_AddNaplesCommand(req, w1.node_name, "taskset 0x8 ifconfig int_mnic0 40.40.40.40 netmask 255.255.255.0 up")
    api.Trigger_AddNaplesCommand(req, w2.node_name, "taskset 0x8 ifconfig int_mnic0 40.40.40.41 netmask 255.255.255.0 up")


    trig_resp = api.Trigger(req)

    term_resp = api.Trigger_TerminateAllCommands(trig_resp)

    tc.resp = api.Trigger_AggregateCommandsResponse(trig_resp, term_resp)

    return api.types.status.SUCCESS

def Verify(tc):
    if tc.resp is None:
        return api.types.status.FAILURE
    result = api.types.status.SUCCESS

    api.Logger.info("If_Up Results for %s" % (tc.cmd_descr))
    for cmd in tc.resp.commands:
        api.PrintCommandResults(cmd)
        if cmd.exit_code != 0 and not api.Trigger_IsBackgroundCommand(cmd):
            result = api.types.status.FAILURE
    return result

def Teardown(tc):
    return api.types.status.SUCCESS

