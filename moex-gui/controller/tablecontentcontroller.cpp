//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "tablecontentcontroller.h"

TableContentController::TableContentController()
{

}

void TableContentController::InitModel()
{
    if(model_) delete model_;

    model_ = new QStandardItemModel();
    model_->setHorizontalHeaderLabels(
                QStringList()
                << QStringLiteral("name")
                << QStringLiteral("name")
                << QStringLiteral("name")
                << QStringLiteral("name")
                );

    QStandardItem * item = new QStandardItem(QStringLiteral("hello"));
    model_->appendRow(item);
    int col = 1;
    model_->setItem(model_->rowCount()-1,col++,new QStandardItem(QStringLiteral("world")));
    model_->setItem(model_->rowCount()-1,col++,new QStandardItem(QStringLiteral("world")));
    model_->setItem(model_->rowCount()-1,col++,new QStandardItem(QStringLiteral("world")));

}
