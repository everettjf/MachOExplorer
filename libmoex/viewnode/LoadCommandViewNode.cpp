//
// Created by everettjf on 2017/8/4.
//

#include "LoadCommandViewNode.h"

MOEX_NAMESPACE_BEGIN


void LoadCommandViewNode::Init(LoadCommandPtr d){
    d_ = d;

}

std::string LoadCommandViewNode::GetDisplayName(){
    std::string charact = d_->GetShortCharacteristicDescription();
    if(charact.length() > 0){
        return boost::str(boost::format("%1%(%2%)")%d_->GetLoadCommandTypeString()%charact);
    }else{
        return d_->GetLoadCommandTypeString();
    }
}



MOEX_NAMESPACE_END