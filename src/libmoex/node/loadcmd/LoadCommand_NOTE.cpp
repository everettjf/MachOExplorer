#include "LoadCommand_NOTE.h"

MOEX_NAMESPACE_BEGIN

void LoadCommand_LC_NOTE::Init(void *offset, NodeContextPtr &ctx) {
    LoadCommandImpl::Init(offset, ctx);

    std::size_t len = 0;
    while (len < sizeof(cmd_->data_owner) && cmd_->data_owner[len] != '\0') {
        ++len;
    }
    owner_ = std::string(cmd_->data_owner, len);
}

MOEX_NAMESPACE_END
