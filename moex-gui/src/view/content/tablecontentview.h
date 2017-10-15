//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef TABLECONTENTVIEW_H
#define TABLECONTENTVIEW_H

#include <QWidget>
#include "contentviewinterface.h"
#include <QTableView>
#include "../../controller/tablecontentcontroller.h"
#include <QModelIndex>


class TableContentView : public ContentViewInterface
{
    Q_OBJECT
public:
    explicit TableContentView(QWidget *parent = 0);

    void showViewData(moex::ViewData *data)override;
private:
    QTableView *tableView;
    TableContentController *controller;

signals:

public slots:
};

#endif // TABLECONTENTVIEW_H
