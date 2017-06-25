//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef WORKSPACE_H
#define WORKSPACE_H


#include "../view/skeletonview.h"
#include "../view/contentview.h"
#include "../view/logview.h"
#include "../view/documentview.h"
#include <QDockWidget>
#include <libmoex/binary.h>

struct WorkspaceUI{
    SkeletonView *skeleton;
    ContentView *content;
    LogView *log;
    DocumentView *document;

    QDockWidget *skeletonDock;
    QDockWidget *contentDock;
    QDockWidget *logDock;
    QDockWidget *documentDock;
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
