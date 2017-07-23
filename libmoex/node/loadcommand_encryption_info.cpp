//
// Created by everettjf on 2017/7/20.
//

#include "loadcommand_encryption_info.h"


MOEX_NAMESPACE_BEGIN

std::string LoadCommand_LC_ENCRYPTION_INFO::GetShortCharacteristicDescription(){
    return boost::str(boost::format("cryptid=%1%")%cmd()->cryptid);
}

std::string LoadCommand_LC_ENCRYPTION_INFO_64::GetShortCharacteristicDescription(){
    return boost::str(boost::format("cryptid=%1%")%cmd()->cryptid);
}

MOEX_NAMESPACE_END

