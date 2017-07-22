//
// Created by everettjf on 2017/7/23.
//

#include "viewnodemanager.h"

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
    // todo


    return true;
}

ViewNode *ViewNodeManager::GetRootNode() {
    return nullptr;
}
MOEX_NAMESPACE_END
