//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef BINARY_H
#define BINARY_H

#include "node/node.h"
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include "node/fatheader.h"
#include "node/machheader.h"

MOEX_NAMESPACE_BEGIN



class Binary
{
private:
    std::string filepath_;
    void *memory_;
    std::size_t memorysize_;

    // Make a choice from Data below
    Magic magic_;

    // Data (1 / 3)
    FatHeaderPtr fath_;
    MachHeaderPtr mh_;
    MachHeader64Ptr mh64_;

    // File Mapping
    boost::interprocess::file_mapping mapping_;
    boost::interprocess::mapped_region region_;
public:
    Node *GetNode(){
        if(magic_.IsFat())
            return fath_.get();
        else if(magic_.Is64())
            return mh64_.get();
        else
            return mh_.get();
    }

    bool IsFat(){return magic_.IsFat();}
    bool Is64(){return magic_.Is64();}

    FatHeaderPtr & fath(){return fath_;}
    MachHeaderPtr & mh(){return mh_;}
    MachHeader64Ptr & mh64(){return mh64_;}

public:
    Binary(const std::string & filepath) : filepath_(filepath){
        using namespace boost::interprocess;

        if(filepath_.length() == 0){
            throw NodeException("invalid file path");
        }

        // Mapping file
        const char * file_name = filepath_.c_str();

        file_mapping tmp_mapping(file_name,read_only);
        mapping_ = std::move(tmp_mapping);
        mapped_region tmp_region(mapping_,read_only);
        region_ = std::move(tmp_region);

        void *addr = region_.get_address();
        std::size_t size = region_.get_size();

        // Set member
        memory_ = addr;
        memorysize_ = size;

        // Context
        NodeContextPtr context = std::make_shared<NodeContext>();
        context->file_start = memory_;

        // Header
        magic_.Parse(memory_);
        if(!magic_.IsValid()){
            throw NodeException("Not a MachO file");
            return;
        }

        if(magic_.IsFat()){
            fath_ = std::make_shared<FatHeader>();
            fath_->init(memory_,context);
        }else if(magic_.Is64()){
            mh64_ = std::make_shared<MachHeader64>();
            mh64_->init(memory_,context);
        }else{
            mh_ = std::make_shared<MachHeader>();
            mh_->init(memory_,context);
        }
    }
};
using BinaryPtr = std::shared_ptr<Binary>;


MOEX_NAMESPACE_END

#endif // BINARY_H
