//
// Created by everettjf on 2017/7/24.
//

#ifndef MOEX_SYMBOLTABLEVIEWNODE_H
#define MOEX_SYMBOLTABLEVIEWNODE_H

#include "ViewNode.h"

MOEX_NAMESPACE_BEGIN

class SymbolTableViewNode : public ViewNode{
public:
    std::string GetDisplayName()override { return "Symbol Table";}
    void InitViewDatas()override;
};
using SymbolTableViewNodePtr = std::shared_ptr<SymbolTableViewNode>;

MOEX_NAMESPACE_END


#endif //MOEX_SYMBOLTABLEVIEWNODE_H
