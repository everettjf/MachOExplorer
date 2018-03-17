//
// Created by everettjf on 2017/7/21.
//

#include "LoadCommand_VERSION_MIN.h"

MOEX_NAMESPACE_BEGIN

std::string LoadCommand_VERSION_MIN::GetVersion(){
    return util::FormatVersion(cmd_->version);
}

std::string LoadCommand_VERSION_MIN::GetSDK(){
    return util::FormatVersion(cmd_->sdk);
}

MOEX_NAMESPACE_END
