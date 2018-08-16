#! /usr/bin/python3

import copy
from iris.test.rdma.utils import *
from infra.common.glopts import GlobalOptions
from infra.common.logging import logger as logger
def Setup(infra, module):
    return

def Teardown(infra, module):
    return

def TestCaseSetup(tc):
    logger.info("RDMA TestCaseSetup() Implementation.")
    rs = tc.config.rdmasession

    # Read RQ pre state
    rs.lqp.rq.qstate.Read()
    tc.pvtdata.rq_pre_qstate = rs.lqp.rq.qstate.data

    # Read CQ pre state
    rs.lqp.rq_cq.qstate.Read()
    tc.pvtdata.rq_cq_pre_qstate = rs.lqp.rq_cq.qstate.data

    # Read ASYNC_EQ pre state
    rs.lqp.pd.ep.intf.lif.async_eq.qstate.Read()
    tc.pvtdata.async_eq_pre_qstate = rs.lqp.pd.ep.intf.lif.async_eq.qstate.data

    return

def TestCaseTrigger(tc):
    logger.info("RDMA TestCaseTrigger() Implementation.")
    return

def TestCaseStepTrigger(tc, step):
    logger.info("RDMA TestCaseStepTrigger() Implementation with step_id: %d" % (step.step_id))
    if (GlobalOptions.dryrun): return True
    return
               
def TestCaseVerify(tc):
    if (GlobalOptions.dryrun): return True
    logger.info("RDMA TestCaseVerify() Implementation.")
    return True

def TestCaseStepVerify(tc, step):
    if (GlobalOptions.dryrun): return True
    logger.info("RDMA TestCaseStepVerify() Implementation.")
    logger.info("step id: %d" %(step.step_id))

    rs = tc.config.rdmasession
    rs.lqp.rq.qstate.Read()
    ring0_mask = (rs.lqp.num_rq_wqes - 1)

    rs.lqp.rq.qstate.Read()
    tc.pvtdata.rq_post_qstate = rs.lqp.rq.qstate.data

    if step.step_id == 0 or step.step_id == 1:
        ############     RQ VALIDATIONS #################
        # verify that e_psn is NOT incremented
        if not VerifyFieldModify(tc, tc.pvtdata.rq_pre_qstate, tc.pvtdata.rq_post_qstate, 'e_psn', 0):
            return False

        # verify that msn is NOT incremented
        if not VerifyFieldModify(tc, tc.pvtdata.rq_pre_qstate, tc.pvtdata.rq_post_qstate, 'msn', 0):
            return False

        # verify that proxy_cindex is NOT incremented 
        if not VerifyFieldMaskModify(tc, tc.pvtdata.rq_pre_qstate, tc.pvtdata.rq_post_qstate, 'proxy_cindex', ring0_mask, 0):
            return False

        # verify that token_id is incremented by 1
        if not VerifyFieldModify(tc, tc.pvtdata.rq_pre_qstate, tc.pvtdata.rq_post_qstate, 'token_id', 1):
            return False

        # verify that nxt_to_go_token_id is incremented by 1
        if not VerifyFieldModify(tc, tc.pvtdata.rq_pre_qstate, tc.pvtdata.rq_post_qstate, 'nxt_to_go_token_id', 1):
            return False

        # verify that nak_prune is set to 1
        if not VerifyFieldAbsolute(tc, tc.pvtdata.rq_post_qstate, 'nak_prune', 1):
            return False

    elif step.step_id == 2:
        ############     RQ VALIDATIONS #################
        # verify that e_psn is NOT incremented
        if not VerifyFieldModify(tc, tc.pvtdata.rq_pre_qstate, tc.pvtdata.rq_post_qstate, 'e_psn', 0):
            return False

        # verify that msn is NOT incremented
        if not VerifyFieldModify(tc, tc.pvtdata.rq_pre_qstate, tc.pvtdata.rq_post_qstate, 'msn', 0):
            return False

        # verify that proxy_cindex is NOT incremented
        if not VerifyFieldMaskModify(tc, tc.pvtdata.rq_pre_qstate, tc.pvtdata.rq_post_qstate, 'proxy_cindex', ring0_mask, 0):
            return False

        # verify that token_id is incremented by 1
        if not VerifyFieldModify(tc, tc.pvtdata.rq_pre_qstate, tc.pvtdata.rq_post_qstate, 'token_id', 1):
            return False

        # verify that nxt_to_go_token_id is NOT incremented
        if not VerifyFieldModify(tc, tc.pvtdata.rq_pre_qstate, tc.pvtdata.rq_post_qstate, 'nxt_to_go_token_id', 0):
            return False

        # verify that nak_prune is set to 0
        if not VerifyFieldAbsolute(tc, tc.pvtdata.rq_post_qstate, 'nak_prune', 0):
            return False

        # verify that state is now moved to ERR (2)
        if not VerifyErrQState(tc):
            return False
    
        ############   ASYNC EQ VALIDATIONS #################
        if not ValidateAsyncEQChecks(tc):
            return False

    elif step.step_id == 3:

        if not ValidatePostSyncCQChecks(tc):
            return False 

    # update current as pre_qstate ... so next step_id can use it as pre_qstate
    tc.pvtdata.rq_pre_qstate = copy.deepcopy(rs.lqp.rq.qstate.data)
    tc.pvtdata.rq_cq_pre_qstate = copy.deepcopy(rs.lqp.rq_cq.qstate.data)

    return True

def TestCaseTeardown(tc):
    if (GlobalOptions.dryrun): return
    logger.info("RDMA TestCaseTeardown() Implementation.")
    ResetErrQState(tc)
    return
