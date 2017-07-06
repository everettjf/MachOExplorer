//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef FATHEADER_H
#define FATHEADER_H

#include "node.h"
#include "machheader.h"
#include "magic.h"

MOEX_NAMESPACE_BEGIN

class FatArch : public NodeData<fat_arch>{
private:
    MachHeaderPtr mh_;

    bool swap_ = false;
public:
    void set_swap(bool swap){swap_ = swap;}
    MachHeaderPtr & mh(){return mh_;}

    void init(void *offset,NodeContextPtr &ctx) override;

    bool is64()const{ return mh_->is64(); }

    std::string GetTypeName() override{ return "fat_arch";}
    std::string GetDisplayName() override;
    std::string GetDescription() override;
    void ForEachChild(std::function<void(Node*)> func) override;
};
using FatArchPtr = std::shared_ptr<FatArch>;


class FatHeader : public NodeData<fat_header>{
private:
    std::vector<FatArchPtr> archs_;
public:
    void init(void * offset, NodeContextPtr&ctx) override;

    const std::vector<FatArchPtr> & archs()const { return archs_;}

    std::string GetTypeName() override{ return "fat_header";}
    std::string GetDisplayName() override{ return "fat_header";}
    std::string GetDescription() override;
    void ForEachChild(std::function<void(Node*)> func) override;
    NodeViewArray GetViews() override;
};
using FatHeaderPtr = std::shared_ptr<FatHeader>;


MOEX_NAMESPACE_END

#endif // FATHEADER_H
