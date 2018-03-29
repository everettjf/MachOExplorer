//
// Created by everettjf on 2017/7/24.
//

#ifndef MOEX_DYNAMICSYMBOLTABLE_H
#define MOEX_DYNAMICSYMBOLTABLE_H

#include "../ViewNode.h"

MOEX_NAMESPACE_BEGIN

class DynamicSymbolTable : public ViewNode{
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh);
public:
    std::string GetDisplayName()override { return "Dynamic Symbol Table";}
    void InitViewDatas()override;

};
using DynamicSymbolTablePtr = std::shared_ptr<DynamicSymbolTable>;

MOEX_NAMESPACE_END

#endif //MOEX_DYNAMICSYMBOLTABLE_H
