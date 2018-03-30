//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef TABLECONTENTVIEW_H
#define TABLECONTENTVIEW_H

#include <QWidget>
#include <QTableView>
#include "src/controller/TableInfoController.h"
#include <QModelIndex>

#include <libmoex/moex-view.h>
#include <QWidget>


class TableInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TableInfoWidget(QWidget *parent = 0);

    void showViewData(moex::TableViewData *data);
private:
    QTableView *tableView;
    TableInfoController *controller;
private:
    void clicked(const QModelIndex &index);

signals:

public slots:
};

#endif // TABLECONTENTVIEW_H
