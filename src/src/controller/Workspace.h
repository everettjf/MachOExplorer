//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include "src/view/LayoutView.h"
#include "src/view/ContentView.h"
#include "src/view/LogView.h"
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
    static Workspace *Instance();
public:
    Workspace();

    WorkspaceUI *ui(){return ui_;}

    void openFile(const QString & filePath);
    void addLog(const QString & log);
    void showNode(moex::ViewNode *node);
private:
    WorkspaceUI *ui_;
};

inline Workspace * WS(){
    return Workspace::Instance();
}

#endif // WORKSPACEMANAGER_H
