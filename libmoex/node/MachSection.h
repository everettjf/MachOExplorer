//
// Created by everettjf on 2017/4/2.
//

#ifndef MACHOEXPLORER_MACHSECTION_H
#define MACHOEXPLORER_MACHSECTION_H

#include "Node.h"

MOEX_NAMESPACE_BEGIN



class MachSectionInternal : public NodeOffset<section>{
protected:

public:
    std::string segment_name() const {return std::string(offset_->segname,16).c_str();}
    std::string section_name() const {return std::string(offset_->sectname,16).c_str();}

    std::string GetTypeName() override{ return "section";}

};
using MachSectionInternalPtr = std::shared_ptr<MachSectionInternal>;

class MachSection64Internal : public NodeOffset<section_64>{
protected:

public:
    std::string segment_name() const {return std::string(offset_->segname,16).c_str();}
    std::string section_name() const {return std::string(offset_->sectname,16).c_str();}

    std::string GetTypeName() override{ return "section_64";}

};
using MachSection64InternalPtr = std::shared_ptr<MachSection64Internal>;


class MachSection : public Node{
private:
    MachSectionInternalPtr section_;
    MachSection64InternalPtr section64_;
    bool is64_;
public:
    bool Is64()const{return is64_;}

    void Init(section *offset,NodeContextPtr & ctx);
    void Init(section_64 *offset,NodeContextPtr & ctx);

    MachSectionInternalPtr & section_ref(){return section_;}
    MachSection64InternalPtr & section64_ref(){return section64_;}

    const section * offset()const {return section_->offset();}
    const section_64 * offset64()const {return section64_->offset();}

    std::string segment_name() const {return is64_ ? section64_->segment_name() : section_->segment_name();}
    std::string section_name() const {return is64_ ? section64_->section_name() : section_->section_name();}

    std::string GetTypeName() override{ return is64_ ? section64_->GetTypeName() : section_->GetTypeName();}

    uint64_t GetRAW(const void * addr){
        if(is64_)
            return (uint64_t)addr - (uint64_t)section64_->ctx()->file_start;
        else
            return (uint64_t)addr - (uint64_t)section_->ctx()->file_start;
    }
};
using MachSectionPtr = std::shared_ptr<MachSection>;

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_MACHSECTION_H
