//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_VIEWNODE_H
#define MOEX_VIEWNODE_H

#include "../moex.h"
#include <initializer_list>
#include <vector>

MOEX_NAMESPACE_BEGIN

///////////////////////////////// Binary ////////////////////////////
class BinaryViewData{
public:
    char * offset = nullptr;
    uint64_t size = 0;
    uint64_t start_value = 0;

    BinaryViewData() = default;
    bool IsEmpty()const{return offset == nullptr;}
};
using BinaryViewDataPtr = std::shared_ptr<BinaryViewData>;
inline BinaryViewDataPtr CreateBinaryViewDataPtr(){return std::make_shared<BinaryViewData>();}

///////////////////////////////// Table ////////////////////////////
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

class TableViewData{
public:
    std::vector<TableViewHeaderItemPtr> headers;
    std::vector<TableViewRowPtr> rows;
    std::vector<uint32_t> widths;

    TableViewData(){
        SetHeaders({"Offset","Size","Description","Value"});
        SetWidths({80,80,200,200});
    }

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


///////////////////////////////// View Node Base Class ///////////////////////////////


class ViewNode {
protected:
    BinaryViewDataPtr binary_;
    TableViewDataPtr table_;
    bool inited_ = false;
protected:
    // Lazy load
    virtual void InitViewDatas(){ }

public:
    virtual ~ViewNode(){ }

    // Name for the node
    virtual std::string GetDisplayName(){ return "null"; }

    // Children for the node
    virtual void ForEachChild(std::function<void(ViewNode*)>){ }

    // Related content
    BinaryViewDataPtr & binary(){return binary_;};
    TableViewDataPtr & table(){return table_;};

    void SetViewData(BinaryViewDataPtr data){binary_ = data;}
    void SetViewData(TableViewDataPtr data){table_ = data;}

    void Init(){
        if(!inited_){
            inited_ = true;
            InitViewDatas();
        }
    }
};


MOEX_NAMESPACE_END

#endif //MOEX_VIEWNODE_H
