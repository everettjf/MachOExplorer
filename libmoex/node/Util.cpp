//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "Util.h"
#include <iostream>
using namespace std;
#include <cstdio>


MOEX_NAMESPACE_BEGIN

namespace util {

    std::string FormatUUIDArray(uint8_t d[]){
        char buffer[33];
        sprintf(buffer,
                "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],
                d[8], d[9], d[10], d[11], d[12], d[13], d[14], d[15]);

        return std::string(buffer);
    }

    std::string GetLoadCommandType(uint32_t cmd){
        static std::unordered_map<uint32_t ,std::string> mapper{
                DECLARE_MAP_ITEM(LC_REQ_DYLD)
                DECLARE_MAP_ITEM(LC_SEGMENT)
                DECLARE_MAP_ITEM(LC_SYMTAB)
                DECLARE_MAP_ITEM(LC_SYMSEG)
                DECLARE_MAP_ITEM(LC_THREAD)
                DECLARE_MAP_ITEM(LC_UNIXTHREAD)
                DECLARE_MAP_ITEM(LC_LOADFVMLIB)
                DECLARE_MAP_ITEM(LC_IDFVMLIB)
                DECLARE_MAP_ITEM(LC_IDENT)
                DECLARE_MAP_ITEM(LC_FVMFILE)
                DECLARE_MAP_ITEM(LC_PREPAGE)
                DECLARE_MAP_ITEM(LC_DYSYMTAB)
                DECLARE_MAP_ITEM(LC_LOAD_DYLIB)
                DECLARE_MAP_ITEM(LC_ID_DYLIB)
                DECLARE_MAP_ITEM(LC_LOAD_DYLINKER)
                DECLARE_MAP_ITEM(LC_ID_DYLINKER)
                DECLARE_MAP_ITEM(LC_PREBOUND_DYLIB)
                DECLARE_MAP_ITEM(LC_ROUTINES)
                DECLARE_MAP_ITEM(LC_SUB_FRAMEWORK)
                DECLARE_MAP_ITEM(LC_SUB_UMBRELLA)
                DECLARE_MAP_ITEM(LC_SUB_CLIENT)
                DECLARE_MAP_ITEM(LC_SUB_LIBRARY)
                DECLARE_MAP_ITEM(LC_TWOLEVEL_HINTS)
                DECLARE_MAP_ITEM(LC_PREBIND_CKSUM)
                DECLARE_MAP_ITEM(LC_LOAD_WEAK_DYLIB)
                DECLARE_MAP_ITEM(LC_SEGMENT_64)
                DECLARE_MAP_ITEM(LC_ROUTINES_64)
                DECLARE_MAP_ITEM(LC_UUID)
                DECLARE_MAP_ITEM(LC_RPATH)
                DECLARE_MAP_ITEM(LC_CODE_SIGNATURE)
                DECLARE_MAP_ITEM(LC_SEGMENT_SPLIT_INFO)
                DECLARE_MAP_ITEM(LC_REEXPORT_DYLIB)
                DECLARE_MAP_ITEM(LC_LAZY_LOAD_DYLIB)
                DECLARE_MAP_ITEM(LC_ENCRYPTION_INFO)
                DECLARE_MAP_ITEM(LC_DYLD_INFO)
                DECLARE_MAP_ITEM(LC_DYLD_INFO_ONLY)
                DECLARE_MAP_ITEM(LC_LOAD_UPWARD_DYLIB)
                DECLARE_MAP_ITEM(LC_VERSION_MIN_MACOSX)
                DECLARE_MAP_ITEM(LC_VERSION_MIN_IPHONEOS)
                DECLARE_MAP_ITEM(LC_FUNCTION_STARTS)
                DECLARE_MAP_ITEM(LC_DYLD_ENVIRONMENT)
                DECLARE_MAP_ITEM(LC_MAIN)
                DECLARE_MAP_ITEM(LC_DATA_IN_CODE)
                DECLARE_MAP_ITEM(LC_SOURCE_VERSION)
                DECLARE_MAP_ITEM(LC_DYLIB_CODE_SIGN_DRS)
                DECLARE_MAP_ITEM(LC_ENCRYPTION_INFO_64)
                DECLARE_MAP_ITEM(LC_LINKER_OPTION)
                DECLARE_MAP_ITEM(LC_LINKER_OPTIMIZATION_HINT)
                DECLARE_MAP_ITEM(LC_VERSION_MIN_TVOS)
                DECLARE_MAP_ITEM(LC_VERSION_MIN_WATCHOS)
        };
        try{
            return mapper.at(cmd);
        }catch(std::out_of_range&){
        }
        return "unknown";
    }
    std::string GetMagicString(uint32_t magic){
        static std::unordered_map<uint32_t ,std::string> mapper{
                DECLARE_MAP_ITEM(MH_MAGIC)
                DECLARE_MAP_ITEM(MH_CIGAM)
                DECLARE_MAP_ITEM(MH_MAGIC_64)
                DECLARE_MAP_ITEM(MH_CIGAM_64)
                DECLARE_MAP_ITEM(FAT_MAGIC)
                DECLARE_MAP_ITEM(FAT_CIGAM)
                DECLARE_MAP_ITEM(FAT_MAGIC_64)
                DECLARE_MAP_ITEM(FAT_CIGAM_64)
        };
        try{
            return mapper.at(magic);
        }catch(std::out_of_range&){
        }
        return "unknown";
    }
    std::string GetCpuTypeString(cpu_type_t type){
        static std::unordered_map<cpu_type_t,std::string> mapper{
                DECLARE_MAP_ITEM(CPU_TYPE_ANY)
                DECLARE_MAP_ITEM(CPU_TYPE_VAX)
                DECLARE_MAP_ITEM(CPU_TYPE_MC680x0)
                DECLARE_MAP_ITEM(CPU_TYPE_X86)
                DECLARE_MAP_ITEM(CPU_TYPE_I386)
                DECLARE_MAP_ITEM(CPU_TYPE_X86_64)
                DECLARE_MAP_ITEM(CPU_TYPE_MC98000)
                DECLARE_MAP_ITEM(CPU_TYPE_HPPA)
                DECLARE_MAP_ITEM(CPU_TYPE_ARM)
                DECLARE_MAP_ITEM(CPU_TYPE_ARM64)
                DECLARE_MAP_ITEM(CPU_TYPE_MC88000)
                DECLARE_MAP_ITEM(CPU_TYPE_SPARC)
                DECLARE_MAP_ITEM(CPU_TYPE_I860)
                DECLARE_MAP_ITEM(CPU_TYPE_POWERPC)
                DECLARE_MAP_ITEM(CPU_TYPE_POWERPC64)
        };
        try{
            return mapper.at(type);
        }catch(std::out_of_range&){
        }
        return "unknown";
    }
    std::string GetArchStringFromCpuType(cpu_type_t type){
        static std::unordered_map<cpu_type_t,std::string> mapper{
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_ANY,any)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_VAX,vax)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_MC680x0,mc680x0)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_X86,x86)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_I386,i386)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_X86_64,x86_64)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_MC98000,mc98000)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_HPPA,hppa)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_ARM,arm)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_ARM64,arm64)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_MC88000,mc88000)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_SPARC,sparc)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_I860,i860)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_POWERPC,powerpc)
                DECLARE_MAP_ITEM_VALUE(CPU_TYPE_POWERPC64,powerpc64)
        };
        try{
            return mapper.at(type);
        }catch(std::out_of_range&){
        }
        return "unknown";
    }

    std::string GetCpuSubTypeString(cpu_subtype_t type){
        // todo
        return boost::str(boost::format("%1%")%type);
    }

    std::string GetMachFileType(uint32_t type){
        // todo
        return boost::str(boost::format("%1%")%type);
    }



    std::string GetCmdTypeString(uint32_t cmd){
        static std::unordered_map<uint32_t ,std::string> mapper{
                DECLARE_MAP_ITEM(LC_SEGMENT)
                DECLARE_MAP_ITEM(LC_SYMTAB)
                DECLARE_MAP_ITEM(LC_SYMSEG)
                DECLARE_MAP_ITEM(LC_THREAD)
                DECLARE_MAP_ITEM(LC_UNIXTHREAD)
                DECLARE_MAP_ITEM(LC_LOADFVMLIB)
                DECLARE_MAP_ITEM(LC_IDFVMLIB)
                DECLARE_MAP_ITEM(LC_IDENT)
                DECLARE_MAP_ITEM(LC_FVMFILE)
                DECLARE_MAP_ITEM(LC_PREPAGE)
                DECLARE_MAP_ITEM(LC_DYSYMTAB)
                DECLARE_MAP_ITEM(LC_LOAD_DYLIB)
                DECLARE_MAP_ITEM(LC_ID_DYLIB)
                DECLARE_MAP_ITEM(LC_LOAD_DYLINKER)
                DECLARE_MAP_ITEM(LC_ID_DYLINKER)
                DECLARE_MAP_ITEM(LC_PREBOUND_DYLIB)
                DECLARE_MAP_ITEM(LC_ROUTINES)
                DECLARE_MAP_ITEM(LC_SUB_FRAMEWORK)
                DECLARE_MAP_ITEM(LC_SUB_UMBRELLA)
                DECLARE_MAP_ITEM(LC_SUB_CLIENT)
                DECLARE_MAP_ITEM(LC_SUB_LIBRARY)
                DECLARE_MAP_ITEM(LC_TWOLEVEL_HINTS)
                DECLARE_MAP_ITEM(LC_PREBIND_CKSUM)
                DECLARE_MAP_ITEM(LC_LOAD_WEAK_DYLIB)
                DECLARE_MAP_ITEM(LC_SEGMENT_64)
                DECLARE_MAP_ITEM(LC_ROUTINES_64)
                DECLARE_MAP_ITEM(LC_UUID)
                DECLARE_MAP_ITEM(LC_RPATH)
                DECLARE_MAP_ITEM(LC_CODE_SIGNATURE)
                DECLARE_MAP_ITEM(LC_SEGMENT_SPLIT_INFO)
                DECLARE_MAP_ITEM(LC_REEXPORT_DYLIB)
                DECLARE_MAP_ITEM(LC_LAZY_LOAD_DYLIB)
                DECLARE_MAP_ITEM(LC_ENCRYPTION_INFO)
                DECLARE_MAP_ITEM(LC_DYLD_INFO)
                DECLARE_MAP_ITEM(LC_DYLD_INFO_ONLY)
                DECLARE_MAP_ITEM(LC_LOAD_UPWARD_DYLIB)
                DECLARE_MAP_ITEM(LC_VERSION_MIN_MACOSX)
                DECLARE_MAP_ITEM(LC_VERSION_MIN_IPHONEOS)
                DECLARE_MAP_ITEM(LC_FUNCTION_STARTS)
                DECLARE_MAP_ITEM(LC_DYLD_ENVIRONMENT)
                DECLARE_MAP_ITEM(LC_MAIN)
                DECLARE_MAP_ITEM(LC_DATA_IN_CODE)
                DECLARE_MAP_ITEM(LC_SOURCE_VERSION)
                DECLARE_MAP_ITEM(LC_DYLIB_CODE_SIGN_DRS)
                DECLARE_MAP_ITEM(LC_ENCRYPTION_INFO_64)
                DECLARE_MAP_ITEM(LC_LINKER_OPTION)
                DECLARE_MAP_ITEM(LC_LINKER_OPTIMIZATION_HINT)
                DECLARE_MAP_ITEM(LC_VERSION_MIN_TVOS)
                DECLARE_MAP_ITEM(LC_VERSION_MIN_WATCHOS)
        };
        try{
            return mapper.at(cmd);
        }catch(std::out_of_range&){
        }
        return "unknown";
    }


    std::string AsAddress(void *address)
    {
        return boost::str(boost::format("%016x") % (uint64_t)address);
    }

    std::string AsHexData(void *address, std::size_t size)
    {
        char *offset = (char*)address;
        std::string res;
        const char *pos = (const char*)address;
        for(auto idx = 0; idx < size; ++idx){
            char sz[3] = {0,0,0};
            sprintf(sz,"%02X",(uint8_t)offset[idx]);
            res += std::string(sz);
        }
        return res;
    }

    string AsHexData(uint8_t value){
        char sz[2+1] = {0,0,0};
        sprintf(sz,"%02X",value);
        return std::string(sz);
    }



    string AsHexData(uint16_t value){
        char sz[4+1] = {0,0,0,0,0};
        sprintf(sz,"%04X",value);
        return std::string(sz);
    }



    string AsHexData(uint32_t value){
        return boost::str(boost::format("%08X")%value);
    }

    string AsHexData(uint64_t value){
        return boost::str(boost::format("%016X")%value);
    }




}


MOEX_NAMESPACE_END

