//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "skeletoncontroller.h"
#include <QDebug>

SkeletonController::SkeletonController()
{
    model_ = nullptr;
}

bool SkeletonController::initModel(QString & error)
{
    if(filePath_.length() == 0){
        error = "File path is empty";
        return false;
    }

    log("Start parsing " + filePath_);

    // Init model
    if(model_) delete model_;
    model_ = new QStandardItemModel();
    model_->setHorizontalHeaderLabels(
                QStringList()
                << QStringLiteral("name")
                << QStringLiteral("type")
                );

    // Parse file
    std::string filepath = filePath_.toStdString();
    std::string init_error;
    if(!vnm_.Init(filepath,init_error)){
        error = QString("Exception : %1").arg(init_error.c_str());
        log(error);
        return false;
    }
    log("Parse succeed");

    // Root item
    moex::ViewNode *root = vnm_.GetRootNode();
    QStandardItem *item = new QStandardItem(QString::fromStdString(root->GetDisplayName()));
    model_->appendRow(item);
    item->setData(QVariant::fromValue((void*)root));

//    // Children
//    initChildren(rootNode,item);



    return true;
}

//void SkeletonController::initChildren(moex::Node *parentNode,QStandardItem *parentItem){
//    parentNode->ForEachChild([&](moex::Node* node){
//        QStandardItem *subItem = new QStandardItem(QString::fromStdString(node->GetDisplayName()));
//        parentItem->appendRow(subItem);
//        parentItem->setChild(subItem->index().row(),1,new QStandardItem(util::qstr(node->GetTypeName())));
//        subItem->setData(QVariant::fromValue((void*)node));

//        // Loop
//        initChildren(node,subItem);
//    });
//}
