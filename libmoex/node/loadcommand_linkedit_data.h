//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H
#define MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H

#include "loadcommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LINKEDIT_DATA : public LoadCommandImpl<linkedit_data_command>{
public:
    std::string GetTypeName() override{ return "linkedit_data_command";}
    std::string GetDisplayName() override{
        return hp::GetLoadCommandType(offset_->cmd);
    }
    std::string GetDescription() override{
        return boost::str(boost::format("%1%(type=%2%,size=%3%,data offset=%4%, data size=%5%)")
                          % this->GetDisplayName()
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize
                          % cmd_->dataoff
                          % cmd_->datasize
        );
    }
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H
