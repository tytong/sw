// {C} Copyright 2018 Pensando Systems Inc. All rights reserved.

#include <stdio.h>
#include <iostream>

#include "upgrade.hpp"
#include "upgrade_mgr.hpp"
#include "upgrade_app_resp_handlers.hpp"
#include "nic/upgrade_manager/include/c/upgrade_state_machine.hpp"

namespace upgrade {

using namespace std;

// UpgradeService constructor
UpgradeService::UpgradeService(delphi::SdkPtr sk) : UpgradeService(sk, "UpgradeService") {
}

// UpgradeService constructor
UpgradeService::UpgradeService(delphi::SdkPtr sk, string name) {
    // save a pointer to sdk
    sdk_ = sk;
    svcName_ = name;

    // mount objects
    delphi::objects::UpgReq::Mount(sdk_, delphi::ReadMode);
    delphi::objects::UpgResp::Mount(sdk_, delphi::ReadWriteMode);
    delphi::objects::UpgStateReq::Mount(sdk_, delphi::ReadWriteMode);
    delphi::objects::UpgAppResp::Mount(sdk_, delphi::ReadMode);
    delphi::objects::UpgApp::Mount(sdk_, delphi::ReadMode);

    // create upgrade manager event handler
    upgMgr_ = make_shared<UpgradeMgr>(sdk_);

    upgAppRespHdlr_ = make_shared<UpgAppRespHdlr>(sdk_, upgMgr_);
    upgAppRegHdlr_ = make_shared<UpgAppRegReact>(upgMgr_, sdk_);

    // Register upgrade request reactor
    delphi::objects::UpgReq::Watch(sdk_, upgMgr_);
    delphi::objects::UpgAppResp::Watch(sdk_, upgAppRespHdlr_);
    delphi::objects::UpgApp::Watch(sdk_, upgAppRegHdlr_);
    sdk_->WatchMountComplete(upgAppRegHdlr_);

    InitStateMachineVector();
    LogInfo("Upgrade service constructor got called for {}", name);
}

// OnMountComplete gets called when all the objects are mounted
void UpgradeService::OnMountComplete() {
    LogInfo("UpgradeService OnMountComplete got called\n");

    // walk all upgrade request objects and reconcile them
    auto upgReq = upgMgr_->findUpgReq(10);
    if (upgReq == NULL) {
        LogInfo("No active upgrade request");
        return;
    }
    LogInfo("UpgReq found for {}/{}/{}", (upgReq), upgReq->key(), upgReq->meta().ShortDebugString());
    auto upgStateReq = upgMgr_->findUpgStateReq(10);
    if (upgStateReq == NULL) {
        LogInfo("Reconciling outstanding upgrade request with key: {}", upgReq->key());
        upgMgr_->OnUpgReqCreate(upgReq);
    } else {
        LogInfo("Update request in progress. Check if State Machine can be moved.");
        if (upgMgr_->CanMoveStateMachine()) {
            LogInfo("Can move state machine. Moving it forward.");
            UpgReqStateType type = upgStateReq->upgreqstate();
            if (!upgMgr_->InvokePrePostStateHandlers(type)) {
                LogInfo("PrePostState handlers returned false");
                type = UpgStateFailed;
                upgMgr_->SetAppRespFail();
            } else {
                type = upgMgr_->GetNextState();
            }
            upgMgr_->MoveStateMachine(type);
            return;
        } else {
            LogInfo("Cannot move state machine yet");
            return;
        }
    }

    LogInfo("============== UpgradeService Finished Reconciliation ==================\n");
}

} // namespace upgrade
