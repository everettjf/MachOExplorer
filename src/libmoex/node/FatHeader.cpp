//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "FatHeader.h"
#include "Util.h"
#include <cstring>

MOEX_NAMESPACE_BEGIN



void FatArch::Init(void *offset, NodeContextPtr &ctx) {
    offset_ = static_cast<qv_fat_arch *>(offset);
    ctx_ = ctx;

    uint64_t object_offset = 0;
    uint64_t object_size = 0;
    if (is_fat64_) {
        std::memcpy(&raw_arch64_, offset, sizeof(raw_arch64_));
        if (swap_) {
            qv_swap_fat_arch_64(&raw_arch64_, 1, NX_LittleEndian);
        }
        object_offset = raw_arch64_.offset;
        object_size = raw_arch64_.size;
    } else {
        std::memcpy(&raw_arch32_, offset, sizeof(raw_arch32_));
        if (swap_) {
            qv_swap_fat_arch(&raw_arch32_, 1, NX_LittleEndian);
        }
        object_offset = raw_arch32_.offset;
        object_size = raw_arch32_.size;
    }

    if (object_offset > ctx_->file_size || object_size > ctx_->file_size - object_offset) {
        throw NodeException("Malformed Fat Mach-O: arch range exceeds file size");
    }
    if (object_size < sizeof(uint32_t)) {
        throw NodeException("Malformed Fat Mach-O: arch payload too small");
    }

    void *mach_offset = reinterpret_cast<char *>(ctx_->file_start) + object_offset;
    mh_ = std::make_shared<MachHeader>();
    mh_->Init(mach_offset,ctx_);
}

std::string FatArch::GetCpuTypeString()
{
    return util::GetCpuTypeString(cpu_type());
}

std::string FatArch::GetCpuSubTypeString()
{
    return util::GetCpuSubTypeString(cpu_type(), cpu_subtype());
}


void FatHeader::Init(void *offset, NodeContextPtr &ctx) {
    NodeData::Init(offset,ctx);

    bool swap = false;
    if(data_.magic == FAT_CIGAM || data_.magic == FAT_CIGAM_64){
        swap = true;
        qv_swap_fat_header(& data_,NX_LittleEndian);
    }

    const bool fat64 = data_.magic == FAT_MAGIC_64;
    const uint64_t base = reinterpret_cast<uint64_t>(offset_) - reinterpret_cast<uint64_t>(ctx_->file_start);
    if (base > ctx_->file_size || ctx_->file_size - base < sizeof(qv_fat_header)) {
        throw NodeException("Malformed Fat Mach-O: header exceeds file size");
    }

    const uint64_t arch_entry_size = fat64 ? sizeof(qv_fat_arch_64) : sizeof(qv_fat_arch);
    const uint64_t arch_table_size = static_cast<uint64_t>(data_.nfat_arch) * arch_entry_size;
    if (data_.nfat_arch > 0 && arch_table_size / arch_entry_size != data_.nfat_arch) {
        throw NodeException("Malformed Fat Mach-O: nfat_arch overflow");
    }
    if (ctx_->file_size - base - sizeof(qv_fat_header) < arch_table_size) {
        throw NodeException("Malformed Fat Mach-O: arch table exceeds file size");
    }

    char * arch_offset = (char*)offset_ + sizeof(qv_fat_header);

    for(uint32_t i = 0; i < data_.nfat_arch; ++i){
        auto arch = std::make_shared<FatArch>();
        arch->set_swap(swap);
        arch->set_is_fat64(fat64);
        arch->Init(arch_offset + i * arch_entry_size,ctx);

        archs_.push_back(arch);
    }
}

std::string FatHeader::GetMagicString()
{
    return util::GetMagicString(this->data().magic);
}


MOEX_NAMESPACE_END
