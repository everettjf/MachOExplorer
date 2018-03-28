//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include "src/CentralWidget.h"
#include "src/dock/LayoutDockWidget.h"
#include "src/dock/LogDockWidget.h"
#include "src/dock/HexDockWidget.h"
#include "src/dock/HelpDockWidget.h"
#include <QDockWidget>
#include <libmoex/moex.h>
#include <libmoex/moex-view.h>

struct WorkspaceUI{
    CentralWidget *main;

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
