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
#include "src/dock/InformationDockWidget.h"
#include "src/dock/SourceCodeDockWidget.h"
#include <QDockWidget>
#include <libmoex/moex.h>
#include <libmoex/moex-view.h>

struct WorkspaceUI{
    CentralWidget *main;

    LayoutDockWidget *layout;
    LogDockWidget *log;
    HexDockWidget *hex;
    InformationDockWidget *information;
    SourceCodeDockWidget *sourcecode;

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
    void selectHexRange(void *data,uint64_t size);
    void clearHexSelection();
    void setInformation(const QString & info);
private:
    WorkspaceUI *ui_;
};

inline Workspace * WS(){
    return Workspace::Instance();
}

#endif // WORKSPACEMANAGER_H
