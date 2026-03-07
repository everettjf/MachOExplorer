#include "LoadCommand_BUILD_VERSION.h"
#include <algorithm>

MOEX_NAMESPACE_BEGIN

void LoadCommand_LC_BUILD_VERSION::Init(void *offset, NodeContextPtr &ctx) {
    LoadCommandImpl::Init(offset, ctx);

    tools_.clear();
    if (cmd_->cmdsize < sizeof(qv_build_version_command)) {
        return;
    }

    const char *begin = reinterpret_cast<const char *>(offset_) + sizeof(qv_build_version_command);
    const char *end = reinterpret_cast<const char *>(offset_) + cmd_->cmdsize;

    const std::size_t max_items = static_cast<std::size_t>(end - begin) / sizeof(qv_build_tool_version);
    const std::size_t wanted = std::min<std::size_t>(cmd_->ntools, max_items);

    const auto *tool = reinterpret_cast<const qv_build_tool_version *>(begin);
    for (std::size_t i = 0; i < wanted; ++i) {
        tools_.push_back(tool[i]);
    }
}

std::string LoadCommand_LC_BUILD_VERSION::GetShortCharacteristicDescription() {
    return GetPlatformString(cmd_->platform);
}

std::string LoadCommand_LC_BUILD_VERSION::GetPlatformString(uint32_t platform) {
    switch (platform) {
        case PLATFORM_MACOS: return "macOS";
        case PLATFORM_IOS: return "iOS";
        case PLATFORM_TVOS: return "tvOS";
        case PLATFORM_WATCHOS: return "watchOS";
        case PLATFORM_BRIDGEOS: return "bridgeOS";
        case PLATFORM_MACCATALYST: return "macCatalyst";
        case PLATFORM_IOSSIMULATOR: return "iOSSimulator";
        case PLATFORM_TVOSSIMULATOR: return "tvOSSimulator";
        case PLATFORM_WATCHOSSIMULATOR: return "watchOSSimulator";
        case PLATFORM_DRIVERKIT: return "driverKit";
        default: return "unknown";
    }
}

std::string LoadCommand_LC_BUILD_VERSION::GetToolString(uint32_t tool) {
    switch (tool) {
        case TOOL_CLANG: return "clang";
        case TOOL_SWIFT: return "swift";
        case TOOL_LD: return "ld";
        default: return "unknown";
    }
}

MOEX_NAMESPACE_END
