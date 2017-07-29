//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef SKELETONCONTROLLER_H
#define SKELETONCONTROLLER_H

#include <QStandardItemModel>
#include <libmoex/viewnode/ViewNodeManager.h>
#include "../utility/utility.h"
#include "basecontroller.h"

class SkeletonController : public BaseController
{
public:
    SkeletonController();

    void setFilePath(const QString & filePath){ filePath_ = filePath;}

    bool initModel(QString & error);
    void initChildren(moex::ViewNode *parentNode,QStandardItem *parentItem);
    int getExpandDepth();

    QStandardItemModel* model(){return model_;}

private:
    QStandardItemModel *model_;
    QString filePath_;
    moex::ViewNodeManager vnm_;
};

#endif // SKELETONCONTROLLER_H
