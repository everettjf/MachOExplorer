//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include "src/view/LayoutDockWidget.h"
#include "src/view/MainContentWidget.h"
#include "src/view/LogDockWidget.h"
#include "src/view/BinaryDockWidget.h"
#include <QDockWidget>
#include <libmoex/moex.h>
#include <libmoex/moex-view.h>

struct WorkspaceUI{
    MainContentWidget *content;

    LayoutDockWidget *layout;
    LogDockWidget *log;
    BinaryDockWidget *binary;

    QList<QDockWidget *> dockWidgets;
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
