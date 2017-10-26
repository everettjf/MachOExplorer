//
// Created by everettjf on 2017/7/20.
//
#include "LoadCommand_DYLD_INFO.h"


MOEX_NAMESPACE_BEGIN

std::string LoadCommand_DYLD_INFO::GetRebaseTypeString(uint8_t type){
    switch(type){
        case REBASE_TYPE_POINTER: return "REBASE_TYPE_POINTER";
        case REBASE_TYPE_TEXT_ABSOLUTE32: return "REBASE_TYPE_TEXT_ABSOLUTE32";
        case REBASE_TYPE_TEXT_PCREL32: return "REBASE_TYPE_TEXT_PCREL32";
        default: return "Unknown";
    }
}
std::string LoadCommand_DYLD_INFO::GetRebaseTypeShortString(uint8_t type){
    switch(type){
        case REBASE_TYPE_POINTER: return "POINTER";
        case REBASE_TYPE_TEXT_ABSOLUTE32: return "ABSOLUTE32";
        case REBASE_TYPE_TEXT_PCREL32: return "PCREL32";
        default: return "Unknown";
    }
}

MOEX_NAMESPACE_END

