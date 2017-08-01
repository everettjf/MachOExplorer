//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef HELPER_H
#define HELPER_H

#include "Common.h"
#include <cstdio>

MOEX_NAMESPACE_BEGIN

#define DECLARE_PAIR_ITEM(name) {name,#name},
#define DECLARE_MAP_ITEM(name) {name,#name},
#define DECLARE_MAP_ITEM_VALUE(name,value) {name,#value},

namespace util {

std::string FormatUUIDArray(uint8_t d[16]);
std::string GetLoadCommandType(uint32_t cmd);
std::string GetMagicString(uint32_t magic);
std::string GetCpuTypeString(cpu_type_t type);
std::string GetArchStringFromCpuType(cpu_type_t type);
std::string GetCmdTypeString(uint32_t cmd);

std::string GetCpuSubTypeString(cpu_type_t cputype,cpu_subtype_t subtype);
std::vector<std::tuple<cpu_type_t,cpu_subtype_t,std::string>> GetCpuSubTypeArray(cpu_type_t cputype,cpu_subtype_t subtype);

std::string GetMachFileType(uint32_t type);
std::vector<std::tuple<uint32_t,std::string>> GetMachFlagsArray(uint32_t flag);

template <typename T>
std::string AsString(T value){
    return boost::str(boost::format("%1%") % value);
}
template <typename T>
std::string AsHexString(T value){
    return boost::str(boost::format("%08X") % value);
}

std::string AsAddress(void *address);

template <typename T>
std::string AsAddress(T value){
    return boost::str(boost::format("%08X") % value);
}

std::string AsHexData(void *address,std::size_t size);

template <typename T>
std::string AsHexData(T & value){
    if(sizeof(value) == sizeof(uint8_t)){
        char sz[2+1] = {0,0,0};
        sprintf(sz,"%02X",value);
        return std::string(sz);
    }else if(sizeof(value) == sizeof(uint16_t)){
        char sz[4+1] = {0,0,0,0,0};
        sprintf(sz,"%04X",value);
        return std::string(sz);
    }else if(sizeof(value) == sizeof(uint32_t)){
        return boost::str(boost::format("%08X")%value);
    }else if(sizeof(value) == sizeof(uint64_t)){
        return boost::str(boost::format("%016X")%value);
    }else{
        return AsHexData(&value,sizeof(value));
    }
}
}

MOEX_NAMESPACE_END


#endif // HELPER_H
