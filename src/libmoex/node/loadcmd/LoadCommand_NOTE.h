#ifndef MACHOEXPLORER_LOADCOMMAND_NOTE_H
#define MACHOEXPLORER_LOADCOMMAND_NOTE_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_NOTE : public LoadCommandImpl<qv_note_command> {
private:
    std::string owner_;
public:
    const std::string &owner() const { return owner_; }

    void Init(void *offset, NodeContextPtr &ctx) override;
    std::string GetShortCharacteristicDescription() override { return owner_; }
};

MOEX_NAMESPACE_END

#endif // MACHOEXPLORER_LOADCOMMAND_NOTE_H
