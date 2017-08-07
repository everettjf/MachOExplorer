//
// Created by everettjf on 2017/3/31.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H
#define MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_CODE_SIGNATURE : public LoadCommandImpl<linkedit_data_command>{
public:
};


class LoadCommand_LC_SEGMENT_SPLIT_INFO: public LoadCommandImpl<linkedit_data_command>{
public:
};

class LoadCommand_LC_FUNCTION_STARTS: public LoadCommandImpl<linkedit_data_command>{
public:
};


class DataInCodeEntry: public NodeOffset<data_in_code_entry>{
public:
    std::string GetKindString(){
        switch(offset_->kind){
            case DICE_KIND_DATA:return "DICE_KIND_DATA";
            case DICE_KIND_JUMP_TABLE8: return "DICE_KIND_JUMP_TABLE8";
            case DICE_KIND_JUMP_TABLE16: return "DICE_KIND_JUMP_TABLE16";
            case DICE_KIND_JUMP_TABLE32: return "DICE_KIND_JUMP_TABLE32";
            case DICE_KIND_ABS_JUMP_TABLE32: return "DICE_KIND_ABS_JUMP_TABLE32";
            default: return "unknown";
        }
    }
};
using DataInCodeEntryPtr = std::shared_ptr<DataInCodeEntry>;

class LoadCommand_LC_DATA_IN_CODE: public LoadCommandImpl<linkedit_data_command>{
private:
    std::vector<DataInCodeEntryPtr> dices_;
public:
    std::vector<DataInCodeEntryPtr> &GetDices(){
        if(dices_.size()>0){
            return dices_;
        }

        char* cur_offset = (char*)this->ctx()->file_start + cmd_->dataoff;
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
};

class LoadCommand_LC_DYLIB_CODE_SIGN_DRS: public LoadCommandImpl<linkedit_data_command>{
public:
};

class LoadCommand_LC_LINKER_OPTIMIZATION_HINT: public LoadCommandImpl<linkedit_data_command>{
public:
};
MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_LINKEDIT_DATA_H
