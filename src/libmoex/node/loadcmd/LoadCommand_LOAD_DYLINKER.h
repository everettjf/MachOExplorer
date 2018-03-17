//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_LOAD_DYLINKER_H
#define MACHOEXPLORER_LOADCOMMAND_LOAD_DYLINKER_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_LOAD_DYLINKER : public LoadCommandImpl<dylinker_command>{
private:
    std::string dylinker_path_name_;
    char* dylinker_path_name_offset_;
public:
    const std::string & dylinker_path_name()const{return dylinker_path_name_;}
    char * dylinker_path_name_offset(){return dylinker_path_name_offset_;}

    void Init(void * offset,NodeContextPtr & ctx)override {
        LoadCommandImpl::Init(offset,ctx);

        dylinker_path_name_offset_ = reinterpret_cast<char*>((char*)offset_ + cmd_->name.offset);
        dylinker_path_name_ = dylinker_path_name_offset_;
    }
};


MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_LOAD_DYLINKER_H
