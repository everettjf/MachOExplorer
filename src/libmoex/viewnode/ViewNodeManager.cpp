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
    return file_.get();
}

bool ViewNodeManager::IsFat()
{
    return bin_->IsFat();
}

void ViewNodeManager::ConstructNode(){
    file_ = std::make_shared<FileViewNode>();
    file_->Init(bin_);
}

MOEX_NAMESPACE_END
