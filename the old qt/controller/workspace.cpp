//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "workspace.h"

Workspace::Workspace()
{

}

void Workspace::openFile(const QString &filePath)
{
    ui_->skeleton->openFile(filePath);
}

void Workspace::addLog(const QString &log)
{
    ui_->log->addLine(log);
}

void Workspace::showNode(moex::Node *node)
{
    ui_->content->showNode(node);
}
