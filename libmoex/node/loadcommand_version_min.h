//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_VERSION_MIN_H
#define MACHOEXPLORER_LOADCOMMAND_VERSION_MIN_H

#include "loadcommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_VERSION_MIN: public LoadCommandImpl<version_min_command>{
public:
    std::string GetTypeName() override{ return "version_min_command";}
    std::string GetDisplayName() override{ return "version_min";}
    std::string GetDescription() override{
        return boost::str(boost::format("%1%(type=%2%,size=%3%,version=%4%, sdk=%5%)")
                          % this->GetDisplayName()
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize
                          % cmd_->version
                          % cmd_->sdk
        );
    }
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_VERSION_MIN_H
