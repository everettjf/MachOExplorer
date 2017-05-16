//
// Created by qiwei on 2017/3/29.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_LOAD_DYLIB_H
#define MACHOEXPLORER_LOADCOMMAND_LOAD_DYLIB_H

#include "loadcommand.h"

MOEX_NAMESPACE_BEGIN


class LoadCommand_DYLIB : public LoadCommandImpl<dylib_command>{
private:
    std::string dylib_path_;
    std::string dylib_name_;
public:
    const std::string & dylib_name()const{return dylib_name_;}

    void init(void * offset,NodeContextPtr & ctx)override {
        LoadCommandImpl::init(offset,ctx);

        dylib_path_ = reinterpret_cast<char*>((char*)offset_ + cmd_->dylib.name.offset);

        std::vector<std::string> path_items;
        boost::split(path_items,dylib_path_,boost::is_any_of("/"),boost::token_compress_on);
        if(path_items.size() > 0){
            dylib_name_ = path_items.back();
        }
    }

    std::string GetTypeName() override{ return "dylib_command";}
    std::string GetDisplayName() override{
        return "dylib=" + dylib_name_;
    }
    std::string GetDescription() override{
        return boost::str(boost::format("dylib_command(type=%1%,size=%2%,dylibname=%3%)")
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize
                          % dylib_path_
        );
    }
};

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_LOAD_DYLIB_H
