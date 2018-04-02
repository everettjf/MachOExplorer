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
    void *data = nullptr;
    uint64_t size = 0;

    void SetValues(const std::initializer_list<std::string> & vals);
    void SetData(char *data,uint64_t size);
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

    std::function<uint64_t(const void *addr)> GetRAW;

    TableViewData(){
        // default headers and widths
        SetHeaders({"Offset","Description","Value"});
        SetWidths({80,200,200});
    }

    bool IsEmpty()const{return rows.empty();}

    void SetHeaders(const std::initializer_list<std::string> & vals);
    void SetWidths(const std::initializer_list<uint32_t> & vals);


    TableViewRowPtr AddRow(const std::initializer_list<std::string> & vals);
    void AddRow(void *data,uint64_t size,const std::initializer_list<std::string> & vals);

    void AddRow(void* data,uint64_t size,uint64_t addr,const std::string & desc,const std::string & val);
    void AddRow(void* data,uint64_t size,uint64_t addr,const char *desc,const std::string & val);
    void AddRow(const char * addr, const std::string & desc,const std::string & val);
    void AddRow(const char * addr, const char* desc,const std::string & val);

    void AddRow(void* data,uint64_t size,const char *desc,const std::string &val);

    template <typename T>
    void AddRow(T& field,const char *desc,const std::string &val);

    void AddSeparator();

    template <typename A,typename B,typename C,typename D>
    void AddRow(A a,B b,C c,D d);
    template <typename A,typename B,typename C,typename D,typename E>
    void AddRow(A a,B b,C c,D d,E e);

    std::string GetRowDescription(int row);

};
using TableViewDataPtr = std::shared_ptr<TableViewData>;

inline TableViewDataPtr CreateTableViewDataPtr(){
    return std::make_shared<TableViewData>();
}

inline TableViewDataPtr CreateTableViewDataPtr(MachHeaderPtr mh){
    auto ret = std::make_shared<TableViewData>();
    ret->GetRAW = [&mh](const void *addr) -> uint64_t{
        return mh->GetRAW(addr);
    };
    return ret;
}

template <typename T>
void TableViewData::AddRow(T& field,const char *desc,const std::string &val){
    AddRow((void*)&(field),
           (uint64_t)sizeof(field),
           GetRAW(&(field)),
           desc,
           val);
}


template <typename A,typename B,typename C,typename D>
void TableViewData::AddRow(A a,B b,C c,D d){
}

template <typename A,typename B,typename C,typename D,typename E>
void TableViewData::AddRow(A a,B b,C c,D d,E e){
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

    BinaryViewDataPtr CreateBinaryView(){
        binary_ = std::make_shared<BinaryViewData>();
        return binary_;
    }

    TableViewDataPtr CreateTableView(){
        table_ = std::make_shared<TableViewData>();
        return table_;
    }

    TableViewDataPtr CreateTableView(Node * node){
        auto ret = std::make_shared<TableViewData>();
        ret->GetRAW = [node](const void *addr) -> uint64_t{
            return node->GetRAW(addr);
        };
        table_ = ret;
        return ret;
    }
};


MOEX_NAMESPACE_END

#endif //MOEX_VIEWNODE_H
