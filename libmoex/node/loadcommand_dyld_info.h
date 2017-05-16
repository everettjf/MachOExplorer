//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_DYLD_INFO_H
#define MACHOEXPLORER_LOADCOMMAND_DYLD_INFO_H

#include "loadcommand.h"

MOEX_NAMESPACE_BEGIN


class LoadCommand_DYLD_INFO : public LoadCommandImpl<dyld_info_command>{
public:
    std::string GetTypeName() override{ return "dyld_info_command";}
    std::string GetDisplayName() override{ return "dyld_info";}
    std::string GetDescription() override{
        return boost::str(boost::format("%1%(type=%2%,size=%3%,rebase_offset=%4%)")
                          % this->GetDisplayName()
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize
                          % cmd_->rebase_off
        );
    }
};

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_DYLD_INFO_H
