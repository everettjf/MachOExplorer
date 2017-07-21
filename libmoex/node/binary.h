//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef BINARY_H
#define BINARY_H

#include "node.h"
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include "fatheader.h"
#include "machheader.h"

MOEX_NAMESPACE_BEGIN



class Binary
{
private:
    std::string filepath_;
    void *memory_;
    std::size_t memorysize_;

    // Make a choice from Data below
    Magic magic_;

    // Data
    FatHeaderPtr fath_;
    MachHeaderPtr mh_;

    // File Mapping
    boost::interprocess::file_mapping mapping_;
    boost::interprocess::mapped_region region_;
public:
    Node *GetNode(){
        if(magic_.IsFat())
            return fath_.get();
        else
            return mh_.get();
    }

    bool IsFat(){return magic_.IsFat();}

    FatHeaderPtr & fath(){return fath_;}
    MachHeaderPtr & mh(){return mh_;}
    void ForEachHeader(std::function<void(MachHeaderPtr)> callback){
        if(IsFat()){
            for(auto & arch : fath()->archs()){
                callback(arch->mh());
            }
        }else{
            callback(mh_);
        }
    }

public:
    Binary(const std::string & filepath);

};
using BinaryPtr = std::shared_ptr<Binary>;


MOEX_NAMESPACE_END

#endif // BINARY_H
