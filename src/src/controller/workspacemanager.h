//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include "../view/layoutview.h"
#include "../view/contentview.h"
#include "../view/logview.h"
#include <QDockWidget>
#include <libmoex/moex.h>
#include <libmoex/moex-view.h>

struct WorkspaceUI{
    LayoutView *layout;
    ContentView *content;
    LogView *log;

    QDockWidget *layoutDock;
    QDockWidget *contentDock;
    QDockWidget *logDock;
};



class Workspace
{
public:
    Workspace();

    WorkspaceUI *ui(){return ui_;}

    void openFile(const QString & filePath);
    void addLog(const QString & log);
    void showNode(moex::ViewNode *node);
private:
    WorkspaceUI *ui_;
};


class WorkspaceManager
{
public:
    WorkspaceManager();

    static WorkspaceManager * Instance();

    void newWorkspace();
};

#endif // WORKSPACEMANAGER_H
