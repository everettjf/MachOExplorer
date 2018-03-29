//
// Created by everettjf on 2017/7/24.
//

#ifndef MOEX_STRINGTABLEVIEWNODE_H
#define MOEX_STRINGTABLEVIEWNODE_H


#include "libmoex/viewnode/ViewNode.h"

MOEX_NAMESPACE_BEGIN

class StringTableViewNode : public ViewNode{
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh);
public:
    std::string GetDisplayName()override { return "String Table";}
    void InitViewDatas()override;
};
using StringTableViewNodePtr = std::shared_ptr<StringTableViewNode>;

MOEX_NAMESPACE_END


#endif //MOEX_STRINGTABLEVIEWNODE_H
