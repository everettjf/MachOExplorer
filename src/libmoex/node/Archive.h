#ifndef MOEX_ARCHIVE_H
#define MOEX_ARCHIVE_H

#include "Node.h"
#include "MachHeader.h"
#include <vector>

MOEX_NAMESPACE_BEGIN

struct ArchiveMember {
    std::string name;
    uint64_t header_offset = 0;
    uint64_t data_offset = 0;
    uint64_t data_size = 0;
    bool is_macho = false;
    MachHeaderPtr mh;
};
using ArchiveMemberPtr = std::shared_ptr<ArchiveMember>;

class Archive : public Node {
private:
    NodeContextPtr ctx_;
    void *archive_start_ = nullptr;
    std::vector<ArchiveMemberPtr> members_;

private:
    void Parse(void *offset, std::size_t file_size);

public:
    static bool IsArchiveMagic(const void *offset, std::size_t size);

    void Init(void *offset, std::size_t file_size, NodeContextPtr &ctx);
    std::vector<ArchiveMemberPtr> &members() { return members_; }
    NodeContextPtr &ctx() { return ctx_; }

    uint64_t GetRAW(const void *addr) override {
        return (uint64_t)addr - (uint64_t)ctx_->file_start;
    }
};
using ArchivePtr = std::shared_ptr<Archive>;

MOEX_NAMESPACE_END

#endif // MOEX_ARCHIVE_H
