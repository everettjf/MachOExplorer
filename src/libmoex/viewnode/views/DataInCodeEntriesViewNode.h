//
// Created by everettjf on 2017/7/24.
//

#ifndef MOEX_DATAINCODEENTRIESVIEWNODE_H
#define MOEX_DATAINCODEENTRIESVIEWNODE_H

#include "../ViewNode.h"

MOEX_NAMESPACE_BEGIN

class DataInCodeEntriesViewNode : public ViewNode{
private:
    MachHeaderPtr mh_;
public:
    void Init(MachHeaderPtr mh){mh_ = mh;}
public:
    std::string GetDisplayName()override { return "Data in Code Entries";}
    void InitViewDatas()override;
};
using DataInCodeEntriesViewNodePtr = std::shared_ptr<DataInCodeEntriesViewNode>;

MOEX_NAMESPACE_END

#endif //MOEX_DATAINCODEENTRIESVIEWNODE_H
