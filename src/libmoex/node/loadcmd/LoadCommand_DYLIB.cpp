//
// Created by everettjf on 2017/7/20.
//

#include "LoadCommand_DYLIB.h"


MOEX_NAMESPACE_BEGIN


void LoadCommand_DYLIB::Init(void * offset,NodeContextPtr & ctx){
    LoadCommandImpl::Init(offset,ctx);

    dylib_path_offset_ = reinterpret_cast<char*>((char*)offset_ + cmd_->dylib.name.offset);
    dylib_path_ = dylib_path_offset_;

    std::size_t pos = dylib_path_.find_last_of("/");
    if(pos != std::string::npos){
        dylib_name_ = dylib_path_.substr(pos+1,dylib_path_.length());
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
