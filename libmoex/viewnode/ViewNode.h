//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_VIEWNODE_H
#define MOEX_VIEWNODE_H

#include "../moex.h"
#include <initializer_list>

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

    BinaryViewData(){
        mode_ = ViewDataMode::Binary;
    }
};
using BinaryViewDataPtr = std::shared_ptr<BinaryViewData>;

class TableViewItem{
public:
    std::string data;
};
using TableViewItemPtr = std::shared_ptr<TableViewItem>;

class TableViewRow{
public:
    std::vector<TableViewItemPtr> items;

    void SetValues(const std::initializer_list<std::string> & vals);
};
using TableViewRowPtr = std::shared_ptr<TableViewRow>;

class TableViewHeaderItem{
public:
    std::string data;
};
using TableViewHeaderItemPtr = std::shared_ptr<TableViewHeaderItem>;

class TableViewData : public ViewData{
public:
    std::vector<TableViewHeaderItemPtr> headers;
    std::vector<TableViewRowPtr> rows;

    TableViewData();

    void SetHeaders(const std::initializer_list<std::string> & vals);
    void AddRow(const std::initializer_list<std::string> & vals);

    template <typename T>
    void AddRow(uint64_t addr,T data,const std::string & desc,const std::string & val);
};
using TableViewDataPtr = std::shared_ptr<TableViewData>;

enum class ViewNodeType{
    Unknown,
    FatHeader,
    MachHeader,
    //...
};

class ViewNode {
protected:
    ViewNodeType type_ = ViewNodeType::Unknown;
public:
    ViewNodeType GetDisplayType(){ return type_;}

public:
    virtual std::string GetDisplayName(){ return "unknown";}
    virtual void ForEachChild(std::function<void(ViewNode*)> callback){}
    virtual std::vector<ViewData*> GetViewDatas(){ return {};}
};


MOEX_NAMESPACE_END

#endif //MOEX_VIEWNODE_H
