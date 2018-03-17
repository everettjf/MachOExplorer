//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef LAYOUTCONTROLLER_H
#define LAYOUTCONTROLLER_H

#include <QStandardItemModel>
#include <libmoex/viewnode/ViewNodeManager.h>
#include "../utility/utility.h"
#include "basecontroller.h"

class LayoutController : public BaseController
{
public:
    LayoutController();

    void setFilePath(const QString & filePath){ filePath_ = filePath;}

    bool initModel(QString & error);
    void initChildren(moex::ViewNode *parentNode,QStandardItem *parentItem);
    int getExpandDepth();

    QStandardItemModel* model(){return model_;}

    moex::ViewNode *rootNode(){return vnm_.GetRootNode();}

private:
    QStandardItemModel *model_;
    QString filePath_;
    moex::ViewNodeManager vnm_;
};

#endif // LAYOUTCONTROLLER_H
