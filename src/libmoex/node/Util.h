//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MOEXHELPER_H
#define MOEXHELPER_H

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
std::string GetArchStringFromCpuType(cpu_type_t type,cpu_subtype_t subtype);
std::string GetCmdTypeString(uint32_t cmd);

std::string GetCpuSubTypeString(cpu_type_t cputype,cpu_subtype_t subtype);
std::vector<std::tuple<cpu_type_t,cpu_subtype_t,std::string>> GetCpuSubTypeArray(cpu_type_t cputype,cpu_subtype_t subtype);

std::string GetMachFileType(uint32_t type);
std::vector<std::tuple<uint32_t,std::string>> GetMachFlagsArray(uint32_t flag);

template <typename T>
std::string AsString(T value){
    return fmt::format("{}",value);
}

template <typename T>
std::string AsHexString(T value){
    return fmt::format("{0:0>8X}",value);
}

template <typename T>
std::string AsShortHexString(T value){
    return fmt::format("{0:X}",value);
}

template <typename T>
std::string ToString(T value){
    return fmt::format("{}",value);
}
template <typename T>
std::string ToHexString(T value){
    return fmt::format("{0:0>#X}",value);
}


std::string AsAddress(void *address);

template <typename T>
std::string AsAddress(T value){
    return fmt::format("{0:0>8X}",value);
}

std::string AsHexData(void *address,std::size_t size);
std::string AsHexData(char *address,std::size_t size);
std::string AsHexDataPrefix(void *address,std::size_t size);

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
        return fmt::format("{0:0>8X}",value);
    }else if(sizeof(value) == sizeof(uint64_t)){
        return fmt::format("{0:0>16X}",value);
    }else{
        return AsHexData(&value,sizeof(value));
    }
}


std::vector<std::tuple<vm_prot_t,std::string>> ParseProts(vm_prot_t prot);

std::string FormatTimeStamp(uint32_t timestamp);
std::string FormatVersion(uint32_t ver);

const char * readUnsignedLeb128(const char *cur_offset,uint64_t & data,uint32_t & occupy_size);
const char * readSignedLeb128(const char *cur_offset,int64_t & data,uint32_t & occupy_size);

std::vector<char*> ParseStringLiteral(char * offset,uint32_t size);

template <typename T>
std::vector<T*> ParsePointerAsType(char *offset, uint32_t size){
    std::vector<T*> results;

    char *cur = offset;
    char *end = offset + size;

    while(cur < end){
        results.push_back((T*)cur);
        cur += sizeof(T);
    }

    return results;
}

std::vector<char*> ParseDataAsSize(char *offset, uint32_t size,size_t unitsize);



}
MOEX_NAMESPACE_END


#endif // MOEXHELPER_H
