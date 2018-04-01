//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef TABLECONTENTCONTROLLER_H
#define TABLECONTENTCONTROLLER_H
#include <libmoex/moex.h>
#include <libmoex/moex-view.h>
#include "Stable.h"
#include <QStandardItemModel>
#include <QAbstractTableModel>

class TableInfoModel : public QAbstractTableModel{
public:
    explicit TableInfoModel(QObject *parent=0);

    moex::TableViewData *data_ptr(){return data_;}


    void InitModel(moex::TableViewData *data);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const ;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const ;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

private:
    moex::TableViewData *data_ = nullptr;
};



class TableInfoController
{
public:
    TableInfoController();

    void InitModel(moex::TableViewData *data);

    TableInfoModel* model(){return model_;}

private:
    TableInfoModel *model_ = nullptr;
};

#endif // TABLECONTENTCONTROLLER_H
