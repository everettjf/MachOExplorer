//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_VIEWNODE_H
#define MOEX_VIEWNODE_H

#include "../moex.h"

MOEX_NAMESPACE_BEGIN


struct BinaryViewData{
    char * offset;
    uint64_t size;
};

struct TableViewData{
    std::vector<std::vector<std::string>> rows;
};

enum class ViewNodeType{
    Unknown,
    FatHeader,
    MachHeader,
};

class ViewNode {
protected:
    ViewNodeType type_ = ViewNodeType::Unknown;
public:
    ViewNodeType GetDisplayType(){ return type_;}

public:
    virtual std::string GetDisplayName(){ return "unknown";}
    virtual BinaryViewData* GetBinaryViewData(){return nullptr;}
    virtual TableViewData* GetTableViewData(){return nullptr;}
    virtual void ForEachChild(std::function<void(ViewNode*)> callback){}
};


MOEX_NAMESPACE_END

#endif //MOEX_VIEWNODE_H
