//
// Created by everettjf on 2017/3/23.
//

#ifndef MACHOEXPLORER_LOADCOMMAND_H
#define MACHOEXPLORER_LOADCOMMAND_H

#include "node.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand : public NodeOffset<load_command>{
public:
    std::string GetTypeName() override{ return "load_command";}
    std::string GetDisplayName() override{
        return boost::str(boost::format("load_command(type=%1%)") % offset_->cmd );
    }
    std::string GetDescription() override{
        return boost::str(boost::format("type=%1%,size=%2%")
                          % hp::GetLoadCommandType(offset_->cmd)
                          % offset_->cmdsize);
    }
};

using LoadCommandPtr = std::shared_ptr<LoadCommand>;


template <typename T>
class LoadCommandImpl : public LoadCommand{
protected:
    T *cmd_;
public:
    static constexpr std::size_t data_size_cmd = sizeof(T);

    void init(void * offset,NodeContextPtr & ctx)override {
        LoadCommand::init(offset,ctx);
        cmd_ = reinterpret_cast<T*>(offset);
    }
};



MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_LOADCOMMAND_H
