//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "LayoutDockWidget.h"
#include "src/utility/Utility.h"
#include "src/controller/Workspace.h"
#include "src/controller/LayoutController.h"

#include <QHBoxLayout>

LayoutDockWidget::LayoutDockWidget(QWidget *parent) : QDockWidget(parent)
{
    setWindowTitle(tr("Layout"));

    controller = nullptr;

    treeView = new LayoutTreeView(this);
    treeView->setMinimumWidth(200);
    treeView->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    treeView->setHeaderHidden(true);
    setWidget(treeView);

    connect(treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickedTreeNode(QModelIndex)));
}

void LayoutDockWidget::openFile(const QString &filePath)
{
    if(controller) delete controller;
    controller = new LayoutController();

    controller->setFilePath(filePath);
    QString error;
    if(!controller->initModel(error)){
        util::showError(this,error);
        return;
    }

    treeView->setModel(controller->model());
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeView->setColumnWidth(0,300);

    treeView->expandToDepth(controller->getExpandDepth());

    // Auto display root node
    showViewNode(controller->rootNode());
}


void LayoutDockWidget::showViewNode(moex::ViewNode *node)
{
    if(!node)
        return;

    qDebug() << QString::fromStdString(node->GetDisplayName());
    WS()->showNode(node);
}

void LayoutDockWidget::clickedTreeNode(QModelIndex index)
{
    QStandardItem *item = controller->model()->itemFromIndex(index);
    if(!item)
        return;

    moex::ViewNode *node = static_cast<moex::ViewNode*>(item->data().value<void*>());
    showViewNode(node);
}
