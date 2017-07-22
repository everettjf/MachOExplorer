//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_FATBINARYVIEWNODE_H
#define MOEX_FATBINARYVIEWNODE_H


#include "ViewNode.h"
#include "../node/fatheader.h"

MOEX_NAMESPACE_BEGIN

//std::string GetDisplayName()override { return "Fat Binary";}
//BinaryViewData* GetBinaryViewData()override {return nullptr;}
//TableViewData* GetTableViewData()override {return nullptr;}
//void ForEachChild(std::function<void(ViewNode*)> callback)override {}

class FatBinaryViewNode : public ViewNode{
private:
    FatHeaderPtr d;
public:

    std::string GetDisplayName()override { return "Fat Binary";}
    BinaryViewData* GetBinaryViewData()override {return nullptr;}
    TableViewData* GetTableViewData()override {return nullptr;}
    void ForEachChild(std::function<void(ViewNode*)> callback)override {}

};
;
MOEX_NAMESPACE_END



#endif //MOEX_FATBINARYVIEWNODE_H
