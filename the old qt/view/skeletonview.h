//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef SKELETONVIEW_H
#define SKELETONVIEW_H

#include <QWidget>
#include <QTreeView>
#include "../controller/skeletoncontroller.h"
#include <QModelIndex>



class SkeletonView : public QWidget
{
    Q_OBJECT
private:
    QTreeView *treeView;
    SkeletonController *controller;

public:
    explicit SkeletonView(QWidget *parent = 0);

    void openFile(const QString & filePath);

signals:

public slots:
    void clickedTreeNode(QModelIndex index);
};

#endif // SKELETONVIEW_H
