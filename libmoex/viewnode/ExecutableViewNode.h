//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_EXECUTABLEVIEWNODE_H
#define MOEX_EXECUTABLEVIEWNODE_H

#include "ViewNode.h"
#include "../node/fatheader.h"

MOEX_NAMESPACE_BEGIN

class ExecutableViewNode : public ViewNode{
private:
    MachHeaderPtr d_;
public:
    ExecutableViewNode(MachHeaderPtr d):d_(d){}

    std::string GetDisplayName()override { return "Executable";}
    BinaryViewData* GetBinaryViewData()override {return nullptr;}
    TableViewData* GetTableViewData()override {return nullptr;}
    void ForEachChild(std::function<void(ViewNode*)> callback)override {}
};
using ExecutableViewNodePtr = std::shared_ptr<ExecutableViewNode>;

MOEX_NAMESPACE_END

#endif //MOEX_EXECUTABLEVIEWNODE_H
