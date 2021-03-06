#! /usr/bin/python3
import os
import time
import iota.harness.api as api
from iota.test.iris.testcases.alg.alg_utils import *
from iota.test.iris.testcases.alg.sunrpc.sunrpc_utils import *
import pdb

def Setup(tc):
    return api.types.status.SUCCESS

def Trigger(tc):
    triplet = GetThreeWorkloads()
    server = triplet[0][0]
    client1 = triplet[0][1]
    client2 = triplet[0][2]
    tc.cmd_cookies = []

    naples = server
    if not server.IsNaples():
       naples = client1
       if not client1.IsNaples():
          naples = client2
          if not client2.IsNaples():
             return api.types.status.SUCCESS

    req = api.Trigger_CreateExecuteCommandsRequest(serial = True)
    tc.cmd_descr = "Server: %s(%s) <--> Client: %s(%s)" %\
                   (server.workload_name, server.ip_address, client1.workload_name, client1.ip_address)
    api.Logger.info("Starting SUNRPC test from %s" % (tc.cmd_descr))

    SetupNFSServer(server, client1)

    api.Trigger_AddCommand(req, server.node_name, server.workload_name,
                           "sh -c 'ls -al /home/sunrpcmntdir | grep sunrpc_file.txt'")
    tc.cmd_cookies.append("Before rpc")

    api.Trigger_AddCommand(req, client1.node_name, client1.workload_name,
                           "sudo sh -c 'mkdir -p /home/sunrpcdir && mount %s:/home/sunrpcmntdir /home/sunrpcdir' "%(server.ip_address), timeout=180)
    tc.cmd_cookies.append("Create mount point")
    
    api.Trigger_AddCommand(req, client1.node_name, client1.workload_name,
                           "sudo chmod 777 /home/sunrpcdir")
    tc.cmd_cookies.append("add permission")

    api.Trigger_AddCommand(req, client1.node_name, client1.workload_name,
                           "mv sunrpcdir/sunrpc_file.txt /home/sunrpcdir/", timeout=180)
    tc.cmd_cookies.append("Create file")

    api.Trigger_AddCommand(req, client1.node_name, client1.workload_name,
                           "ls -al /home/sunrpcdir")
    tc.cmd_cookies.append("verify file")

    api.Trigger_AddCommand(req, server.node_name, server.workload_name,
                           "ls -al /home/sunrpcmntdir/")
    tc.cmd_cookies.append("After rpc")

    api.Trigger_AddCommand(req, server.node_name, server.workload_name,
                           "sh -c 'cat /home/sunrpcmntdir/sunrpc_file.txt'")
    tc.cmd_cookies.append("After rpc")

    # Add Naples command validation
    api.Trigger_AddNaplesCommand(req, naples.node_name,
                           "/nic/bin/halctl show session --alg sun_rpc")
    tc.cmd_cookies.append("show session")
    api.Trigger_AddNaplesCommand(req, naples.node_name,
                           "/nic/bin/halctl show nwsec flow-gate | grep SUN_RPC")
    tc.cmd_cookies.append("show security flow-gate")

    trig_resp = api.Trigger(req)
    term_resp = api.Trigger_TerminateAllCommands(trig_resp)
    tc.resp = api.Trigger_AggregateCommandsResponse(trig_resp, term_resp)

    #Get it from flow gate
    dport = 2049

    req = api.Trigger_CreateExecuteCommandsRequest(serial = True)

    api.Trigger_AddCommand(req, client2.node_name, client2.workload_name,
                           "hping3 -c 1 -s 45535 -p {} -d 0 -S {}".format(dport, "192.168.100.200"))
    tc.cmd_cookies.append("ping unknown host")

    # Get the timeout from the config
    api.Trigger_AddNaplesCommand(req, naples.node_name,
                           "/nic/bin/halctl show session --srcip %s | grep SYN"%("10.10.10.10"))
    tc.cmd_cookies.append("show session different source")

    api.Trigger_AddNaplesCommand(req, naples.node_name,
                           "sleep 100", timeout=120)
    tc.cmd_cookies.append("sleep")

    api.Trigger_AddNaplesCommand(req, naples.node_name,
                           "/nic/bin/halctl show nwsec flow-gate | grep SUN_RPC")
    tc.cmd_cookies.append("After flow-gate ageout")

    trig_resp = api.Trigger(req)
    term_resp = api.Trigger_TerminateAllCommands(trig_resp)
    tc.resp2 = api.Trigger_AggregateCommandsResponse(trig_resp, term_resp)

    CleanupNFSServer(server, client1)

    return api.types.status.SUCCESS

def Verify(tc):
    if tc.resp is None:
        return api.types.status.FAILURE

    result = api.types.status.SUCCESS
    api.Logger.info("Results for %s" % (tc.cmd_descr))
    cookie_idx = 0
    for cmd in tc.resp.commands:
        api.PrintCommandResults(cmd)
        if cmd.exit_code != 0 and not api.Trigger_IsBackgroundCommand(cmd):
            if tc.cmd_cookies[cookie_idx].find("Before") != -1:
                result = api.types.status.SUCCESS
            else:
                result = api.types.status.FAILURE
        if tc.cmd_cookies[cookie_idx].find("show session") != -1 and \
           cmd.stdout == '':
           result = api.types.status.FAILURE
        cookie_idx += 1
    for cmd in tc.resp2.commands:
        api.PrintCommandResults(cmd)
        if cmd.exit_code != 0 and not api.Trigger_IsBackgroundCommand(cmd):
            if tc.cmd_cookies[cookie_idx].find("Before") != -1 or \
               tc.cmd_cookies[cookie_idx].find("ping unknown host") != -1 or \
               tc.cmd_cookies[cookie_idx].find("After flow-gate ageout") != -1 or\
               tc.cmd_cookies[cookie_idx].find("show session different source") != -1:
                result = api.types.status.SUCCESS
            else:
                result = api.types.status.FAILURE
        if tc.cmd_cookies[cookie_idx].find("After flow-gate ageout") != -1 and \
            cmd.stdout != '':
            result = api.types.status.FAILURE
        if tc.cmd_cookies[cookie_idx].find("ping unknown host") != -1 and \
           cmd.exit_code == 0:
           result = api.types.status.FAILURE
        if tc.cmd_cookies[cookie_idx].find("show session different source") != -1 and \
           cmd.stdout != '':
           result = api.types.status.FAILUR
        cookie_idx += 1

    return result

def Teardown(tc):
    return api.types.status.SUCCESS
