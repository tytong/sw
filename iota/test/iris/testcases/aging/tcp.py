#! /usr/bin/python3
import time
import iota.harness.api as api
from iota.test.iris.testcases.aging.aging_utils import *
from iota.test.iris.testcases.security.conntrack.session_info import *
import pdb

TCP_TICKLE_GAP = 15
NUM_TICKLES = 4 
GRACE_TIME = 50 

def Setup(tc):
    return api.types.status.SUCCESS

def Trigger(tc):
    pairs = api.GetLocalWorkloadPairs()
    tc.cmd_cookies = []
    server,client  = pairs[0]
    naples = server
    if not server.IsNaples():
       naples = client
       if not client.IsNaples():
          return api.types.status.SUCCESS
       else:
          client, server = pairs[0]

    req = api.Trigger_CreateExecuteCommandsRequest(serial = True)
    cmd_cookie = "%s(%s) --> %s(%s)" %\
                (server.workload_name, server.ip_address, client.workload_name, client.ip_address)
    api.Logger.info("Starting TCP aging test from %s" % (cmd_cookie))

    cmd_cookie = "halctl clear session"
    api.Trigger_AddNaplesCommand(req, server.node_name, "/nic/bin/halctl clear session")
    tc.cmd_cookies.append(cmd_cookie)

    #Step 0: Update the timeout in the config object
    update_timeout('tcp-timeout', tc.iterators.timeout)
    #update_timeout('tcp-connection-setup', "0s")

    #profilereq = api.Trigger_CreateExecuteCommandsRequest(serial = True)
    #api.Trigger_AddNaplesCommand(profilereq, naples.node_name, "/nic/bin/halctl show nwsec profile --id 11")
    #profcommandresp = api.Trigger(profilereq)
    #cmd = profcommandresp.commands[-1]
    #for command in profcommandresp.commands:
    #    api.PrintCommandResults(command)
    #timeout = get_haltimeout('tcp-timeout', cmd) 
    #tc.config_update_fail = 0
    #if (timeout != timetoseconds(tc.iterators.timeout)):
    #    tc.config_update_fail = 1
    timeout = timetoseconds(tc.iterators.timeout)

    server_port = api.AllocateTcpPort() 
    client_port = api.AllocateTcpPort()
    cmd_cookie = "start server"
    api.Trigger_AddCommand(req, server.node_name, server.workload_name,
                           "nc -l %s" % (server_port), background = True)
    tc.cmd_cookies.append(cmd_cookie)

    cmd_cookie = "Send SYN"
    api.Trigger_AddCommand(req, client.node_name, client.workload_name,
                           "hping3 -c 1 -s %s -p %s -M 0 -L 0 -S %s" % (client_port, server_port, server.ip_address))
    tc.cmd_cookies.append(cmd_cookie)

    #Get Seq + Ack
    api.Trigger_AddNaplesCommand(req, naples.node_name,
                "/nic/bin/halctl show session ".format(server_port, server.ip_address))
    tc.cmd_cookies.append("show session")
    api.Trigger_AddNaplesCommand(req, naples.node_name,
                "/nic/bin/halctl show session --dstport {} --dstip {} --yaml".format(server_port, server.ip_address))
    tc.cmd_cookies.append("show session detail")
    trig_resp1 = api.Trigger(req)
    cmd = trig_resp1.commands[-1]
    for command in trig_resp1.commands:
        api.PrintCommandResults(command)
    iseq_num, iack_num, iwindosz, iwinscale, rseq_num, rack_num, rwindo_sz, rwinscale = get_conntrackinfo(cmd)

    #Send ACK
    req2 = api.Trigger_CreateExecuteCommandsRequest(serial = True)
    api.Trigger_AddCommand(req2, client.node_name, client.workload_name,
               "hping3 -c 1 -s {} -p {} -M {}  -L {} --ack {}".format(client_port, server_port, rack_num, rseq_num, server.ip_address))
    tc.cmd_cookies.append("Send ACK") 

    cmd_cookie = "Before aging show session"
    api.Trigger_AddNaplesCommand(req2, naples.node_name, "/nic/bin/halctl show session --dstport {} --dstip {} | grep ESTABLISHED".format(server_port, server.ip_address))
    tc.cmd_cookies.append(cmd_cookie)

    #Get it from the config
    timeout += get_timeout('tcp-close') + (TCP_TICKLE_GAP * NUM_TICKLES) + GRACE_TIME
    cmd_cookie = "sleep"
    api.Trigger_AddNaplesCommand(req2, naples.node_name, "sleep %s" % timeout, timeout=300)
    tc.cmd_cookies.append(cmd_cookie)

    cmd_cookie = "After aging show session"
    api.Trigger_AddNaplesCommand(req2, naples.node_name, "/nic/bin/halctl show session --dstport {} --dstip {} | grep ESTABLISHED".format(server_port, server.ip_address))
    tc.cmd_cookies.append(cmd_cookie)

    cmd_cookie = "show session yaml"
    api.Trigger_AddNaplesCommand(req2, naples.node_name, "/nic/bin/halctl show session --yaml".format(server_port, server.ip_address))
    tc.cmd_cookies.append(cmd_cookie)

    trig_resp2 = api.Trigger(req2)
    term_resp2 = api.Trigger_TerminateAllCommands(trig_resp2)
    tc.resp2 = api.Trigger_AggregateCommandsResponse(trig_resp2, term_resp2)
   
    term_resp1 = api.Trigger_TerminateAllCommands(trig_resp1)
    tc.resp = api.Trigger_AggregateCommandsResponse(trig_resp1, term_resp1)
    return api.types.status.SUCCESS

def Verify(tc):
    if tc.resp is None:
        return api.types.status.FAILURE

    #if tc.config_update_fail == 1:
    #    return api.types.status.FAILURE

    result = api.types.status.SUCCESS
    cookie_idx = 0
    for cmd in tc.resp.commands:
        api.Logger.info("Results for %s" % (tc.cmd_cookies[cookie_idx]))
        api.PrintCommandResults(cmd)
        if cmd.exit_code != 0 and not api.Trigger_IsBackgroundCommand(cmd):
            #This is expected so dont set failure for this case
            result = api.types.status.FAILURE
        cookie_idx += 1

    result = api.types.status.SUCCESS
    grep_cmd = "TCP"
    for cmd in tc.resp2.commands:
        api.Logger.info("Results for %s" % (tc.cmd_cookies[cookie_idx]))
        api.PrintCommandResults(cmd)
        if cmd.exit_code != 0 and not api.Trigger_IsBackgroundCommand(cmd):
            #This is expected so dont set failure for this case
            if tc.cmd_cookies[cookie_idx].find("After aging") != -1 and cmd.stdout == '':
               result = api.types.status.SUCCESS
            elif tc.cmd_cookies[cookie_idx].find("show session yaml") != -1:
               result = api.types.status.SUCCESS
            else:
               result = api.types.status.FAILURE
        if tc.cmd_cookies[cookie_idx].find("clear session") != -1:
           if cmd.stdout != '':
              result = api.types.status.FAILURE
        elif tc.cmd_cookies[cookie_idx].find("Before aging") != -1:
           #Session were not established ?
           if cmd.stdout.find("") == -1:
               result = api.types.status.FAILURE
        elif tc.cmd_cookies[cookie_idx].find("After aging") != -1:
           #Check if sessions were aged
           if cmd.stdout.find(grep_cmd) != -1:
               result = api.types.status.FAILURE
        cookie_idx += 1
    return result

def Teardown(tc):
    return api.types.status.SUCCESS
