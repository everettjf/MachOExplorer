//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "Workspace.h"
#include "src/MainWindow.h"
#include <QFileInfo>

Workspace::Workspace()
{
    ui_ = new WorkspaceUI();
}

void Workspace::openFile(const QString &filePath)
{
    if (filePath.isEmpty()) {
        addLog("[workspace] openFile ignored: empty path");
        return;
    }
    if (!QFileInfo::exists(filePath)) {
        addLog(QString("[workspace] openFile ignored: missing file %1").arg(filePath));
        return;
    }
    currentFilePath_ = filePath;
    ui_->layout->openFile(filePath);
}

void Workspace::addLog(const QString &log)
{
    ui_->log->addLine(log);
}

void Workspace::showNode(moex::ViewNode *node)
{
    if(!node)
        return;

    // Lazy init
    node->Init();

    // central widget
    ui_->main->showTableViewData(node->table().get());

    // hex dock widget
    ui_->hex->showViewData(node->binary().get());
}
void Workspace::selectHexRange(void *data,uint64_t size)
{
    if(!data){
        clearHexSelection();
        return;
    }

    ui_->hex->selectRange((uint64_t)data,size);
}

void Workspace::clearHexSelection()
{
    ui_->hex->clearSelection();
}


void Workspace::setInformation(const QString & info)
{
    ui_->information->setContent(info);
}


Workspace *Workspace::Instance()
{
    static Workspace w;
    return & w;
}
