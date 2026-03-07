#ifndef MACHOEXPLORER_LOADCOMMAND_LINKER_OPTION_H
#define MACHOEXPLORER_LOADCOMMAND_LINKER_OPTION_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_LINKER_OPTION : public LoadCommandImpl<qv_linker_option_command> {
private:
    std::vector<std::string> options_;
public:
    const std::vector<std::string> &options() const { return options_; }

    void Init(void *offset, NodeContextPtr &ctx) override;
    std::string GetShortCharacteristicDescription() override;
};

MOEX_NAMESPACE_END

#endif // MACHOEXPLORER_LOADCOMMAND_LINKER_OPTION_H
