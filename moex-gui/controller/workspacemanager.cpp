//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "workspacemanager.h"


Workspace::Workspace()
{

}

void Workspace::openFile(const QString &filePath)
{
    ui_->layout->openFile(filePath);
}

void Workspace::addLog(const QString &log)
{
    ui_->log->addLine(log);
}

void Workspace::showNode(moex::ViewNode *node)
{
    ui_->content->showNode(node);
}


///////////////////////////////////////////////////

WorkspaceManager::WorkspaceManager()
{

}

Workspace *WorkspaceManager::current()
{
    static Workspace workspace;
    return & workspace;
}
