//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_VERSION_MIN_H
#define MACHOEXPLORER_LOADCOMMAND_VERSION_MIN_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_VERSION_MIN: public LoadCommandImpl<version_min_command>{
public:
    std::string GetVersion();
    std::string GetSDK();
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_VERSION_MIN_H
