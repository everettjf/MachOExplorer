//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include "../view/skeletonview.h"
#include "../view/contentview.h"
#include "../view/logview.h"
#include <QDockWidget>
#include <libmoex/moex.h>
#include <libmoex/moex-view.h>

struct WorkspaceUI{
    SkeletonView *skeleton;
    ContentView *content;
    LogView *log;

    QDockWidget *skeletonDock;
    QDockWidget *contentDock;
    QDockWidget *logDock;
};



class Workspace
{
public:
    Workspace();

    void set_ui(WorkspaceUI*ui){ui_ = ui;}

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

    static Workspace * current();
};

#endif // WORKSPACEMANAGER_H
