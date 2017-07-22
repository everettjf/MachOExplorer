//
// Created by qiwei on 2017/3/29.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_SEGMENT_H
#define MACHOEXPLORER_LOADCOMMAND_SEGMENT_H


#include "loadcommand.h"
#include "machsection.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_SEGMENT : public LoadCommandImpl<segment_command>{
protected:
    std::vector<MachSectionPtr> sections_;
public:
    std::vector<MachSectionPtr> & sections_ref(){return sections_;}

    std::string segment_name()const{ return std::string(cmd_->segname,16).c_str();}

    void Init(void * offset,NodeContextPtr & ctx)override {
        LoadCommandImpl::Init(offset,ctx);

        for(uint32_t idx = 0; idx < cmd_->nsects; ++idx){
            section * cur = reinterpret_cast<section*>((char*)offset_ + data_size_cmd + idx * sizeof(section));

            MachSectionPtr section = std::make_shared<MachSection>();
            section->Init(cur,ctx);
            sections_.push_back(section);
        }
    }

    std::string GetTypeName() override{ return "segment_command";}
};

class LoadCommand_LC_SEGMENT_64 : public LoadCommandImpl<segment_command_64>{
protected:
    std::vector<MachSection64Ptr> sections_;
public:

    std::string segment_name()const{ return std::string(cmd_->segname,16).c_str();}

    std::vector<MachSection64Ptr> & sections_ref(){return sections_;}

    void Init(void * offset,NodeContextPtr & ctx)override {
        LoadCommandImpl::Init(offset,ctx);

        for(uint32_t idx = 0; idx < cmd_->nsects; ++idx){
            section * cur = reinterpret_cast<section*>((char*)offset_ + data_size_cmd + idx * sizeof(section_64));

            MachSection64Ptr section = std::make_shared<MachSection64>();
            section->Init(cur,ctx);
            sections_.push_back(section);
        }
    }

    std::string GetTypeName() override{ return "segment_command_64";}
};

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_SEGMENT_H
