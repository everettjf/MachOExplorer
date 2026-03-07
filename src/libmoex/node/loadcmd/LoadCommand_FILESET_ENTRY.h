#ifndef MACHOEXPLORER_LOADCOMMAND_FILESET_ENTRY_H
#define MACHOEXPLORER_LOADCOMMAND_FILESET_ENTRY_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_FILESET_ENTRY : public LoadCommandImpl<qv_fileset_entry_command> {
private:
    std::string entry_id_;
    char *entry_id_offset_ = nullptr;
public:
    const std::string &entry_id() const { return entry_id_; }
    char *entry_id_offset() { return entry_id_offset_; }

    void Init(void *offset, NodeContextPtr &ctx) override;
    std::string GetShortCharacteristicDescription() override { return entry_id_; }
};

MOEX_NAMESPACE_END

#endif // MACHOEXPLORER_LOADCOMMAND_FILESET_ENTRY_H
