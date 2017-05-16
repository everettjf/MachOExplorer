//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MAGIC_H
#define MAGIC_H

#include "node.h"

MOEX_NAMESPACE_BEGIN


class Magic{
private:
    uint32_t magic_;
public:
    uint32_t magic()const{return magic_;}

    Magic(){}
    Magic(void *memory){ magic_ = *(uint32_t*)memory; }

    void Parse(void *memory){ magic_ = *(uint32_t*)memory; }

    bool IsFat(){
        return magic_ == FAT_MAGIC || magic_ == FAT_CIGAM;
    }
    bool Is64(){
        return magic_ == FAT_MAGIC_64 || magic_ == FAT_CIGAM_64 ||
               magic_ == MH_MAGIC_64 || magic_ == MH_CIGAM_64;
    }
    bool IsValid(){
        return IsFat() || Is64();
    }
    bool IsSwap(){
        return magic_ == FAT_CIGAM || magic_ == FAT_CIGAM_64 ||
               magic_ == MH_CIGAM || magic_ == MH_CIGAM_64;
    }
};

MOEX_NAMESPACE_END

#endif // MAGIC_H
