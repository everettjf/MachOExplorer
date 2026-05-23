//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "LayoutController.h"
#include <QDebug>

LayoutController::LayoutController()
{
    model_ = nullptr;
}

LayoutController::~LayoutController()
{
    if (model_) {
        delete model_;
        model_ = nullptr;
    }
}

bool LayoutController::parse(QString & error)
{
    if(filePath_.length() == 0){
        error = "File path is empty";
        return false;
    }

    std::string filepath = filePath_.toStdString();
    std::string init_error;
    if(!vnm_.Init(filepath,init_error)){
        error = QString("Exception : %1").arg(init_error.c_str());
        return false;
    }
    return true;
}

void LayoutController::buildModel()
{
    // Init model
    if(model_) delete model_;
    model_ = new QStandardItemModel();
    model_->setHorizontalHeaderLabels(
                QStringList()
                << QStringLiteral("name")
                );

    // Root item
    moex::ViewNode *root = vnm_.GetRootNode();
    QStandardItem *item = new QStandardItem(QString::fromStdString(root->GetDisplayName()));
    model_->appendRow(item);
    item->setData(QVariant::fromValue((void*)root));

    // Children
    initChildren(root,item);
}

void LayoutController::initChildren(moex::ViewNode *parentNode,QStandardItem *parentItem,int depth){
    // Guard against a pathologically deep / cyclic node tree from malformed input.
    if(depth >= 256)
        return;
    parentNode->ForEachChild([&](moex::ViewNode* node){
        QStandardItem *subItem = new QStandardItem(QString::fromStdString(node->GetDisplayName()));
        parentItem->appendRow(subItem);
        subItem->setData(QVariant::fromValue((void*)node));

        // Loop
        initChildren(node,subItem,depth + 1);
    });
}

int LayoutController::getExpandDepth()
{
    return vnm_.IsFat()?2:1;
}
