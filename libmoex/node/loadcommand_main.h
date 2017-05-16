//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_MAIN_H
#define MACHOEXPLORER_LOADCOMMAND_MAIN_H

#include "loadcommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_MAIN : public LoadCommandImpl<entry_point_command>{
public:
    std::string GetTypeName() override{ return "entry_point_command";}
    std::string GetDisplayName() override{ return "entry_point";}
    std::string GetDescription() override{
        return boost::str(boost::format("%1%(type=%2%,size=%3%,entry offset=%4%, stack size=%5%)")
                          % this->GetDisplayName()
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize
                          % cmd_->entryoff
                          % cmd_->stacksize
        );
    }
};


MOEX_NAMESPACE_END
#endif //MACHOEXPLORER_LOADCOMMAND_MAIN_H
