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
    if(!node)
        return;

    // Lazy init
    node->Init();

    // central widget
    ui_->main->showTableViewData(node->table().get());

    // hex dock widget
    ui_->hex->showViewData(node->binary().get());
}


Workspace *Workspace::Instance()
{
    static Workspace w;
    return & w;
}

