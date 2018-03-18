//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "BaseController.h"
#include "Workspace.h"

BaseController::BaseController()
{

}

void BaseController::log(const QString &log)
{
    WS()->addLog(log);
}
