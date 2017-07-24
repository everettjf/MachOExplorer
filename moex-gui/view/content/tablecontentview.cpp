//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "tablecontentview.h"
#include "../../utility/utility.h"

TableContentView::TableContentView(QWidget *parent) : QWidget(parent)
{
    controller = nullptr;
    tableView = new QTableView(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(tableView);
    setLayout(layout);

}

void TableContentView::showNode(moex::TableViewData *node)
{
    if(controller) delete controller;
    controller = new TableContentController();

    controller->InitModel(node);
    tableView->setModel(controller->model());
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

}
