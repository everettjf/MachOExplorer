//
// Created by everettjf on 2017/7/20.
//

#include "LoadCommand_LINKEDIT_DATA.h"
#include "../MachHeader.h"

MOEX_NAMESPACE_BEGIN


std::string DataInCodeEntry::GetKindString(){
    switch(offset_->kind){
    case DICE_KIND_DATA:return "DICE_KIND_DATA";
    case DICE_KIND_JUMP_TABLE8: return "DICE_KIND_JUMP_TABLE8";
    case DICE_KIND_JUMP_TABLE16: return "DICE_KIND_JUMP_TABLE16";
    case DICE_KIND_JUMP_TABLE32: return "DICE_KIND_JUMP_TABLE32";
    case DICE_KIND_ABS_JUMP_TABLE32: return "DICE_KIND_ABS_JUMP_TABLE32";
    default: return "unknown";
    }
}

std::vector<DataInCodeEntryPtr> &LoadCommand_LC_DATA_IN_CODE::GetDices(){
    if(dices_.size()>0){
        return dices_;
    }

    char* cur_offset = (char*)this->header_->header_start() + cmd_->dataoff;
    uint32_t cur_size = 0;
    while(cur_size < cmd_->datasize){
        DataInCodeEntryPtr entry = std::make_shared<DataInCodeEntry>();
        entry->Init(cur_offset,ctx_);
        dices_.push_back(entry);

        cur_offset += entry->DATA_SIZE();
        cur_size += entry->DATA_SIZE();
    }

    return dices_;
}

std::vector<Uleb128Data> &LoadCommand_LC_FUNCTION_STARTS::GetFunctions(){
    if(functions_.size() > 0){
        return functions_;
    }

    const char* start = (char*)this->header_->header_start() + cmd_->dataoff;
    const char* end = start + cmd_->datasize;
    const char* cur_offset = start;
    while(cur_offset < start +  cmd_->datasize){
        Uleb128Data data;
        data.offset = (uint64_t)cur_offset;
        cur_offset = util::readUnsignedLeb128(cur_offset,data.data,data.occupy_size);

        functions_.push_back(data);
    }
    return functions_;
}


MOEX_NAMESPACE_END


