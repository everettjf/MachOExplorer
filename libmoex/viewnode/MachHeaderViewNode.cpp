//
// Created by everettjf on 2017/7/23.
//

#include "MachHeaderViewNode.h"
#include <boost/algorithm/string.hpp>

MOEX_NAMESPACE_BEGIN

void MachHeaderViewNode::Init(MachHeaderPtr d){
    d_ = d;

    load_commands_ = std::make_shared<LoadCommandsViewNode>();
    load_commands_->Init(d_);
    children_.push_back(load_commands_.get());

    sections_ = std::make_shared<SectionsViewNode>();
    sections_->Init(d_);
    children_.push_back(sections_.get());

    dynamic_loader_info_ = std::make_shared<DynamicLoaderInfoViewNode>();
    dynamic_loader_info_->Init(d_);
    children_.push_back(dynamic_loader_info_.get());


    symbol_table_ = std::make_shared<SymbolTableViewNode>();
    symbol_table_->Init(d_);
    children_.push_back(symbol_table_.get());


    string_table_ = std::make_shared<StringTableViewNode>();
    string_table_->Init(d_);
    children_.push_back(string_table_.get());

    code_signature_ = std::make_shared<CodeSignatureViewNode>();
    code_signature_->Init(d_);
    children_.push_back(code_signature_.get());

    if(d_->exist_function_starts()){
        function_starts_ = std::make_shared<FunctionStartsViewNode>();
        function_starts_->Init(d_);
        children_.push_back(function_starts_.get());
    }

    if(d_->exist_data_in_code_entries()){
        data_in_code_entries_ = std::make_shared<DataInCodeEntriesViewNode>();
        data_in_code_entries_->Init(d_);
        children_.push_back(data_in_code_entries_.get());
    }
}

std::string MachHeaderViewNode::GetDisplayName() {
    std::string arch = d_->GetArch();
    boost::to_upper(arch);
    return "Mach Header (" + arch +")";
}

void MachHeaderViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    for(auto item : children_){
        callback(item);
    }
}

void MachHeaderViewNode::InitViewDatas(){
    using namespace moex::util;

    // Table
    {
        auto t = CreateTableViewDataPtr();
        const mach_header *m = d_->data_ptr();
        const mach_header *offset = (const mach_header*)d_->header_start();

        t->AddRow(d_->GetRAW(&(offset->magic)),m->magic,"Magic Number",d_->GetMagicString());
        t->AddRow(d_->GetRAW(&(offset->cputype)),m->cputype,"CPU Type",d_->GetCpuTypeString());

        t->AddRow(d_->GetRAW(&(offset->cpusubtype)),m->cpusubtype,"CPU SubType","");
        for(auto & item : d_->GetCpuSubTypeArray()){
            t->AddRow("","",AsShortHexString(std::get<1>(item)),std::get<2>(item));
        }

        t->AddRow(d_->GetRAW(&(offset->filetype)),m->filetype,"File Type",d_->GetFileTypeString());
        t->AddRow(d_->GetRAW(&(offset->ncmds)),m->ncmds,"Number of Load Commands",AsString(m->ncmds));
        t->AddRow(d_->GetRAW(&(offset->sizeofcmds)),m->sizeofcmds,"Size of Load Commands",AsString(m->sizeofcmds));

        t->AddRow(d_->GetRAW(&(offset->flags)),m->flags,"Flags","");
        for(auto & item : d_->GetFlagsArray()){
            t->AddRow("","",AsShortHexString(std::get<0>(item)),std::get<1>(item));
        }

        if(d_->Is64()){
            const mach_header_64 *offset64 = (const mach_header_64*)d_->header_start();

            t->AddRow(d_->GetRAW(&(offset64->reserved)),d_->mh64()->data_ptr()->reserved,"Reserved",AsString(d_->mh64()->data_ptr()->reserved));
        }

        AddViewData(t);
    }

    // Binary
    {
        auto b = CreateBinaryViewDataPtr();
        b->offset = (char*)d_->header_start();
        b->size = d_->DATA_SIZE();
        AddViewData(b);
    }
}

MOEX_NAMESPACE_END
