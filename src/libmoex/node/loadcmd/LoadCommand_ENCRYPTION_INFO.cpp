//
// Created by everettjf on 2017/7/20.
//

#include "LoadCommand_ENCRYPTION_INFO.h"


MOEX_NAMESPACE_BEGIN

std::string LoadCommand_LC_ENCRYPTION_INFO::GetShortCharacteristicDescription(){
    return fmt::format("cryptid={}",cmd()->cryptid);
}

std::string LoadCommand_LC_ENCRYPTION_INFO_64::GetShortCharacteristicDescription(){
    return fmt::format("cryptid={}",cmd()->cryptid);
}

MOEX_NAMESPACE_END

