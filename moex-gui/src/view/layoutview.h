//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef LAYOUTVIEW_H
#define LAYOUTVIEW_H

#include <QWidget>
#include <QTreeView>
#include "../controller/layoutcontroller.h"
#include <QModelIndex>



class LayoutView : public QWidget
{
    Q_OBJECT
private:
    QTreeView *treeView;
    LayoutController *controller;
private:
    void showViewNode(moex::ViewNode * node);
public:
    explicit LayoutView(QWidget *parent = 0);

    void openFile(const QString & filePath);

signals:

public slots:
    void clickedTreeNode(QModelIndex index);
};

#endif // LAYOUTVIEW_H
