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

class FatArch : public NodeData<fat_arch>{
private:
    MachHeaderPtr mh_;

    bool swap_ = false;
public:
    void set_swap(bool swap){swap_ = swap;}
    MachHeaderPtr & mh(){return mh_;}

    void Init(void *offset,NodeContextPtr &ctx) override;

    bool Is64()const{ return mh_->Is64(); }

    std::string GetCpuTypeString();
    std::string GetCpuSubTypeString();
};
using FatArchPtr = std::shared_ptr<FatArch>;


class FatHeader : public NodeData<fat_header>{
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
