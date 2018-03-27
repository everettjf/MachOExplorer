//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef LAYOUTVIEW_H
#define LAYOUTVIEW_H

#include <QDockWidget>
#include <QTreeView>
#include "src/controller/LayoutController.h"
#include <QModelIndex>



class LayoutDockWidget : public QDockWidget
{
    Q_OBJECT
private:
    QTreeView *treeView;
    LayoutController *controller;

private:
    void showViewNode(moex::ViewNode * node);
public:
    explicit LayoutDockWidget(QWidget *parent = 0);

    void openFile(const QString & filePath);

    QSize sizeHint() const;
signals:

public slots:
    void clickedTreeNode(QModelIndex index);
};

#endif // LAYOUTVIEW_H
