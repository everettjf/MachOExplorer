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

    ConstructNode();

    return true;
}
void ViewNodeManager::Init(BinaryPtr bin){
    bin_ = bin;

    ConstructNode();
}

ViewNode *ViewNodeManager::GetRootNode() {
    if(bin_->IsFat()){
        return fat_.get();
    }else{
        return mh_.get();
    }
}

void ViewNodeManager::ConstructNode(){
    // Construct ViewNode Tree
    if(bin_->IsFat()){
        fat_ = std::make_shared<FatHeaderViewNode>();
        fat_->Init(bin_->fath());
    }else{
        mh_ = std::make_shared<MachHeaderViewNode>();
        mh_->Init(bin_->mh());
    }
}

MOEX_NAMESPACE_END
