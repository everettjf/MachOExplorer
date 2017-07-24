//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_VIEWNODE_H
#define MOEX_VIEWNODE_H

#include "../moex.h"

MOEX_NAMESPACE_BEGIN

enum class ViewDataMode{
    Binary,
    Table,
};
class ViewData{
protected:
    ViewDataMode mode_;
public:
    ViewDataMode mode()const{return mode_;}

};

class BinaryViewData: public ViewData{
public:
    char * offset;
    uint64_t size;
};

class TableViewItem{
public:
    std::string data;
};
using TableViewItemPtr = std::shared_ptr<TableViewItem>;

class TableViewRow{
public:
    std::vector<TableViewItemPtr> items;
};
using TableViewRowPtr = std::shared_ptr<TableViewRow>;

class TableViewData : public ViewData{
public:
    std::vector<TableViewRowPtr> rows;
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
    virtual std::vector<ViewData*> GetViewDatas(){ return {};}
    virtual void ForEachChild(std::function<void(ViewNode*)> callback){}
};


MOEX_NAMESPACE_END

#endif //MOEX_VIEWNODE_H
