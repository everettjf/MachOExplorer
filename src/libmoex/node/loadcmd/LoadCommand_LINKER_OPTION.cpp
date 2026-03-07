#include "LoadCommand_LINKER_OPTION.h"

MOEX_NAMESPACE_BEGIN

void LoadCommand_LC_LINKER_OPTION::Init(void *offset, NodeContextPtr &ctx) {
    LoadCommandImpl::Init(offset, ctx);

    options_.clear();
    if (cmd_->cmdsize < sizeof(qv_linker_option_command)) {
        return;
    }

    const char *cur = reinterpret_cast<const char *>(offset_) + sizeof(qv_linker_option_command);
    const char *end = reinterpret_cast<const char *>(offset_) + cmd_->cmdsize;

    for (uint32_t i = 0; i < cmd_->count && cur < end; ++i) {
        const char *term = cur;
        while (term < end && *term != '\0') {
            ++term;
        }

        options_.emplace_back(cur, static_cast<std::size_t>(term - cur));

        if (term >= end) {
            break;
        }
        cur = term + 1;
    }
}

std::string LoadCommand_LC_LINKER_OPTION::GetShortCharacteristicDescription() {
    if (options_.empty()) {
        return "";
    }
    return options_.front();
}

MOEX_NAMESPACE_END
