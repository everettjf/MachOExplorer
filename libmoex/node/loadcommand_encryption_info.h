//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_ENCRYPTION_INFO_H
#define MACHOEXPLORER_LOADCOMMAND_ENCRYPTION_INFO_H

#include "loadcommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_ENCRYPTION_INFO : public LoadCommandImpl<encryption_info_command>{
public:
    std::string GetTypeName() override{ return "encryption_info_command";}
    std::string GetDisplayName() override{ return "encryption_info";}
    std::string GetDescription() override{
        return boost::str(boost::format("%1%(type=%2%,size=%3%,crypt offset=%4%, crypt size=%5%, crypt id=%6%)")
                          % this->GetDisplayName()
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize
                          % cmd_->cryptoff
                          % cmd_->cryptsize
                          % cmd_->cryptid
        );
    }
};

class LoadCommand_LC_ENCRYPTION_INFO_64 : public LoadCommandImpl<encryption_info_command_64>{
public:
    std::string GetTypeName() override{ return "encryption_info_command_64";}
    std::string GetDisplayName() override{ return "encryption_info";}
    std::string GetDescription() override{
        return boost::str(boost::format("%1%(type=%2%,size=%3%,crypt offset=%4%, crypt size=%5%, crypt id=%6%)")
                          % this->GetDisplayName()
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize
                          % cmd_->cryptoff
                          % cmd_->cryptsize
                          % cmd_->cryptid
        );
    }
};

MOEX_NAMESPACE_END
#endif //MACHOEXPLORER_LOADCOMMAND_ENCRYPTION_INFO_H
