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

class InfoWidgetBase : public QWidget{
Q_OBJECT
public:
    explicit InfoWidgetBase(QWidget *parent = 0):QWidget(parent){}
    virtual void showViewData(moex::ViewData *data) = 0;
};


class TableInfoWidget : public InfoWidgetBase
{
    Q_OBJECT
public:
    explicit TableInfoWidget(QWidget *parent = 0);

    void showViewData(moex::ViewData *data)override;
private:
    QTableView *tableView;
    TableInfoController *controller;

signals:

public slots:
};

#endif // TABLECONTENTVIEW_H
