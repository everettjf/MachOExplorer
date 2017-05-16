//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H


#include "workspace.h"

class WorkspaceManager
{
public:
    WorkspaceManager();

    static Workspace * current();
};

#endif // WORKSPACEMANAGER_H
