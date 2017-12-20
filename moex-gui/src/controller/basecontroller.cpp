//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "basecontroller.h"
#include "workspacemanager.h"

BaseController::BaseController()
{

}

void BaseController::log(const QString &log)
{
    workspace->addLog(log);
}
