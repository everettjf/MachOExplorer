#include "LoadCommand_RPATH.h"

MOEX_NAMESPACE_BEGIN

void LoadCommand_LC_RPATH::Init(void *offset, NodeContextPtr &ctx) {
    LoadCommandImpl::Init(offset, ctx);

    if (cmd_->path.offset >= cmd_->cmdsize) {
        path_.clear();
        path_offset_ = nullptr;
        return;
    }

    path_offset_ = reinterpret_cast<char *>(reinterpret_cast<char *>(offset_) + cmd_->path.offset);
    path_ = path_offset_;
}

MOEX_NAMESPACE_END
