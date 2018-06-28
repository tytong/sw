// {C} Copyright 2018 Pensando Systems Inc. All rights reserved.

#include <stdio.h>
#include <iostream>

#include "upgrade.hpp"
#include "upgrade_mgr.hpp"
#include "upgrade_app_resp_handlers.hpp"
#include "nic/upgrade_manager/include/c/upgrade_state_machine.hpp"

namespace upgrade {

using namespace std;

void UpgradeMgr::RegNewApp(string name) {
    if (appRegMap_[name] == false) {
        LogInfo("App not registered. Registering {} now.", name);
        appRegMap_[name] = true;
    } else {
        LogInfo("App {} already registered.", name);
    }
}

UpgReqStateType UpgradeMgr::GetNextState(void) {
    UpgReqStateType  reqType;
    vector<delphi::objects::UpgStateReqPtr> upgReqStatusList = delphi::objects::UpgStateReq::List(sdk_);
    for (vector<delphi::objects::UpgStateReqPtr>::iterator reqStatus=upgReqStatusList.begin(); reqStatus!=upgReqStatusList.end(); ++reqStatus) {
        reqType = (*reqStatus)->upgreqstate();
        break;
    }
    if (GetAppRespFail() && (reqType != UpgFailed) && (reqType != Cleanup)) {
        LogInfo("Some application(s) responded with failure");
        return UpgFailed;
    }
    if (reqType == UpgSuccess) 
        upgPassed_ = true;
    return StateMachine[reqType].stateNext;
}

bool UpgradeMgr::IsRespTypeFail(UpgRespStateType type) {
    bool ret = false;
    switch (type) {
        case UpgReqRcvdFail:
        case PreUpgStateFail:
        case ProcessesQuiescedFail:
        case PostBinRestartFail:
        case DataplaneDowntimePhase1StartFail:
        case DataplaneDowntimeAdminQHandlingFail:
        case DataplaneDowntimePhase2StartFail:
        case CleanupFail:
        case UpgSuccessFail:
        case UpgFailedFail:
        case UpgAbortedFail:
            ret = true;
        default:
            break;
    }
    return ret;
}

UpgRespStateType UpgradeMgr::GetFailRespType(UpgReqStateType type) {
    return StateMachine[type].stateFailResp;
}

UpgRespStateType UpgradeMgr::GetPassRespType(UpgReqStateType type) {
    return StateMachine[type].statePassResp;
}

bool UpgradeMgr::CanMoveStateMachine(void) {
    UpgRespStateType passType, failType;
    UpgReqStateType  reqType;
    bool ret = true;
    LogInfo("Checking if state machine can be moved forward");
    //Find UpgStateReq object
    vector<delphi::objects::UpgStateReqPtr> upgReqStatusList = delphi::objects::UpgStateReq::List(sdk_);
    for (vector<delphi::objects::UpgStateReqPtr>::iterator reqStatus=upgReqStatusList.begin(); reqStatus!=upgReqStatusList.end(); ++reqStatus) {
        reqType = (*reqStatus)->upgreqstate();
        passType = GetPassRespType(reqType);
        failType = GetFailRespType(reqType);
        //LogInfo("reqType/passType/failType: {}/{}/{}", reqType, passType, failType);
    }

    //check if all responses have come
    vector<delphi::objects::UpgAppRespPtr> upgAppRespList = delphi::objects::UpgAppResp::List(sdk_);
    if (upgAppRespList.size() != appRegMap_.size()) {
        ret = false;
        LogInfo("Number of responses from Applications {} is not same as the number of applications {}", upgAppRespList.size(), appRegMap_.size());
    } else {
        for (vector<delphi::objects::UpgAppRespPtr>::iterator appResp=upgAppRespList.begin(); appResp!=upgAppRespList.end(); ++appResp) {
            if (((*appResp)->upgapprespval() != passType) &&
                ((*appResp)->upgapprespval() != failType)){
                LogInfo("Application {} still processing {}", (*appResp)->key(), UpgReqStateTypeToStr(reqType));
                ret = false;
            } else if ((*appResp)->upgapprespval() == passType) {
                LogInfo("Got pass from application {}/{}", (*appResp)->key(), ((*appResp))->meta().ShortDebugString());
            } else {
                LogInfo("Got fail from application {}", (*appResp)->key());
            }
        }
    }
    if (ret) {
        LogInfo("Got pass/fail response from all applications. Can move state machine.");
        UpgPostStateFunc postStFunc = StateMachine[reqType].postStateFunc;
        if (postStFunc) {
            LogInfo("Going to invoke post-state handler function");
            if (!(postStateHandlers->*postStFunc)()) {
                LogInfo("post-state handler function returned false");
                SetAppRespFail();
            }
        }
    }
    return ret;
}

string UpgradeMgr::UpgReqStateTypeToStr(UpgReqStateType type) {
    return StateMachine[type].upgReqStateTypeToStr;
}

bool UpgradeMgr::GetAppRespFail(void) {
    return appRespFail_;
}

void UpgradeMgr::ResetAppResp(void) {
    appRespFail_ = false;
}

void UpgradeMgr::SetAppRespFail(void) {
    appRespFail_ = true;
}

void UpgradeMgr::AppendAppRespFailStr (string str) {
    appRespFailStrList_.push_back(str);
}

delphi::error UpgradeMgr::DeleteUpgMgrResp (void) {
    return upgMgrResp_->DeleteUpgMgrResp();
}
delphi::error UpgradeMgr::MoveStateMachine(UpgReqStateType type) {
    //Find UpgStateReq object
    LogInfo("UpgradeMgr::MoveStateMachine {}", type);
    UpgPreStateFunc preStFunc = StateMachine[type].preStateFunc;
    if (preStFunc) {
        LogInfo("Going to invoke pre-state handler function");
        if (!(preStateHandlers->*preStFunc)()) {
            LogInfo("pre-state handler function returned false");
            type = UpgFailed;
            SetAppRespFail();
        }
    }
    vector<delphi::objects::UpgStateReqPtr> upgReqStatusList = delphi::objects::UpgStateReq::List(sdk_);
    for (vector<delphi::objects::UpgStateReqPtr>::iterator reqStatus=upgReqStatusList.begin(); reqStatus!=upgReqStatusList.end(); ++reqStatus) {
        (*reqStatus)->set_upgreqstate(type);
        sdk_->SetObject(*reqStatus);
    }
    if (type == UpgStateTerminal) {
        UpgRespType respType = UpgRespAbort;
        if (GetAppRespFail())
            respType = UpgRespFail;
        if (upgPassed_ && !upgAborted_)
            respType = UpgRespPass;
        upgMgrResp_->UpgradeFinish(respType, appRespFailStrList_);
        if (appRespFailStrList_.empty()) {
            LogInfo("Emptied all the responses from applications to agent");
            ResetAppResp();
            upgPassed_ = false;
            upgAborted_ = false;
        }
    }
    if (type != UpgStateTerminal)
        LogInfo("========== Upgrade state moved to {} ==========", UpgReqStateTypeToStr(type));
    return delphi::error::OK();
}

// OnUpgReqCreate gets called when UpgReq object is created
delphi::error UpgradeMgr::OnUpgReqCreate(delphi::objects::UpgReqPtr req) {
    LogInfo("UpgReq got created for {}/{}", req, req->meta().ShortDebugString());

    UpgReqStateType type = UpgReqRcvd;
    // find the status object
    auto upgReqStatus = findUpgStateReq(req->key());
    if (upgReqStatus == NULL) {
        // create it since it doesnt exist
        UpgPreStateFunc preStFunc = StateMachine[UpgReqRcvd].preStateFunc;
        if (preStFunc) {
            LogInfo("Going to invoke pre-state handler function");
            if (!(preStateHandlers->*preStFunc)()) {
                LogInfo("pre-state handler function returned false");
                type = UpgFailed;
                SetAppRespFail();
            }
        }
        RETURN_IF_FAILED(createUpgStateReq(req->key(), type));
    }

    return delphi::error::OK();
}

// OnUpgReqDelete gets called when UpgReq object is deleted
delphi::error UpgradeMgr::OnUpgReqDelete(delphi::objects::UpgReqPtr req) {
    LogInfo("UpgReq got deleted");
    auto upgReqStatus = findUpgStateReq(req->key());
    if (upgReqStatus != NULL) {
        LogInfo("Deleting Upgrade Request Status");
        sdk_->DeleteObject(upgReqStatus);
    }
    return delphi::error::OK();
}

delphi::error UpgradeMgr::StartUpgrade(uint32_t key) {
    delphi::objects::UpgStateReqPtr upgReqStatus = findUpgStateReq(key);
    if (upgReqStatus != NULL) {
        upgReqStatus->set_upgreqstate(UpgReqRcvd);
        sdk_->SetObject(upgReqStatus);
        LogInfo("Updated Upgrade Request Status UpgReqRcvd");
        return delphi::error::OK();
    }
    return delphi::error("Did not find UpgStateReqPtr");
}

delphi::error UpgradeMgr::AbortUpgrade(uint32_t key) {
    delphi::objects::UpgStateReqPtr upgReqStatus = findUpgStateReq(key);
    if (upgReqStatus != NULL) {
        upgAborted_ = true;
        upgReqStatus->set_upgreqstate(UpgAborted);
        sdk_->SetObject(upgReqStatus);
        LogInfo("Updated Upgrade Request Status UpgAborted");
        return delphi::error::OK();
    }
    return delphi::error("Did not find UpgStateReqPtr");
}

// OnUpgReqCmd gets called when UpgReqCmd attribute changes
delphi::error UpgradeMgr::OnUpgReqCmd(delphi::objects::UpgReqPtr req) {
    // start or abort?
    if (req->upgreqcmd() == UpgStart) {
        LogInfo("Start Upgrade");
        return StartUpgrade(req->key());
    } else if (req->upgreqcmd() == UpgAbort) {
        LogInfo("Abort Upgrade");
        return AbortUpgrade(req->key());
    }
    return delphi::error("Cannot decipher the upgreqcmd");
}

// createUpgStateReq creates a upgrade request status object
delphi::error UpgradeMgr::createUpgStateReq(uint32_t id, UpgReqStateType status) {
    // create an object
    delphi::objects::UpgStateReqPtr req = make_shared<delphi::objects::UpgStateReq>();
    req->set_key(id);
    req->set_upgreqstate(status);
    //TODO figure out UpgTypeDisruptive vs UpgTypeNonDisruptive
    req->set_upgreqtype(UpgTypeDisruptive);

    // add it to database
    sdk_->SetObject(req);

    LogInfo("Created upgrade request status object for id {} state {} req: {}", id, status, req);

    return delphi::error::OK();
}

//  ffindUpgReqStat::objects::usinds the upgrade request status object
delphi::objects::UpgStateReqPtr UpgradeMgr::findUpgStateReq(uint32_t id) {
    delphi::objects::UpgStateReqPtr req = make_shared<delphi::objects::UpgStateReq>();
    req->set_key(id);

    // find the object
    delphi::BaseObjectPtr obj = sdk_->FindObject(req);

    return static_pointer_cast<delphi::objects::UpgStateReq>(obj);
}

} // namespace upgrade
