//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef BINARYCONTENTCONTROLLER_H
#define BINARYCONTENTCONTROLLER_H


#include "basecontroller.h"
#include <QStandardItemModel>
#include <QAbstractTableModel>

class BinaryContentModel : public QAbstractTableModel{
public:
    explicit BinaryContentModel(QObject *parent=0);

    void InitModel(moex::BinaryViewData *data);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const ;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const ;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

private:
    moex::BinaryViewData *data_;
    std::vector<std::string> headers_;
};

class BinaryContentController
{
public:
    BinaryContentController();


    void InitModel(moex::BinaryViewData *data);

    BinaryContentModel* model(){return model_;}

private:
    BinaryContentModel *model_;
};

#endif // BINARYCONTENTCONTROLLER_H
