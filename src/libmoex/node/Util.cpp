//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "Util.h"
#include <iostream>
using namespace std;


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
    std::string GetArchStringFromCpuType(cpu_type_t type,cpu_subtype_t subtype){
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
    std::string GetCpuSubTypeString(cpu_type_t cputype,cpu_subtype_t subtype){
        auto res = GetCpuSubTypeArray(cputype,subtype);
        if(res.size() == 1) return std::get<2>(res[0]);
        if(res.empty()) return "unknown";

        return std::get<2>(res[0]) + "," + std::get<2>(res[1]); // no case that 3 or more items
    }
    std::vector<std::tuple<cpu_type_t,cpu_subtype_t,std::string>> GetCpuSubTypeArray(cpu_type_t cputype,cpu_subtype_t subtype){
        static std::unordered_map<cpu_type_t, std::unordered_map<cpu_subtype_t,std::string>> mapper{
               {CPU_TYPE_POWERPC ,
                        {
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_ALL)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_601)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_602)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_603)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_603e)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_603ev)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_604)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_604e)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_620)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_750)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_7400)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_7450)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_POWERPC_970)
                        }},
               {CPU_TYPE_ARM,
                        {
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_ALL)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_V4T)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_V6)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_V5TEJ)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_XSCALE)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_V7)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_V7F)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_V7S)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_V7K)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_V6M)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_V7M)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_V7EM)
                                DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM_V8)
                        }},
               {CPU_TYPE_ARM64,
                       {
                               DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM64_ALL)
                               DECLARE_MAP_ITEM(CPU_SUBTYPE_ARM64_V8)
                       }},
               {CPU_TYPE_I386,
                       {
                               DECLARE_MAP_ITEM(CPU_SUBTYPE_I386_ALL)
                       }},
               {CPU_TYPE_X86_64,
                       {
                               DECLARE_MAP_ITEM(CPU_SUBTYPE_X86_64_ALL)
                       }},
               {CPU_TYPE_ANY,
                       {
                               DECLARE_MAP_ITEM(CPU_SUBTYPE_MULTIPLE)
                               DECLARE_MAP_ITEM(CPU_SUBTYPE_LITTLE_ENDIAN)
                               DECLARE_MAP_ITEM(CPU_SUBTYPE_BIG_ENDIAN)
                       }},
        };


        std::vector<std::tuple<cpu_type_t,cpu_subtype_t,std::string>> res;
        if((subtype & CPU_SUBTYPE_LIB64) == CPU_SUBTYPE_LIB64){
            int showsubtype = CPU_SUBTYPE_LIB64;
            res.push_back(std::tie(cputype,showsubtype,"CPU_SUBTYPE_LIB64"));
        }

        try{
            int showsubtype = (subtype & ~CPU_SUBTYPE_MASK);
            res.push_back(std::tie(
                    cputype,
                    showsubtype,
                    mapper.at(cputype).at(subtype & ~CPU_SUBTYPE_MASK)
            ));
            return res;
        }catch(std::out_of_range&){
        }
        return {};
    }

    std::string GetMachFileType(uint32_t type){
        static std::unordered_map<uint32_t ,std::string> mapper{
                DECLARE_MAP_ITEM(MH_OBJECT)
                DECLARE_MAP_ITEM(MH_EXECUTE)
                DECLARE_MAP_ITEM(MH_FVMLIB)
                DECLARE_MAP_ITEM(MH_CORE)
                DECLARE_MAP_ITEM(MH_PRELOAD)
                DECLARE_MAP_ITEM(MH_DYLIB)
                DECLARE_MAP_ITEM(MH_DYLINKER)
                DECLARE_MAP_ITEM(MH_BUNDLE)
                DECLARE_MAP_ITEM(MH_DYLIB_STUB)
                DECLARE_MAP_ITEM(MH_DSYM)
                DECLARE_MAP_ITEM(MH_KEXT_BUNDLE)
        };
        try{
            return mapper.at(type);
        }catch(std::out_of_range&){}
        return "unknown";
    }

    std::vector<std::tuple<uint32_t,std::string>> GetMachFlagsArray(uint32_t flag){
        static std::vector<std::pair<uint32_t,std::string>> db{
                DECLARE_PAIR_ITEM(MH_NOUNDEFS)
                DECLARE_PAIR_ITEM(MH_INCRLINK)
                DECLARE_PAIR_ITEM(MH_DYLDLINK)
                DECLARE_PAIR_ITEM(MH_BINDATLOAD)
                DECLARE_PAIR_ITEM(MH_PREBOUND)
                DECLARE_PAIR_ITEM(MH_SPLIT_SEGS)
                DECLARE_PAIR_ITEM(MH_LAZY_INIT)
                DECLARE_PAIR_ITEM(MH_TWOLEVEL)
                DECLARE_PAIR_ITEM(MH_FORCE_FLAT)
                DECLARE_PAIR_ITEM(MH_NOMULTIDEFS)
                DECLARE_PAIR_ITEM(MH_NOFIXPREBINDING)
                DECLARE_PAIR_ITEM(MH_PREBINDABLE)
                DECLARE_PAIR_ITEM(MH_ALLMODSBOUND)
                DECLARE_PAIR_ITEM(MH_SUBSECTIONS_VIA_SYMBOLS)
                DECLARE_PAIR_ITEM(MH_CANONICAL)
                DECLARE_PAIR_ITEM(MH_WEAK_DEFINES)
                DECLARE_PAIR_ITEM(MH_BINDS_TO_WEAK)
                DECLARE_PAIR_ITEM(MH_ALLOW_STACK_EXECUTION)
                DECLARE_PAIR_ITEM(MH_ROOT_SAFE)
                DECLARE_PAIR_ITEM(MH_SETUID_SAFE)
                DECLARE_PAIR_ITEM(MH_NO_REEXPORTED_DYLIBS)
                DECLARE_PAIR_ITEM(MH_PIE)
                DECLARE_PAIR_ITEM(MH_DEAD_STRIPPABLE_DYLIB)
                DECLARE_PAIR_ITEM(MH_HAS_TLV_DESCRIPTORS)
                DECLARE_PAIR_ITEM(MH_NO_HEAP_EXECUTION)
                DECLARE_PAIR_ITEM(MH_APP_EXTENSION_SAFE)
        };
        std::vector<std::tuple<uint32_t,std::string>> res;
        for(auto &item : db){
            if(item.first & flag){
                res.push_back(std::tie(item.first,item.second));
            }
        }
        return res;
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
        return fmt::format("0:0>16X",(uint64_t)address);
    }

    std::string AsHexDataPrefix(void *address,std::size_t size){
        char *offset = (char*)address;
        std::string res = "0x";
        const char *pos = (const char*)address;
        for(auto idx = 0; idx < size; ++idx){
            char sz[3] = {0,0,0};
            sprintf(sz,"%02X",(uint8_t)offset[idx]);
            res += std::string(sz);
        }
        return res;
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


    std::string AsHexData(char *address,std::size_t size){
        return AsHexData((void*)address,size);
    }

    std::vector<std::tuple<vm_prot_t,std::string>> ParseProts(vm_prot_t prot){
        std::vector<std::tuple<vm_prot_t,std::string>> ret;

        if(prot == VM_PROT_NONE){
            ret.push_back(std::make_tuple(VM_PROT_NONE,"VM_PROT_NONE"));
            return ret;
        }

        if(prot & VM_PROT_READ) ret.push_back(std::make_tuple(VM_PROT_READ,"VM_PROT_READ"));
        if(prot & VM_PROT_WRITE) ret.push_back(std::make_tuple(VM_PROT_WRITE,"VM_PROT_WRITE"));
        if(prot & VM_PROT_EXECUTE) ret.push_back(std::make_tuple(VM_PROT_EXECUTE,"VM_PROT_EXECUTE"));

        if(prot & VM_PROT_NO_CHANGE) ret.push_back(std::make_tuple(VM_PROT_NO_CHANGE,"VM_PROT_NO_CHANGE"));
        if(prot & VM_PROT_COPY) ret.push_back(std::make_tuple(VM_PROT_COPY,"VM_PROT_COPY/VM_PROT_WANTS_COPY"));
        if(prot & VM_PROT_IS_MASK) ret.push_back(std::make_tuple(VM_PROT_IS_MASK,"VM_PROT_IS_MASK"));
        return ret;
    }

    std::string FormatTimeStamp(uint32_t timestamp){
        time_t t = (time_t)timestamp;
        return ctime(&t);
    }
    std::string FormatVersion(uint32_t ver){
        return fmt::format("{0:d}.{1:d}.{2:d}"
                          , (ver>>16)
                          , ((ver>>8)&0xff)
                          , (ver&0xff)
        );
    }

    // return next offset
    const char * readUnsignedLeb128(const char *cur_offset,uint64_t & data,uint32_t & occupy_size) {
        const uint8_t* p = (const uint8_t*)cur_offset;

        uint64_t result = 0;
        int bit = 0;

        do {
            uint64_t slice = *p & 0x7f;

            if (bit >= 64 || slice << bit >> bit != slice){
                // error
                data = 0;
                occupy_size = 0;
                return 0;
            } else {
                result |= (slice << bit);
                bit += 7;
            }
        }
        while (*p++ & 0x80);

        data = result;
        occupy_size = p - (const uint8_t*)cur_offset;
        return (const char *)p;
    }
    const char * readSignedLeb128(const char *cur_offset,int64_t & data,uint32_t & occupy_size){
        const uint8_t* p = (const uint8_t*)cur_offset;

        int64_t result = 0;
        int bit = 0;
        uint8_t byte=0;

        do {
            byte = *p++;
            result |= ((byte & 0x7f) << bit);
            bit += 7;
        } while (byte & 0x80);

        // sign extend negative numbers
        if ( (byte & 0x40) != 0 )
        {
            result |= (-1LL) << bit;
        }

        data = result;
        occupy_size = p - (const uint8_t*)cur_offset;
        return (const char *)p;
    }

    std::vector<char*> ParseStringLiteral(char *offset, uint32_t size)
    {
        std::vector<char*> results;

        char *cur = offset;
        char *end = offset + size;

        while(cur < end){
            if(*cur == 0){
                ++cur;
                continue;
            }
            results.push_back(cur);
            cur += strlen(cur);
        }

        return results;
    }



    std::vector<char *> ParseDataAsSize(char *offset, uint32_t size, size_t unitsize){
        std::vector<char*> results;

        char *cur = offset;
        char *end = offset + size;

        while(cur < end){
            results.push_back((char*)cur);
            cur += unitsize;
        }

        return results;
    }

}


MOEX_NAMESPACE_END

