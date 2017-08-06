//
// Created by everettjf on 2017/7/24.
//

#include "SymbolTableViewNode.h"

MOEX_NAMESPACE_BEGIN


void SymbolTableViewNode::InitViewDatas() {
    using namespace moex::util;

    auto t = CreateTableViewDataPtr();

    moex::LoadCommand_LC_SYMTAB *seg=nullptr;
    for(auto & cmd : mh_->loadcmds_ref()){
        if(cmd->offset()->cmd == LC_SYMTAB) {
            seg = static_cast<moex::LoadCommand_LC_SYMTAB*>(cmd.get());
            break;
        }
    }
    if(!seg)
        return;

    for(auto & item : seg->nlists_ref()){
        t->AddRow(mh_->GetRAW(& (item->n_strx())),item->n_strx(),"String Table Index",seg->GetStringByStrX(item->n_strx()));

        t->AddRow(mh_->GetRAW(&(item->n_type())),item->n_type(),"Type",AsShortHexString((uint32_t )item->n_type()));
        // foreach

        t->AddRow(mh_->GetRAW(&(item->n_sect())),item->n_sect(),"Section Index",AsShortHexString((uint32_t)item->n_sect()));


        if(item->Is64()){
            t->AddRow(mh_->GetRAW(&(item->n_desc64())),item->n_desc64(),"Description",AsShortHexString(item->n_desc()));
            // foreach
            t->AddRow(mh_->GetRAW(&(item->n_value64())),item->n_value64(),"Value",AsShortHexString(item->n_value64()));
        } else{
            t->AddRow(mh_->GetRAW(&(item->n_desc())),item->n_desc(),"Description",AsShortHexString(item->n_desc()));
            // foreach
            t->AddRow(mh_->GetRAW(&(item->n_value())),item->n_value(),"Value",AsShortHexString(item->n_value()));
        }

        t->AddSeparator();
    }

    AddViewData(t);

    auto b = CreateBinaryViewDataPtr();
    b->offset = (char*)seg->GetSymbolTableOffsetAddress();
    b->size = seg->GetSymbolTableTotalBytes();
    AddViewData(b);
}

MOEX_NAMESPACE_END
