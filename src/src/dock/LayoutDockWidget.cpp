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
#include <QtConcurrent>
#include <QFutureWatcher>

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
    if(parsing_){
        WS()->addLog("Still parsing the previous file; please wait...");
        return;
    }

    if(controller) delete controller;
    controller = new LayoutController();
    controller->setFilePath(filePath);

    // Clear the current tree while the new file parses in the background so the
    // UI thread stays responsive on large binaries / dyld shared caches.
    treeView->setModel(nullptr);
    parsing_ = true;
    WS()->addLog("Start parsing " + filePath);

    LayoutController *ctrl = controller;
    auto *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher, ctrl](){
        watcher->deleteLater();
        parsing_ = false;

        // Bail if the controller was replaced while we were parsing.
        if(ctrl != controller){
            return;
        }

        if(!watcher->result()){
            util::showError(this, controller->lastError());
            WS()->addLog(controller->lastError());
            return;
        }

        WS()->addLog("Parse succeed");
        controller->buildModel();
        populateTree();
    });

    watcher->setFuture(QtConcurrent::run([ctrl]() -> bool {
        QString error;
        bool ok = ctrl->parse(error);
        ctrl->setLastError(error);
        return ok;
    }));
}

void LayoutDockWidget::populateTree()
{
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

    // Record the latest selection so it always wins over in-flight builds.
    pendingNode_ = node;

    // A build is already running; its completion handler will pick up the
    // latest pendingNode_.
    if(nodeBuilding_)
        return;

    // Already parsed: display immediately on the GUI thread.
    if(node->inited()){
        WS()->displayNode(node);
        return;
    }

    buildAndShowNode(node);
}

void LayoutDockWidget::buildAndShowNode(moex::ViewNode *node)
{
    nodeBuilding_ = true;
    auto *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher](){
        watcher->deleteLater();
        nodeBuilding_ = false;

        moex::ViewNode *latest = pendingNode_;
        if(latest == nullptr)
            return;

        // If the latest selection still needs parsing, build it; otherwise it
        // is ready to display now.
        if(!latest->inited()){
            buildAndShowNode(latest);
        } else {
            WS()->displayNode(latest);
        }
    });
    watcher->setFuture(QtConcurrent::run([node](){ node->Init(); }));
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
