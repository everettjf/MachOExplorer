//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "workspacemanager.h"

WorkspaceManager::WorkspaceManager()
{

}

Workspace *WorkspaceManager::current()
{
    static Workspace workspace;
    return & workspace;
}
