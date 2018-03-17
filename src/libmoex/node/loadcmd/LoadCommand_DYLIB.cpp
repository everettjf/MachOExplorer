//
// Created by everettjf on 2017/7/20.
//

#include "LoadCommand_DYLIB.h"


MOEX_NAMESPACE_BEGIN


void LoadCommand_DYLIB::Init(void * offset,NodeContextPtr & ctx){
    LoadCommandImpl::Init(offset,ctx);

    dylib_path_offset_ = reinterpret_cast<char*>((char*)offset_ + cmd_->dylib.name.offset);
    dylib_path_ = dylib_path_offset_;

    std::vector<std::string> path_items;
    boost::split(path_items,dylib_path_,boost::is_any_of("/"),boost::token_compress_on);
    if(path_items.size() > 0){
        dylib_name_ = path_items.back();
    }
}

std::string LoadCommand_DYLIB::GetShortCharacteristicDescription(){
    return dylib_name();
}

std::string LoadCommand_DYLIB::GetTimeStamp(){
    return util::FormatTimeStamp(cmd_->dylib.timestamp);
}
std::string LoadCommand_DYLIB::GetCurrentVersion(){
    return util::FormatVersion(cmd_->dylib.current_version);
}
std::string LoadCommand_DYLIB::GetCompatibilityVersion(){
    return util::FormatVersion(cmd_->dylib.compatibility_version);
}

MOEX_NAMESPACE_END
