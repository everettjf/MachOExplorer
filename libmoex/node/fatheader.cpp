//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "fatheader.h"

MOEX_NAMESPACE_BEGIN



void FatArch::Init(void *offset, NodeContextPtr &ctx) {
    NodeData::Init(offset,ctx);

    if(swap_){
        swap_fat_arch(&data_,1,NX_LittleEndian);
    }

    void *mach_offset = (char *)(ctx_->file_start) + data_.offset;
    mh_ = std::make_shared<MachHeader>();
    mh_->Init(mach_offset,ctx_);
}

std::string FatArch::GetDisplayName(){
    std::string cpuname = hp::GetCpuTypeString(data_.cputype);
    boost::algorithm::replace_first(cpuname,"CPU_TYPE_","");
    return cpuname;
}

std::string FatArch::GetDescription(){
    return boost::str(boost::format("cputype=%1%,subcputype=%2%,offset=%3%,size=%4%,align=%5%")
                      % hp::GetCpuTypeString(data_.cputype)
                      % (int)data_.cpusubtype
                      % data_.offset
                      % data_.size
                      % data_.align
                      );
}

void FatArch::ForEachChild(std::function<void (Node *)> func){
    func(mh_.get());
}

void FatHeader::Init(void *offset, NodeContextPtr &ctx) {
    NodeData::Init(offset,ctx);

    bool swap = false;
    if(data_.magic == FAT_CIGAM){
        swap = true;
        swap_fat_header(& data_,NX_LittleEndian);
    }

    char * arch_offset = (char*)offset_ + sizeof(fat_header);

    for(uint32_t i = 0; i < data_.nfat_arch; ++i){
        auto arch = std::make_shared<FatArch>();
        arch->set_swap(swap);
        arch->Init(arch_offset + i * FatArch::DATA_SIZE(),ctx);

        archs_.push_back(arch);
    }
}

std::string FatHeader::GetDescription(){
    return boost::str(boost::format("magic=%1%,nfat_arch=%2%")
                      % hp::GetMagicString(data_.magic)
                      % data_.nfat_arch
                      );
}

void FatHeader::ForEachChild(std::function<void (Node *)> func){
    for(auto arch : archs_){
        func(arch.get());
    }
}

NodeViewArray FatHeader::GetViews()
{
    TableNodeViewPtr view = std::make_shared<TableNodeView>();
    view->SetHeaders({"Offset","Data","Member","Value"});
    view->SetTable({
                       {hp::AsAddress(&(offset_->magic)),hp::AsData(&(offset_->magic)),"magic",hp::AsValue(offset_->magic)},
                       {hp::AsAddress(&(offset_->nfat_arch)),hp::AsData(&(offset_->nfat_arch)),"magic",hp::AsValue(offset_->nfat_arch)},
    });

    return {
        view,
        std::make_shared<AddressNodeView>(offset_,DATA_SIZE())
    };
}


MOEX_NAMESPACE_END
