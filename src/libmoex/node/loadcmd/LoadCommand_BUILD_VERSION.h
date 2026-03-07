#ifndef MACHOEXPLORER_LOADCOMMAND_BUILD_VERSION_H
#define MACHOEXPLORER_LOADCOMMAND_BUILD_VERSION_H

#include "libmoex/node/LoadCommand.h"

MOEX_NAMESPACE_BEGIN

class LoadCommand_LC_BUILD_VERSION : public LoadCommandImpl<qv_build_version_command> {
private:
    std::vector<qv_build_tool_version> tools_;
public:
    const std::vector<qv_build_tool_version> &tools() const { return tools_; }

    void Init(void *offset, NodeContextPtr &ctx) override;
    std::string GetShortCharacteristicDescription() override;

    static std::string GetPlatformString(uint32_t platform);
    static std::string GetToolString(uint32_t tool);
};

MOEX_NAMESPACE_END

#endif // MACHOEXPLORER_LOADCOMMAND_BUILD_VERSION_H
