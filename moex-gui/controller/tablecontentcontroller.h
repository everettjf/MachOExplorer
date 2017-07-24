//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef TABLECONTENTCONTROLLER_H
#define TABLECONTENTCONTROLLER_H

#include "basecontroller.h"
#include <QStandardItemModel>


class TableContentController : public BaseController
{
public:
    TableContentController();

    void InitModel();

    QStandardItemModel* model(){return model_;}

private:
    QStandardItemModel *model_;
};

#endif // TABLECONTENTCONTROLLER_H
