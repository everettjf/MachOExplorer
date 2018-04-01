//
// Created by everettjf on 2017/7/24.
//

#include "SymbolTableViewNode.h"

MOEX_NAMESPACE_BEGIN


void SymbolTableViewNode::InitViewDatas() {
    using namespace moex::util;

    auto t = CreateTableView(mh_.get());
    auto seg= mh_->FindLoadCommand<moex::LoadCommand_LC_SYMTAB>({LC_SYMTAB});
    if(!seg)
        return;

    for(auto & item : seg->nlists_ref()){
        t->AddRow(item->n_strx(),"String Table Index",seg->GetStringByStrX(item->n_strx()));

        t->AddRow(item->n_type(),"Type",AsShortHexString((uint32_t )item->n_type()));
        // foreach

        t->AddRow(item->n_sect(),"Section Index",AsShortHexString((uint32_t)item->n_sect()));


        if(item->Is64()){
            t->AddRow(item->n_desc64(),"Description",AsShortHexString(item->n_desc64()));
            // foreach
            t->AddRow(item->n_value64(),"Value",AsShortHexString(item->n_value64()));
        } else{
            t->AddRow(item->n_desc(),"Description",AsShortHexString(item->n_desc()));
            // foreach
            t->AddRow(item->n_value(),"Value",AsShortHexString(item->n_value()));
        }

        t->AddSeparator();
    }

    auto b = CreateBinaryView();
    b->offset = (char*)seg->GetSymbolTableOffsetAddress();
    b->size = seg->GetSymbolTableTotalBytes();
    b->start_value = (uint64_t)b->offset - (uint64_t)mh_->ctx()->file_start;
}

MOEX_NAMESPACE_END
