//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef SKELETONCONTROLLER_H
#define SKELETONCONTROLLER_H

#include <QStandardItemModel>
#include <libmoex/binary.h>
#include "../utility/utility.h"
#include "basecontroller.h"

class SkeletonController : public BaseController
{
public:
    SkeletonController();

    void setFilePath(const QString & filePath){ filePath_ = filePath;}

    bool initModel(QString & error);
    void initChildren(moex::Node *parentNode,QStandardItem *parentItem);

    QStandardItemModel* model(){return model_;}

private:
    QStandardItemModel *model_;
    QString filePath_;
    moex::BinaryPtr bin_;
};

#endif // SKELETONCONTROLLER_H
