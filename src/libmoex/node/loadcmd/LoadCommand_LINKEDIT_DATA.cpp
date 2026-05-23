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
    uint64_t datasize = cmd_->datasize;
    // Clamp the table to the mapped file so a truncated/crafted datasize cannot
    // create entries that point past the end of the file.
    auto fc = this->ctx();
    if(fc && fc->file_start != nullptr){
        const char* fstart = static_cast<const char*>(fc->file_start);
        const char* fend = fstart + fc->file_size;
        if(cur_offset < fstart || cur_offset > fend){
            return dices_;
        }
        const uint64_t avail = static_cast<uint64_t>(fend - cur_offset);
        if(datasize > avail) datasize = avail;
    }

    uint64_t cur_size = 0;
    while(true){
        DataInCodeEntryPtr entry = std::make_shared<DataInCodeEntry>();
        const uint64_t esz = entry->DATA_SIZE();
        if(esz == 0 || cur_size + esz > datasize) break;
        entry->Init(cur_offset,ctx_);
        dices_.push_back(entry);

        cur_offset += esz;
        cur_size += esz;
    }

    return dices_;
}

std::vector<Uleb128Data> &LoadCommand_LC_FUNCTION_STARTS::GetFunctions(){
    if(functions_.size() > 0){
        return functions_;
    }

    const char* start = (char*)this->header_->header_start() + cmd_->dataoff;
    const char* end = start + cmd_->datasize;
    // Clamp the data range to the mapped file: a truncated/crafted command can
    // claim a datasize that runs past the end of the file.
    auto fctx = this->ctx();
    if(fctx && fctx->file_start != nullptr){
        const char* fstart = static_cast<const char*>(fctx->file_start);
        const char* fend = fstart + fctx->file_size;
        if(start < fstart || start > fend){
            return functions_;
        }
        if(end > fend) end = fend;
    }
    const char* cur_offset = start;
    while(cur_offset < end){
        Uleb128Data data;
        data.offset = (uint64_t)cur_offset;
        const char* next = util::readUnsignedLeb128(cur_offset,end,data.data,data.occupy_size);
        if(next == nullptr) break;
        cur_offset = next;

        functions_.push_back(data);
    }
    return functions_;
}


MOEX_NAMESPACE_END


