//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef WORKSPACE_H
#define WORKSPACE_H


#include "../view/skeletonview.h"
#include "../view/contentview.h"
#include "../view/logview.h"
#include <QDockWidget>
#include <libmoex/node/binary.h>

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
    void showNode(moex::Node *node);
private:
    WorkspaceUI *ui_;
};

#endif // WORKSPACE_H
