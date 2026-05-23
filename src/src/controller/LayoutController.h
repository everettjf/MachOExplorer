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
    ~LayoutController();

    void setFilePath(const QString & filePath){ filePath_ = filePath;}

    // Heavy parsing step. Safe to call off the GUI thread: it only touches
    // libmoex data and never creates Qt UI objects.
    bool parse(QString & error);
    // Builds the tree model from the parsed result. GUI thread only.
    void buildModel();
    void initChildren(moex::ViewNode *parentNode,QStandardItem *parentItem,int depth = 0);
    int getExpandDepth();

    QString lastError() const { return lastError_; }
    void setLastError(const QString & e){ lastError_ = e; }

    QStandardItemModel* model(){return model_;}

    moex::ViewNode *rootNode(){return vnm_.GetRootNode();}

private:
    QStandardItemModel *model_;
    QString filePath_;
    QString lastError_;
    moex::ViewNodeManager vnm_;
};

#endif // LAYOUTCONTROLLER_H
