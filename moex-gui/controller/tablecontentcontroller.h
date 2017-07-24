//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef TABLECONTENTCONTROLLER_H
#define TABLECONTENTCONTROLLER_H

#include "basecontroller.h"
#include <QStandardItemModel>
#include <QAbstractTableModel>

class TableContentModel : public QAbstractTableModel{
public:
    explicit TableContentModel(QObject *parent=0);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const ;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const ;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;


};



class TableContentController : public BaseController
{
public:
    TableContentController();

    void InitModel();

    TableContentModel* model(){return model_;}

private:
    TableContentModel *model_;
};

#endif // TABLECONTENTCONTROLLER_H
