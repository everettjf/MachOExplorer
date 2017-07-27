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
}

void TableViewData::SetHeaders(const std::initializer_list<std::string> & vals){
    headers.clear();
    for(auto & v : vals){
        TableViewHeaderItemPtr h = std::make_shared<TableViewHeaderItem>();
        h->data = v;
        headers.push_back(h);
    }
}
void TableViewData::AddRow(const std::initializer_list<std::string> & vals){
    TableViewRowPtr r = std::make_shared<TableViewRow>();
    r->SetValues(vals);
    rows.push_back(r);
}


MOEX_NAMESPACE_END
