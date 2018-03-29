//
// Created by everettjf on 2017/8/2.
//

#ifndef MOEX_FUNCTIONSTARTSVIEWNODE_H
#define MOEX_FUNCTIONSTARTSVIEWNODE_H

#include "../ViewNode.h"

MOEX_NAMESPACE_BEGIN

class FunctionStartsViewNode : public ViewNode{
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh){mh_ = mh;}
public:
    std::string GetDisplayName()override { return "Function Starts";}
    void InitViewDatas()override;
};
using FunctionStartsViewNodePtr = std::shared_ptr<FunctionStartsViewNode>;

MOEX_NAMESPACE_END;


#endif //MOEX_FUNCTIONSTARTSVIEWNODE_H
