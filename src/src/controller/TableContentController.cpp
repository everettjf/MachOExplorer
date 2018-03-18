//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "TableContentController.h"


TableContentModel::TableContentModel(QObject *parent)
    :QAbstractTableModel (parent)
{

}

void TableContentModel::InitModel(moex::TableViewData *data)
{
    data_ = data;

}

int TableContentModel::rowCount(const QModelIndex &parent) const
{
    if(!data_)return 0;

    return data_->rows.size();
}

int TableContentModel::columnCount(const QModelIndex &parent) const
{
    if(!data_)return 0;

    return data_->headers.size();
}

QVariant TableContentModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole){
        int row = index.row();
        int col = index.column();
        std::string val = data_->rows[row]->items[col]->data;
        return QVariant(val.c_str());
    }
    return QVariant();
}

QVariant TableContentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        std::string val = data_->headers[section]->data;
        return QVariant(val.c_str());
    }
    return QVariant();
}


////////////////////////////////////////////////////////////////////////////////

TableContentController::TableContentController()
{

}

void TableContentController::InitModel(moex::TableViewData *data)
{
    if(model_) delete model_;

    model_ = new TableContentModel();
    model_->InitModel(data);
}
