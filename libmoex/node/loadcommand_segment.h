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
    std::string GetDisplayName() override{
        return this->segment_name();
    }
    std::string GetDescription() override{
        return boost::str(boost::format("type=%1%,size=%2%,segname=%3%")
                          % hp::GetLoadCommandType(cmd_->cmd)
                          % cmd_->cmdsize
                          % this->segment_name()
        );
    }
    void ForEachChild(std::function<void(Node*)> func) override{
        for(auto section : sections_){
            func(section.get());
        }
    }
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
    std::string GetDisplayName() override{
        return this->segment_name();
    }
    std::string GetDescription() override{
        return boost::str(boost::format("type=%1%,size=%2%,segname=%3%")
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize
                          % this->segment_name()
        );
    }
    void ForEachChild(std::function<void(Node*)> func) override{
        for(auto section : sections_){
            func(section.get());
        }
    }
};


// maybe useless for the wrapper below
//class LoadCommandSegment{
//private:
//    LoadCommandPtr cmd_;
//    bool is64_ = false;
//    LoadCommand_LC_SEGMENT *seg_ = nullptr;
//    LoadCommand_LC_SEGMENT_64 *seg64_ = nullptr;
//
//public:
//    LoadCommandSegment(LoadCommandPtr cmd):cmd_(cmd){
//        if(cmd_->offset()->cmd == LC_SEGMENT) {
//            is64_ = false;
//            seg_ = static_cast<moex::LoadCommand_LC_SEGMENT*>(cmd_.get());
//
//        }else if(cmd_->offset()->cmd == LC_SEGMENT_64){
//            is64_ = true;
//            seg64_ = static_cast<moex::LoadCommand_LC_SEGMENT_64*>(cmd_.get());
//        }else{
//            // error
//        }
//    }
//
//
//};

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_SEGMENT_H
