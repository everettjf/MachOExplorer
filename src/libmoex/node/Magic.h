//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MOEXMAGIC_H
#define MOEXMAGIC_H

#include "Node.h"

MOEX_NAMESPACE_BEGIN


class Magic{
private:
    uint32_t magic_ = 0;
public:
    // Construct empty
    Magic(){}

    // Construct by given magic
    Magic(uint32_t magic){ magic_ = magic; }

    // Construct by address
    Magic(void *memory){ magic_ = *(uint32_t*)memory; }

    // Parse by address ( can be reparsed)
    void Parse(void *memory){ magic_ = *(uint32_t*)memory; }

    // Getter for magic number
    uint32_t magic()const{return magic_;}

    // Whether it is Fat binary
    bool IsFat(){
        return magic_ == FAT_MAGIC || magic_ == FAT_CIGAM ||
               magic_ == FAT_MAGIC_64 || magic_ == FAT_CIGAM_64;
    }

    // Whether it is 64 bit arch
    bool Is64(){
        return magic_ == FAT_MAGIC_64 || magic_ == FAT_CIGAM_64 ||
               magic_ == MH_MAGIC_64 || magic_ == MH_CIGAM_64;
    }

    // Whether it is a valid MachO file format
    bool IsValid(){
        return IsFat() || Is64() || magic_ == MH_MAGIC || magic_ == MH_CIGAM;
    }

    // Whether should be swapped
    bool IsSwap(){
        return magic_ == FAT_CIGAM || magic_ == FAT_CIGAM_64 ||
               magic_ == MH_CIGAM || magic_ == MH_CIGAM_64;
    }
};

MOEX_NAMESPACE_END

#endif // MOEXMAGIC_H
