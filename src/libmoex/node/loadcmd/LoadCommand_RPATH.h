#ifndef MACHOEXPLORER_LOADCOMMAND_RPATH_H
#define MACHOEXPLORER_LOADCOMMAND_RPATH_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_RPATH : public LoadCommandImpl<qv_rpath_command> {
private:
    std::string path_;
    char *path_offset_ = nullptr;
public:
    const std::string &path() const { return path_; }
    char *path_offset() { return path_offset_; }

    void Init(void *offset, NodeContextPtr &ctx) override;
    std::string GetShortCharacteristicDescription() override { return path_; }
};

MOEX_NAMESPACE_END

#endif // MACHOEXPLORER_LOADCOMMAND_RPATH_H
