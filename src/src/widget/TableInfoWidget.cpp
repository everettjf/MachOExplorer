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

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    tableView = new QTableView(this);
    layout->addWidget(tableView);

    QHeaderView *verticalHeader = tableView->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(24);

    tableView->setSelectionBehavior(QTableView::SelectRows);
    tableView->setSelectionMode(QTableView::SingleSelection);
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
