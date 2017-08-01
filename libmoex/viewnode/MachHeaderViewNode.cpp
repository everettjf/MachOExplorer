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

    sections_ = std::make_shared<SectionsViewNode>();
    sections_->Init(d_);
}

std::string MachHeaderViewNode::GetDisplayName() {
    std::string arch = d_->GetArch();
    boost::to_upper(arch);
    return "Mach Header (" + arch +")";
}

void MachHeaderViewNode::ForEachChild(std::function<void(ViewNode*)> callback){
    callback(load_commands_.get());

    callback(sections_.get());
}

void MachHeaderViewNode::InitViewDatas(){
    using namespace moex::util;

    // Table
    {
        TableViewDataPtr t = std::make_shared<TableViewData>();
        const mach_header *m = d_->data_ptr();

        t->AddRow(d_->GetRAW(&(m->magic)),m->magic,"Magic Number",d_->GetMagicString());
        t->AddRow(d_->GetRAW(&(m->cputype)),m->cputype,"CPU Type",d_->GetCpuTypeString());

        t->AddRow(d_->GetRAW(&(m->cpusubtype)),m->cpusubtype,"CPU SubType","");
        for(auto & item : d_->GetCpuSubTypeArray()){
            t->AddRow("","",AsHexString(std::get<1>(item)),std::get<2>(item));
        }

        t->AddRow(d_->GetRAW(&(m->filetype)),m->filetype,"File Type",d_->GetFileTypeString());
        t->AddRow(d_->GetRAW(&(m->ncmds)),m->ncmds,"Number of Load Commands",AsString(m->ncmds));
        t->AddRow(d_->GetRAW(&(m->sizeofcmds)),m->sizeofcmds,"Size of Load Commands",AsString(m->sizeofcmds));

        t->AddRow(d_->GetRAW(&(m->flags)),m->flags,"Flags","");
        for(auto & item : d_->GetFlagsArray()){
            t->AddRow("","",AsHexString(std::get<0>(item)),std::get<1>(item));
        }

        if(d_->is64()){
            t->AddRow(d_->GetRAW(&(d_->mh64()->data_ptr()->reserved)),d_->mh64()->data_ptr()->reserved,"Reserved",AsString(d_->mh64()->data_ptr()->reserved));
        }

        AddViewData(t);
    }

    // Binary
    {
        BinaryViewDataPtr b = std::make_shared<BinaryViewData>();
        b->offset = (char*)d_->header_start();
        b->size = d_->DATA_SIZE();
        AddViewData(b);
    }
}

MOEX_NAMESPACE_END