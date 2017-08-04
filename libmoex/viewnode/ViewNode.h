//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_VIEWNODE_H
#define MOEX_VIEWNODE_H

#include "../moex.h"
#include <initializer_list>
#include <vector>

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
using ViewDataPtr = std::shared_ptr<ViewData>;

class BinaryViewData: public ViewData{
public:
    char * offset = nullptr;
    uint64_t size = 0;

    BinaryViewData(){
        mode_ = ViewDataMode::Binary;
    }
    bool IsEmpty()const{return offset == nullptr;}
};
using BinaryViewDataPtr = std::shared_ptr<BinaryViewData>;
inline BinaryViewDataPtr CreateBinaryViewDataPtr(){return std::make_shared<BinaryViewData>();}

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
    std::vector<uint32_t> widths;

    TableViewData();

    bool IsEmpty()const{return rows.empty();}

    void SetHeaders(const std::initializer_list<std::string> & vals);
    void SetWidths(const std::initializer_list<uint32_t> & vals);
    void AddRow(const std::initializer_list<std::string> & vals);

    template <typename T>
    void AddRow(uint64_t addr,T data,const std::string & desc,const std::string & val);

    void AddRow(uint64_t addr,const std::string & data,const std::string & desc,const std::string & val);
    void AddRow(uint64_t addr,void* data,size_t size,const std::string & desc,const std::string & val);
    void AddRow(const std::string & addr,const std::string & data,const std::string & desc,const std::string & val);

    void AddSeparator();
};
using TableViewDataPtr = std::shared_ptr<TableViewData>;
inline TableViewDataPtr CreateTableViewDataPtr(){return std::make_shared<TableViewData>();}


template<typename T>
void TableViewData::AddRow(uint64_t addr, T data, const std::string &desc, const std::string &val)
{
    AddRow({util::AsAddress(addr),util::AsHexData(data),desc,val});
}

enum class ViewNodeType{
    Unknown,
    FatHeader,
    MachHeader,
    //...
};

class ViewNode {
protected:
    ViewNodeType type_ = ViewNodeType::Unknown;
    std::vector<ViewDataPtr> view_datas_;
protected:
    void AddViewData(ViewDataPtr val){
        view_datas_.push_back(val);
    }
public:
    ViewNodeType GetDisplayType(){ return type_;}
    virtual ~ViewNode(){}
    std::vector<ViewDataPtr> & GetViewDatas(){
        if(view_datas_.empty()){
            InitViewDatas();
        }
        return view_datas_;
    }

public:
    virtual std::string GetDisplayName(){ return "unknown";}
    virtual void ForEachChild(std::function<void(ViewNode*)>){}
    virtual void InitViewDatas(){}
};


MOEX_NAMESPACE_END

#endif //MOEX_VIEWNODE_H
