//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef LAYOUTVIEW_H
#define LAYOUTVIEW_H
#include "Stable.h"

#include <QDockWidget>
#include <QTreeView>
#include <QModelIndex>
#include "../widget/LayoutTreeView.h"

class LayoutController;
class LayoutFilterProxyModel;
class QLineEdit;


class LayoutDockWidget : public QDockWidget
{
    Q_OBJECT
private:
    LayoutTreeView *treeView;
    LayoutController *controller;
    QLineEdit *searchEdit;
    LayoutFilterProxyModel *proxyModel;
    bool parsing_ = false;
    bool nodeBuilding_ = false;
    moex::ViewNode *pendingNode_ = nullptr;

private:
    void showViewNode(moex::ViewNode * node);
    void showTreeIndex(const QModelIndex &index);
    void populateTree();
    void buildAndShowNode(moex::ViewNode *node);
public:
    explicit LayoutDockWidget(QWidget *parent = 0);

    void openFile(const QString & filePath);

signals:

public slots:
    void clickedTreeNode(QModelIndex index);
    void currentTreeNodeChanged(const QModelIndex &current, const QModelIndex &previous);
    void onSearchTextChanged(const QString &text);
};

#endif // LAYOUTVIEW_H
