//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_FATHEADERVIEWNODE_H
#define MOEX_FATHEADERVIEWNODE_H


#include "ViewNode.h"
#include "../node/fatheader.h"
#include "MachHeaderViewNode.h"

MOEX_NAMESPACE_BEGIN
class FatHeaderViewNode : public ViewNode{
private:
    FatHeaderPtr d_;
    std::vector<MachHeaderViewNodePtr> headers_;
public:
    FatHeaderViewNode(FatHeaderPtr d);

    std::string GetDisplayName()override { return "Fat Header";}
    void ForEachChild(std::function<void(ViewNode*)> callback)override;
};
using FatHeaderViewNodePtr = std::shared_ptr<FatHeaderViewNode>;

MOEX_NAMESPACE_END

#endif //MOEX_FATHEADERVIEWNODE_H
