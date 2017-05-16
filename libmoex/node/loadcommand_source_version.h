//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_SOURCE_VERSION_H
#define MACHOEXPLORER_LOADCOMMAND_SOURCE_VERSION_H


#include "loadcommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_SOURCE_VERSION : public LoadCommandImpl<source_version_command>{
public:
    std::string GetTypeName() override{ return "source_version_command";}
    std::string GetDisplayName() override{ return "source_version";}
    std::string GetDescription() override{
        return boost::str(boost::format("%1%(type=%2%,size=%3%,version=%4%)")
                          % this->GetDisplayName()
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize
                          % cmd_->version
        );
    }
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_SOURCE_VERSION_H
