#include "LoadCommand_FILESET_ENTRY.h"

MOEX_NAMESPACE_BEGIN

void LoadCommand_LC_FILESET_ENTRY::Init(void *offset, NodeContextPtr &ctx) {
    LoadCommandImpl::Init(offset, ctx);

    if (cmd_->entry_id.offset >= cmd_->cmdsize) {
        entry_id_.clear();
        entry_id_offset_ = nullptr;
        return;
    }

    entry_id_offset_ = reinterpret_cast<char *>(reinterpret_cast<char *>(offset_) + cmd_->entry_id.offset);
    entry_id_ = entry_id_offset_;
}

MOEX_NAMESPACE_END
