//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "binarycontentcontroller.h"



BinaryContentModel::BinaryContentModel(QObject *parent)
    :QAbstractTableModel (parent)
{
    headers_ = {"Offset","DATA LOW","DATA HIGH","TEXT"};
}

void BinaryContentModel::InitModel(moex::BinaryViewData *data)
{
    data_ = data;

}

int BinaryContentModel::rowCount(const QModelIndex &parent) const
{
    if(!data_)return 0;

    return 3;// data_->rows.size();
}

int BinaryContentModel::columnCount(const QModelIndex &parent) const
{
    if(!data_)return 0;

    return headers_.size();
}

QVariant BinaryContentModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole){
        int row = index.row();
        int col = index.column();
        std::string val = "";//data_->rows[row]->items[col]->data;
        return QVariant(val.c_str());
    }
    return QVariant();
}

QVariant BinaryContentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        std::string val = headers_[section];
        return QVariant(val.c_str());
    }
    return QVariant();
}


////////////////////////////////////////////////////////////////////////////////


BinaryContentController::BinaryContentController()
{

}
void BinaryContentController::InitModel(moex::BinaryViewData *data)
{
    if(model_) delete model_;

    model_ = new BinaryContentModel();
    model_->InitModel(data);
}
