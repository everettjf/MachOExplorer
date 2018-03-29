//
// Created by everettjf on 2017/7/24.
//

#ifndef MOEX_SYMBOLTABLEVIEWNODE_H
#define MOEX_SYMBOLTABLEVIEWNODE_H

#include "libmoex/viewnode/ViewNode.h"

MOEX_NAMESPACE_BEGIN

class SymbolTableViewNode : public ViewNode{
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh){mh_ = mh;}
public:
    std::string GetDisplayName()override { return "Symbol Table";}
    void InitViewDatas()override;
};
using SymbolTableViewNodePtr = std::shared_ptr<SymbolTableViewNode>;

MOEX_NAMESPACE_END


#endif //MOEX_SYMBOLTABLEVIEWNODE_H
