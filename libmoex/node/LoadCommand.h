//
// Created by everettjf on 2017/3/23.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_H
#define MACHOEXPLORER_LOADCOMMAND_H

#include "Node.h"

MOEX_NAMESPACE_BEGIN

class MachHeader;

class LoadCommand : public NodeOffset<load_command>{
protected:
    MachHeader* header_;
public:
    void set_header(MachHeader* header){header_ = header;}
    MachHeader * header(){return header_;}

    bool Is64();
    std::string GetLoadCommandTypeString();

    virtual std::string GetShortCharacteristicDescription(){return "";}

    uint32_t GetCommand(){return offset()->cmd;}
    uint32_t GetCommandSize(){return offset()->cmdsize;}
};

using LoadCommandPtr = std::shared_ptr<LoadCommand>;


template <typename T>
class LoadCommandImpl : public LoadCommand{
protected:
    T *cmd_;
public:
    static constexpr std::size_t data_size_cmd = sizeof(T);

    T *cmd(){return cmd_;}

    void Init(void * offset,NodeContextPtr & ctx)override {
        LoadCommand::Init(offset,ctx);
        cmd_ = reinterpret_cast<T*>(offset);
    }
};


class LoadCommandFactory {
public:
    static LoadCommandPtr GetCommand(uint32_t cmd);
    static LoadCommandPtr Create(void * offset,NodeContextPtr & ctx,MachHeader *header);
};

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_H
