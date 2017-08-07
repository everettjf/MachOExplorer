//
// Created by everettjf on 2017/3/29.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_SEGMENT_H
#define MACHOEXPLORER_LOADCOMMAND_SEGMENT_H


#include "libmoex/node/LoadCommand.h"
#include "libmoex/node/MachSection.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_SEGMENT : public LoadCommandImpl<segment_command>{
protected:
    std::vector<MachSectionPtr> sections_;
public:
    std::vector<MachSectionPtr> & sections_ref(){return sections_;}
    std::string segment_name()const{ return std::string(cmd_->segname,16).c_str();}

    void Init(void * offset,NodeContextPtr & ctx)override ;

    std::string GetShortCharacteristicDescription()override;
    std::vector<std::tuple<vm_prot_t,std::string>> GetMaxProts();
    std::vector<std::tuple<vm_prot_t,std::string>> GetInitProts();
};

class LoadCommand_LC_SEGMENT_64 : public LoadCommandImpl<segment_command_64>{
protected:
    std::vector<MachSectionPtr> sections_;
public:
    std::string segment_name()const{ return std::string(cmd_->segname,16).c_str();}
    std::vector<MachSectionPtr> & sections_ref(){return sections_;}

    void Init(void * offset,NodeContextPtr & ctx)override ;

    std::string GetShortCharacteristicDescription()override;
    std::vector<std::tuple<vm_prot_t,std::string>> GetMaxProts();
    std::vector<std::tuple<vm_prot_t,std::string>> GetInitProts();
};

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_SEGMENT_H
