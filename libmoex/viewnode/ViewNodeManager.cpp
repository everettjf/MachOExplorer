//
// Created by everettjf on 2017/7/23.
//

#include "ViewNodeManager.h"

MOEX_NAMESPACE_BEGIN


bool ViewNodeManager::Init(const std::string &filepath, std::string &error){
    // Parse
    try{
        bin_ = std::make_shared<Binary>(filepath);
    }catch(std::exception & ex){
        error = ex.what();
        return false;
    }

    // Construct ViewNode Tree
    if(bin_->IsFat()){
        fat_ = std::make_shared<FatBinaryViewNode>(bin_->fath());
    }else{
        exe_ = std::make_shared<ExecutableViewNode>(bin_->mh());
    }

    return true;
}

ViewNode *ViewNodeManager::GetRootNode() {
    if(bin_->IsFat()){
        return fat_.get();
    }else{
        return exe_.get();
    }
}
MOEX_NAMESPACE_END
