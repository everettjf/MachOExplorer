//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_SOURCE_VERSION_H
#define MACHOEXPLORER_LOADCOMMAND_SOURCE_VERSION_H


#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_SOURCE_VERSION : public LoadCommandImpl<source_version_command>{
public:
    std::string GetVersion();
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_SOURCE_VERSION_H
