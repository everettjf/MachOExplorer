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
    QLineEdit *gotoEdit;
    LayoutFilterProxyModel *proxyModel;
    bool parsing_ = false;
    bool nodeBuilding_ = false;
    moex::ViewNode *pendingNode_ = nullptr;

private:
    void showViewNode(moex::ViewNode * node);
    void showTreeIndex(const QModelIndex &index);
    void populateTree();
    void buildAndShowNode(moex::ViewNode *node);
    void collectMatches(const QModelIndex &proxyParent, QModelIndexList &out) const;
    void goToNextMatch();
    void goToOffset();
    moex::ViewNode *findNodeContainingOffset(moex::ViewNode *node, uint64_t offset, int &budget) const;
    QModelIndex findSourceIndexForNode(const QModelIndex &parent, moex::ViewNode *node) const;
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
