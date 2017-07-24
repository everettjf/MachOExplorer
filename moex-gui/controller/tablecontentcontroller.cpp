//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "tablecontentcontroller.h"


TableContentModel::TableContentModel(QObject *parent)
    :QAbstractTableModel (parent)
{

}

int TableContentModel::rowCount(const QModelIndex &parent) const
{
    return 3;
}

int TableContentModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

QVariant TableContentModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole){
        return QVariant::fromValue(1);
    }
    return QVariant();
}

QVariant TableContentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return QVariant::fromValue(2);
    return QVariant();
}


////////////////////////////////////////////////////////////////////////////////

TableContentController::TableContentController()
{

}

void TableContentController::InitModel()
{
    if(model_) delete model_;

    model_ = new TableContentModel();

}
