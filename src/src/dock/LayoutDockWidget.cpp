//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "LayoutDockWidget.h"
#include "src/utility/Utility.h"
#include "src/controller/Workspace.h"
#include "src/controller/LayoutController.h"

#include <QHBoxLayout>
#include <QAbstractItemView>
#include <QItemSelectionModel>

LayoutDockWidget::LayoutDockWidget(QWidget *parent) : QDockWidget(parent)
{
    setWindowTitle(tr("Layout"));

    controller = nullptr;

    treeView = new LayoutTreeView(this);
    treeView->setMinimumWidth(200);
    treeView->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    treeView->setHeaderHidden(true);
    treeView->setFocusPolicy(Qt::StrongFocus);
    treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView->setAllColumnsShowFocus(true);
    treeView->setUniformRowHeights(true);
    setWidget(treeView);

    connect(treeView, &QTreeView::clicked,
            this, &LayoutDockWidget::clickedTreeNode);
    connect(treeView, &QTreeView::activated,
            this, &LayoutDockWidget::clickedTreeNode);
    connect(treeView, &LayoutTreeView::keyboardNavigationActivated,
            this, &LayoutDockWidget::clickedTreeNode);
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

    connect(treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &LayoutDockWidget::currentTreeNodeChanged);

    treeView->expandToDepth(controller->getExpandDepth());

    QModelIndex rootIndex = controller->model()->index(0, 0);
    treeView->setCurrentIndex(rootIndex);
    treeView->scrollTo(rootIndex, QAbstractItemView::PositionAtTop);
    treeView->setFocus(Qt::OtherFocusReason);
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
    showTreeIndex(index);
}

void LayoutDockWidget::showTreeIndex(const QModelIndex &index)
{
    if(!controller || !index.isValid())
        return;

    QStandardItem *item = controller->model()->itemFromIndex(index);
    if(!item)
        return;

    moex::ViewNode *node = static_cast<moex::ViewNode*>(item->data().value<void*>());
    showViewNode(node);
}

void LayoutDockWidget::currentTreeNodeChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    showTreeIndex(current);
}
