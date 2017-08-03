//
// Created by everettjf on 2017/7/20.
//

#include "LoadCommand_DYLIB.h"


MOEX_NAMESPACE_BEGIN


void LoadCommand_DYLIB::Init(void * offset,NodeContextPtr & ctx){
    LoadCommandImpl::Init(offset,ctx);

    dylib_path_ = reinterpret_cast<char*>((char*)offset_ + cmd_->dylib.name.offset);

    std::vector<std::string> path_items;
    boost::split(path_items,dylib_path_,boost::is_any_of("/"),boost::token_compress_on);
    if(path_items.size() > 0){
        dylib_name_ = path_items.back();
    }
}

std::string LoadCommand_DYLIB::GetShortCharacteristicDescription(){
    return dylib_name();
}


MOEX_NAMESPACE_END
