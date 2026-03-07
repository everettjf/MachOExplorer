//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MOEXBINARY_H
#define MOEXBINARY_H

#include "Node.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include "FatHeader.h"
#include "MachHeader.h"
#include "Archive.h"
#include "DyldSharedCache.h"
#include "../base/mmap/mmaplib.h"

MOEX_NAMESPACE_BEGIN



class Binary
{
private:
    // File path for the targe MachO file
    std::string filepath_;

    // File mapping holder
    std::unique_ptr<mmaplib::MemoryMappedFile> map_;

    // Pointing to the begining fo the file
    void *memory_;

    // Size of the file mapping
    std::size_t memorysize_;

    // Make a choice from Data below
    Magic magic_;

    // Data : Fat header
    FatHeaderPtr fath_;

    // Data : MachO header
    MachHeaderPtr mh_;

    // Data : ar archive
    ArchivePtr archive_;
    bool is_archive_ = false;

    // Data : dyld shared cache
    DyldSharedCachePtr dyld_cache_;
    bool is_dyld_cache_ = false;

public:
    // Get the root node
    Node *GetNode();

    // Whether it is a Fat file
    bool IsFat(){return magic_.IsFat();}
    bool IsArchive(){return is_archive_;}
    bool IsDyldSharedCache(){return is_dyld_cache_;}

    // Getter for address of begging of file
    char *memory(){return (char*)memory_;}

    // Getter for size
    std::size_t memorysize(){return memorysize_;}

    // Getter for Fat Header
    FatHeaderPtr & fath(){return fath_;}

    // Getter for MachO Header ( one arch )
    MachHeaderPtr & mh(){return mh_;}
    ArchivePtr & archive(){return archive_;}
    DyldSharedCachePtr & dyld_cache(){return dyld_cache_;}

    // Loop each header
    void ForEachHeader(std::function<void(MachHeaderPtr)> callback);

public:
    // Constructor
    Binary(const std::string & filepath);

};
using BinaryPtr = std::shared_ptr<Binary>;


MOEX_NAMESPACE_END

#endif // MOEXBINARY_H
