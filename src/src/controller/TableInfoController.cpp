//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "TableInfoController.h"


TableInfoModel::TableInfoModel(QObject *parent)
    :QAbstractTableModel (parent)
{

}

void TableInfoModel::InitModel(moex::TableViewData *data)
{
    data_ = data;

}

int TableInfoModel::rowCount(const QModelIndex &parent) const
{
    if(!data_)return 0;

    return data_->rows.size();
}

int TableInfoModel::columnCount(const QModelIndex &parent) const
{
    if(!data_)return 0;

    return data_->headers.size();
}

QVariant TableInfoModel::data(const QModelIndex &index, int role) const
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

QVariant TableInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        std::string val = data_->headers[section]->data;
        return QVariant(val.c_str());
    }
    return QVariant();
}


////////////////////////////////////////////////////////////////////////////////

TableInfoController::TableInfoController()
{

}

void TableInfoController::InitModel(moex::TableViewData *data)
{
    if(model_) delete model_;

    model_ = new TableInfoModel();
    model_->InitModel(data);
}
