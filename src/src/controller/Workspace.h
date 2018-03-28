//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include "src/view/LayoutDockWidget.h"
#include "src/view/InfoDockWidget.h"
#include "src/view/LogDockWidget.h"
#include "src/view/HexDockWidget.h"
#include "src/view/HelpDockWidget.h"
#include <QDockWidget>
#include <libmoex/moex.h>
#include <libmoex/moex-view.h>

struct WorkspaceUI{
    InfoDockWidget *info;
    LayoutDockWidget *layout;
    LogDockWidget *log;
    HexDockWidget *hex;
    HelpDockWidget *help;

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
