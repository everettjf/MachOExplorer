//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_SYMTAB_H
#define MACHOEXPLORER_LOADCOMMAND_SYMTAB_H

#include <mach-o/nlist.h>
#include "libmoex/node/LoadCommand.h"
#include "libmoex/node/Common.h"
#include "libmoex/node/MachHeader.h"

MOEX_NAMESPACE_BEGIN


class NListInternal : public NodeOffset<struct nlist>{
};

class NList64Internal : public NodeOffset<struct nlist_64>{
};

class NList{
private:
    bool is64_;
    std::shared_ptr<NListInternal> nlist_;
    std::shared_ptr<NList64Internal> nlist64_;
public:

    void Init(void *offset,NodeContextPtr &ctx,bool is64);

    bool Is64(){return is64_;}
    uint32_t &n_strx(){
        return is64_?
               nlist64_->offset()->n_un.n_strx :
               nlist_->offset()->n_un.n_strx ;
    }
    uint8_t &n_type(){
        return is64_?
               nlist64_->offset()->n_type :
               nlist_->offset()->n_type ;
    }

    uint8_t &n_sect(){
        return is64_?
               nlist64_->offset()->n_sect :
               nlist_->offset()->n_sect ;
    }
    int16_t &n_desc(){
        return nlist_->offset()->n_desc ;
    }
    uint16_t &n_desc64(){
        return nlist64_->offset()->n_desc;
    }
    uint32_t &n_value(){
        return nlist_->offset()->n_value;
    }
    uint64_t &n_value64(){
        return nlist64_->offset()->n_value;
    }
};
using NListPtr = std::shared_ptr<NList>;

class LoadCommand_LC_SYMTAB : public LoadCommandImpl<symtab_command>{
private:
    std::vector<NListPtr> nlists_;
    bool inited_ = false;
private:
    void LazyInit();
public:
    std::vector<NListPtr> & nlists_ref(){
        LazyInit();
        return nlists_;
    }

    uint32_t GetSymbolTableOffset(){
        return cmd_->symoff;
    }
    void * GetSymbolTableOffsetAddress(){
        return (char*)(header_->header_start()) + cmd_->symoff;
    }
    uint32_t GetSymbolTableSize(){
        return cmd_->nsyms;
    }
    uint32_t GetSymbolTableTotalBytes(){
        return header_->Is64()? cmd_->nsyms * sizeof(struct nlist_64) : cmd_->nsyms * sizeof(struct nlist);
    }

    uint32_t GetStringTableOffset(){
        return cmd_->stroff;
    }
    void * GetStringTableOffsetAddress(){
        return (char*)(header_->header_start()) + cmd_->stroff;
    }
    uint32_t GetStringTableSize(){
        return cmd_->strsize;
    }

    std::string GetStringByStrX(uint32_t strx){
        char * stroffset = (char*)GetStringTableOffsetAddress();
        std::string name(stroffset + strx);
        return name;
    }

public:
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_SYMTAB_H
