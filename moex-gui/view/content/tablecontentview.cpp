//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "tablecontentview.h"
#include "../../utility/utility.h"
#include <QHBoxLayout>

TableContentView::TableContentView(QWidget *parent) : ContentViewInterface(parent)
{
    controller = nullptr;
    tableView = new QTableView(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    layout->addWidget(tableView);
    setLayout(layout);

}

void TableContentView::showViewData(moex::ViewData *data)
{
    moex::TableViewData *node = static_cast<moex::TableViewData*>(data);

    if(controller) delete controller;
    controller = new TableContentController();

    controller->InitModel(node);
    tableView->setModel(controller->model());
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for(uint32_t idx = 0; idx < node->widths.size(); ++idx){
        tableView->setColumnWidth(idx,node->widths.at(idx));
    }
    tableView->resizeRowsToContents();
}
