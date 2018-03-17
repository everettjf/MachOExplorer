//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "workspacemanager.h"
#include "../view/mainwindow.h"
#include <QDesktopWidget>

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
    ui_->content->showNode(node);
}


///////////////////////////////////////////////////

WorkspaceManager::WorkspaceManager()
{

}

WorkspaceManager *WorkspaceManager::Instance()
{
    static WorkspaceManager wm;
    return & wm;
}

void WorkspaceManager::newWorkspace()
{
    auto *w = new MainWindow();
    w->setGeometry(QApplication::desktop()->availableGeometry().adjusted(200, 100, -200, -100));
    w->show();
}
