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


class LayoutDockWidget : public QDockWidget
{
    Q_OBJECT
private:
    LayoutTreeView *treeView;
    LayoutController *controller;

private:
    void showViewNode(moex::ViewNode * node);
public:
    explicit LayoutDockWidget(QWidget *parent = 0);

    void openFile(const QString & filePath);

signals:

public slots:
    void clickedTreeNode(QModelIndex index);
};

#endif // LAYOUTVIEW_H
