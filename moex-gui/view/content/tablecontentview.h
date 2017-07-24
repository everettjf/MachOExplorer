//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef TABLECONTENTVIEW_H
#define TABLECONTENTVIEW_H

#include <QWidget>
#include <libmoex/moex-view.h>
#include <QTableView>
#include "../../controller/tablecontentcontroller.h"
#include <QModelIndex>


class TableContentView : public QWidget
{
    Q_OBJECT
public:
    explicit TableContentView(QWidget *parent = 0);

    void showNode(moex::TableViewData *node);


private:
    QTableView *tableView;
    TableContentController *controller;

signals:

public slots:
};

#endif // TABLECONTENTVIEW_H
