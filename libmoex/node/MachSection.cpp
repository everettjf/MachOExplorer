//
// Created by everettjf on 2017/7/12.
//

#include "MachSection.h"

MOEX_NAMESPACE_BEGIN

void MachSection::Init(section *offset,NodeContextPtr & ctx){
    is64_ = false;
    section_ = std::make_shared<MachSectionInternal>();
    section_->Init(offset,ctx);
}

void MachSection::Init(section_64 *offset,NodeContextPtr & ctx){
    is64_ = true;
    section64_ = std::make_shared<MachSection64Internal>();
    section64_->Init(offset,ctx);
}


MOEX_NAMESPACE_END
