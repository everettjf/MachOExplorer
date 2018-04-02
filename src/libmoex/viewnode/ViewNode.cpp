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

void TableViewRow::SetData(char *data,uint64_t size){
    this->data = data;
    this->size = size;
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
void TableViewData::AddRow(void *data,uint64_t size,const std::initializer_list<std::string> & vals){
    TableViewRowPtr r = std::make_shared<TableViewRow>();
    r->SetValues(vals);
    r->data = data;
    r->size = size;
    rows.push_back(r);
}

TableViewRowPtr TableViewData::AddRow(const std::initializer_list<std::string> & vals){
    TableViewRowPtr r = std::make_shared<TableViewRow>();
    r->SetValues(vals);
    rows.push_back(r);
    return r;
}
void TableViewData::AddRow(void* data,uint64_t size,uint64_t addr,const std::string & desc,const std::string & val){
    AddRow(data,size,{util::AsAddress(addr),desc,val});
}

void TableViewData::AddRow(void* data,uint64_t size,uint64_t addr,const char *desc,const std::string & val){
    AddRow(data,size,{util::AsAddress(addr),desc,val});
}
void TableViewData::AddRow(const char * addr, const std::string & desc,const std::string & val){
    AddRow({addr,desc,val});
}
void TableViewData::AddRow(const char * addr, const char * desc,const std::string & val){
    AddRow({addr,desc,val});
}
void TableViewData::AddRow(void * data,uint64_t size,const char *desc,const std::string &val){
    AddRow(data,
           size,
           GetRAW(data),
           desc,
           val);
}


void TableViewData::AddSeparator()
{
    AddRow({"","","",""});
}

std::string TableViewData::GetRowDescription(int row)
{
    if(row >= rows.size())
        return "";

    auto data = rows[row];

    std::string ret;
    for(int idx = 0; idx < headers.size(); ++idx){

        std::string val;
        if(idx < data->items.size()){
            val = data->items[idx]->data;
        }

        ret += fmt::format("{} : {}\n",headers[idx]->data,val);
    }

    if(data->size > 0){
        ret += fmt::format("Length : {}\n", data->size);
        ret += fmt::format("Data : {}\n", util::AsHexData(data->data,(size_t)data->size));
    }

    return ret;
}



MOEX_NAMESPACE_END
