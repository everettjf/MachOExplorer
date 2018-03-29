//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "CentralWidget.h"

#include <QBoxLayout>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "src/controller/Workspace.h"
#include <QDebug>

CentralWidget::CentralWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    this->setLayout(layout);

    table = new TableInfoWidget(this);
    layout->addWidget(table);
}


void CentralWidget::showTableViewData(moex::TableViewData *data)
{
    if(!data)return;
    table->showViewData(data);
}




