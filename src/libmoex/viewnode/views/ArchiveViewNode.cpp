#include "ArchiveViewNode.h"

MOEX_NAMESPACE_BEGIN

void ArchiveViewNode::Init(ArchivePtr archive) {
    archive_ = archive;
    headers_.clear();

    for (auto &member : archive_->members()) {
        if (!member->is_macho || !member->mh) {
            continue;
        }
        auto node = std::make_shared<MachHeaderViewNode>();
        node->Init(member->mh);
        headers_.push_back(node);
    }
}

void ArchiveViewNode::ForEachChild(std::function<void(ViewNode*)> callback) {
    for (auto &h : headers_) {
        callback(h.get());
    }
}

void ArchiveViewNode::InitViewDatas() {
    using namespace moex::util;

    auto t = CreateTableView(archive_.get());
    t->SetHeaders({"Offset", "Name", "Type", "Size"});
    t->SetWidths({120, 320, 120, 120});

    for (auto &member : archive_->members()) {
        const char *data = (char *)archive_->ctx()->file_start + member->data_offset;
        t->AddRow((void *)data, member->data_size, {
                AsShortHexString(member->data_offset),
                member->name,
                member->is_macho ? "Mach-O" : "data",
                AsString(member->data_size)
        });
    }

    auto b = CreateBinaryView();
    b->offset = (char *)archive_->ctx()->file_start;
    b->size = archive_->ctx()->file_size;
    b->start_value = 0;
}

MOEX_NAMESPACE_END
