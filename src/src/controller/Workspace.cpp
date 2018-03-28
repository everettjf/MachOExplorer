//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "Workspace.h"
#include "src/MainWindow.h"

Workspace::Workspace()
{
    ui_ = new WorkspaceUI();
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
    ui_->main->showNode(node);
}


Workspace *Workspace::Instance()
{
    static Workspace w;
    return & w;
}

