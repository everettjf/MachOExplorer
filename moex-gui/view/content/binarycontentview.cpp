//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "binarycontentview.h"


#include "../../utility/utility.h"
#include <QHBoxLayout>

BinaryContentView::BinaryContentView(QWidget *parent) : QWidget(parent)
{
    controller = nullptr;
    tableView = new QTableView(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    layout->addWidget(tableView);
    setLayout(layout);

}


void BinaryContentView::showNode(moex::BinaryViewData *node)
{
    if(controller) delete controller;
    controller = new BinaryContentController();

    controller->InitModel(node);
    tableView->setModel(controller->model());
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableView->resizeRowsToContents();
}

