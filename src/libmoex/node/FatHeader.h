//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef FATHEADER_H
#define FATHEADER_H

#include "Node.h"
#include "MachHeader.h"
#include "Magic.h"

MOEX_NAMESPACE_BEGIN

class FatArch : public NodeData<qv_fat_arch>{
private:
    MachHeaderPtr mh_;

    bool swap_ = false;
    bool is_fat64_ = false;

    qv_fat_arch raw_arch32_{};
    qv_fat_arch_64 raw_arch64_{};
public:
    static constexpr std::size_t DATA_SIZE32(){ return sizeof(qv_fat_arch); }
    static constexpr std::size_t DATA_SIZE64(){ return sizeof(qv_fat_arch_64); }

    void set_swap(bool swap){swap_ = swap;}
    void set_is_fat64(bool is_fat64){is_fat64_ = is_fat64;}
    MachHeaderPtr & mh(){return mh_;}

    void Init(void *offset,NodeContextPtr &ctx) override;

    bool Is64()const{ return mh_ && mh_->Is64(); }
    bool IsFat64()const{ return is_fat64_; }
    std::size_t EntrySize() const { return is_fat64_ ? DATA_SIZE64() : DATA_SIZE32(); }

    const qv_fat_arch *offset32() const { return is_fat64_ ? nullptr : static_cast<const qv_fat_arch *>(offset_); }
    const qv_fat_arch_64 *offset64() const { return is_fat64_ ? reinterpret_cast<const qv_fat_arch_64 *>(offset_) : nullptr; }

    qv_cpu_type_t cpu_type() const { return is_fat64_ ? raw_arch64_.cputype : raw_arch32_.cputype; }
    qv_cpu_subtype_t cpu_subtype() const { return is_fat64_ ? raw_arch64_.cpusubtype : raw_arch32_.cpusubtype; }
    uint64_t offset_value() const { return is_fat64_ ? raw_arch64_.offset : raw_arch32_.offset; }
    uint64_t size_value() const { return is_fat64_ ? raw_arch64_.size : raw_arch32_.size; }
    uint32_t align_value() const { return is_fat64_ ? raw_arch64_.align : raw_arch32_.align; }

    std::string GetCpuTypeString();
    std::string GetCpuSubTypeString();
};
using FatArchPtr = std::shared_ptr<FatArch>;


class FatHeader : public NodeData<qv_fat_header>{
private:
    std::vector<FatArchPtr> archs_;
public:
    void Init(void * offset, NodeContextPtr&ctx) override;

    const std::vector<FatArchPtr> & archs()const { return archs_;}

    std::string GetMagicString();
};
using FatHeaderPtr = std::shared_ptr<FatHeader>;


MOEX_NAMESPACE_END

#endif // FATHEADER_H
