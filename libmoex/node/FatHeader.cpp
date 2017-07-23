//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "FatHeader.h"

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


MOEX_NAMESPACE_END
