//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_SYMTAB_H
#define MACHOEXPLORER_LOADCOMMAND_SYMTAB_H

#include "loadcommand.h"
#include "common.h"

MOEX_NAMESPACE_BEGIN


class NList : public NodeOffset<nlist>{
public:

};
using NListPtr = std::shared_ptr<NList>;

class NList64 : public NodeOffset<nlist_64>{
public:

};
using NList64Ptr = std::shared_ptr<NList64>;

class LoadCommand_LC_SYMTAB : public LoadCommandImpl<symtab_command>{
private:
    std::vector<NListPtr> nlists_;
    std::vector<NList64Ptr> nlist64s_;
private:
    void LazyInit(){
        static std::once_flag f;
        std::call_once(f,[&]{

        });
    }
public:
    std::vector<NListPtr> & nlists_ref(){
        LazyInit();
        return nlists_;
    }

    std::vector<NList64Ptr> nlist64s_ref(){
        LazyInit();
        return nlist64s_;
    }

public:
    std::string GetTypeName() override{ return "symtab_command";}
    std::string GetDisplayName() override{ return "symtab";}
    std::string GetDescription() override{
        return boost::str(boost::format("%1%(type=%2%,size=%3%,symbol table offset=%4%, number of symbol entries=%5%)")
                          % this->GetDisplayName()
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize
                          % cmd_->symoff
                          % cmd_->nsyms
        );
    }





};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_SYMTAB_H
