//
// Created by everettjf on 2017/7/23.
//

#include "ViewNode.h"

MOEX_NAMESPACE_BEGIN

void TableViewRow::SetValues(const std::initializer_list<std::string> & vals){
    items.clear();
    for(auto & v : vals){
        TableViewItemPtr h = std::make_shared<TableViewItem>();
        h->data = v;
        items.push_back(h);
    }
}


TableViewData::TableViewData(){
    mode_ = ViewDataMode::Table;
    SetHeaders({"Offset","Data","Description","Value"});
    SetWidths({80,200,180,170});
}

void TableViewData::SetHeaders(const std::initializer_list<std::string> & vals){
    headers.clear();
    for(auto & v : vals){
        TableViewHeaderItemPtr h = std::make_shared<TableViewHeaderItem>();
        h->data = v;
        headers.push_back(h);
    }
}

void TableViewData::SetWidths(const std::initializer_list<uint32_t> &vals)
{
    widths.clear();
    for(auto & v:vals){
        widths.push_back(v);
    }
}
void TableViewData::AddRow(const std::initializer_list<std::string> & vals){
    TableViewRowPtr r = std::make_shared<TableViewRow>();
    r->SetValues(vals);
    rows.push_back(r);
}

void TableViewData::AddRow(uint64_t addr, void *data, size_t size, const std::string &desc, const std::string &val)
{
    AddRow({util::AsAddress(addr),util::AsHexData(data,size),desc,val});
}

void TableViewData::AddRow(uint64_t addr,const std::string & data,const std::string & desc,const std::string & val){
    AddRow({util::AsAddress(addr),data,desc,val});
}


void TableViewData::AddRow(const std::string & addr,const std::string & data,const std::string & desc,const std::string & val){
    AddRow({addr,data,desc,val});
}

void TableViewData::AddSeparator()
{
    AddRow({"","","",""});
}



MOEX_NAMESPACE_END
