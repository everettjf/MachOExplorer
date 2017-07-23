//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef HELPER_H
#define HELPER_H

#include "Common.h"

MOEX_NAMESPACE_BEGIN

#define DECLARE_MAP_ITEM(name) {name,#name},
#define DECLARE_MAP_ITEM_VALUE(name,value) {name,#value},

namespace util {

std::string FormatUUIDArray(uint8_t d[16]);
std::string GetLoadCommandType(uint32_t cmd);
std::string GetMagicString(uint32_t magic);
std::string GetCpuTypeString(cpu_type_t type);
std::string GetArchStringFromCpuType(cpu_type_t type);
std::string GetCpuSubTypeString(cpu_subtype_t type);
std::string GetMachFileType(uint32_t type);
std::string GetCmdTypeString(uint32_t cmd);

std::string AsAddress(void *address);
std::string AsData(void *address,std::size_t size);

template <typename T>
std::string AsData(T *address){
    return AsData(address,sizeof(T));
}

template <typename T>
std::string AsValue(T v){
    return boost::str(boost::format("%1%")%v);
}

}

MOEX_NAMESPACE_END


#endif // HELPER_H
