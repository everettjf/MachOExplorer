//
// Created by everettjf on 2017/7/21.
//

#include "LoadCommand_SOURCE_VERSION.h"

MOEX_NAMESPACE_BEGIN

std::string LoadCommand_LC_SOURCE_VERSION::GetSDK(){
    return util::FormatVersion(cmd_->version);
}

MOEX_NAMESPACE_END
