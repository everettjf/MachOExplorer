//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "BaseController.h"
#include "WorkspaceManager.h"

BaseController::BaseController()
{

}

void BaseController::log(const QString &log)
{
    workspace->addLog(log);
}
