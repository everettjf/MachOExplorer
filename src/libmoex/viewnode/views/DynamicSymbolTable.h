//
// Created by everettjf on 2017/7/24.
//

#ifndef MOEX_DYNAMICSYMBOLTABLE_H
#define MOEX_DYNAMICSYMBOLTABLE_H

#include "../ViewNode.h"

MOEX_NAMESPACE_BEGIN

class DynamicSymbolTableChild : public ViewNode{
protected:
    MachHeaderPtr mh_;
    std::string name_;
public:
    void Init(MachHeaderPtr mh){mh_ = mh;}
    void set_name(const std::string &name) {name_ = name;}

    std::string GetDisplayName()override { return name_;}
};
using DynamicSymbolTableChildPtr = std::shared_ptr<DynamicSymbolTableChild>;


class DynamicSymbolTable : public ViewNode{
private:
    MachHeaderPtr mh_;
    std::vector<DynamicSymbolTableChildPtr> children_;
public:
    void Init(MachHeaderPtr mh);
public:
    std::string GetDisplayName()override { return "Dynamic Symbol Table";}
    void InitViewDatas()override;
    void ForEachChild(std::function<void(ViewNode*)>)override;
};
using DynamicSymbolTablePtr = std::shared_ptr<DynamicSymbolTable>;

MOEX_NAMESPACE_END

#endif //MOEX_DYNAMICSYMBOLTABLE_H
