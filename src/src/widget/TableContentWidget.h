//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef TABLECONTENTVIEW_H
#define TABLECONTENTVIEW_H

#include <QWidget>
#include <QTableView>
#include "src/controller/TableContentController.h"
#include <QModelIndex>

#include <libmoex/moex-view.h>
#include <QWidget>

class ContentWidgetBase : public QWidget{
Q_OBJECT
public:
    explicit ContentWidgetBase(QWidget *parent = 0):QWidget(parent){}
    virtual void showViewData(moex::ViewData *data) = 0;
};


class TableContentWidget : public ContentWidgetBase
{
    Q_OBJECT
public:
    explicit TableContentWidget(QWidget *parent = 0);

    void showViewData(moex::ViewData *data)override;
private:
    QTableView *tableView;
    TableContentController *controller;

signals:

public slots:
};

#endif // TABLECONTENTVIEW_H
