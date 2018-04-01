//
// Created by everettjf on 2017/7/23.
//

#include "MachHeaderViewNode.h"

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

    dynamic_symbol_table_ = std::make_shared<DynamicSymbolTable>();
    dynamic_symbol_table_->Init(d_);
    children_.push_back(dynamic_symbol_table_.get());

    string_table_ = std::make_shared<StringTableViewNode>();
    string_table_->Init(d_);
    children_.push_back(string_table_.get());

    code_signature_ = std::make_shared<CodeSignatureViewNode>();
    code_signature_->Init(d_);
    children_.push_back(code_signature_.get());

    if(d_->ExistLoadCommand({LC_FUNCTION_STARTS})) {
        function_starts_ = std::make_shared<FunctionStartsViewNode>();
        function_starts_->Init(d_);
        children_.push_back(function_starts_.get());
    }
    if(d_->ExistLoadCommand({LC_DATA_IN_CODE})) {
        data_in_code_entries_ = std::make_shared<DataInCodeEntriesViewNode>();
        data_in_code_entries_->Init(d_);
        children_.push_back(data_in_code_entries_.get());
    }

    if(d_->ExistLoadCommand({LC_TWOLEVEL_HINTS})){
        twolevel_hints_table_ = std::make_shared<TwoLevelHintsTableViewNode>();
        twolevel_hints_table_->Init(d_);
        children_.push_back(twolevel_hints_table_.get());
    }
    if(d_->ExistLoadCommand({LC_SEGMENT_SPLIT_INFO})){
        segment_split_info_ = std::make_shared<SegmentSplitInfoViewNode>();
        segment_split_info_->Init(d_);
        children_.push_back(segment_split_info_.get());
    }
}

std::string MachHeaderViewNode::GetDisplayName() {
    std::string arch = d_->GetArch();
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
        auto t = CreateTableView(d_.get());
        const mach_header *m = d_->data_ptr();
        const mach_header *offset = (const mach_header*)d_->header_start();

        t->AddRow(offset->magic, "Magic Number",d_->GetMagicString());

        t->AddRow(offset->cputype,"CPU Type",d_->GetCpuTypeString());

        t->AddRow(offset->cpusubtype,"CPU SubType","");
        for(auto & item : d_->GetCpuSubTypeArray()){
            t->AddRow("",AsHexString((uint)std::get<1>(item)),std::get<2>(item));
        }

        t->AddRow(offset->filetype,"File Type",d_->GetFileTypeString());
        t->AddRow(offset->ncmds,"Number of Load Commands",AsString(m->ncmds));
        t->AddRow(offset->sizeofcmds,"Size of Load Commands",AsString(m->sizeofcmds));

        t->AddRow(offset->flags,"Flags","");
        for(auto & item : d_->GetFlagsArray()){
            t->AddRow("",AsHexString(std::get<0>(item)),std::get<1>(item));
        }

        if(d_->Is64()){
            const mach_header_64 *offset64 = (const mach_header_64*)d_->header_start();

            t->AddRow(offset64->reserved,"Reserved",AsString(d_->mh64()->data_ptr()->reserved));
        }
    }

    // Binary
    {
        auto b = CreateBinaryView();
        b->offset = (char*)d_->header_start();
        b->size = d_->DATA_SIZE();
        b->start_value = (uint64_t)b->offset - (uint64_t)d_->ctx()->file_start;
    }
}

MOEX_NAMESPACE_END
