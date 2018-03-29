//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "TableInfoWidget.h"
#include "src/utility/Utility.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QHeaderView>

TableInfoWidget::TableInfoWidget(QWidget *parent) : QWidget(parent)
{
    controller = nullptr;
    tableView = new QTableView(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    layout->addWidget(tableView);
    setLayout(layout);

    QHeaderView *verticalHeader = tableView->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(24);
}

void TableInfoWidget::showViewData(moex::TableViewData *data)
{
    moex::TableViewData *node = data;

    if(controller) delete controller;
    controller = new TableInfoController();

    qDebug()<<data;
    qDebug()<<node;
    qDebug()<<controller;


    controller->InitModel(node);

    qDebug()<<"after";

    tableView->setModel(controller->model());
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for(uint32_t idx = 0; idx < node->widths.size(); ++idx){
        tableView->setColumnWidth(idx,node->widths.at(idx));
    }
}
