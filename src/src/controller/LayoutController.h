//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef LAYOUTCONTROLLER_H
#define LAYOUTCONTROLLER_H
#include "Stable.h"
#include <QStandardItemModel>
#include <libmoex/viewnode/ViewNodeManager.h>
#include "src/utility/Utility.h"
#include "Workspace.h"

class LayoutController
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
