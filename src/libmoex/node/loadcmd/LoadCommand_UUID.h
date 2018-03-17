//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_UUID_H
#define MACHOEXPLORER_LOADCOMMAND_UUID_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_UUID : public LoadCommandImpl<uuid_command>{
public:
    std::string GetUUIDString(){
        return util::FormatUUIDArray(cmd_->uuid);
    }
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_UUID_H
