#include "IntegrityReportViewNode.h"
#include "../../node/loadcmd/LoadCommand_SEGMENT.h"
#include "../../node/loadcmd/LoadCommand_SYMTAB.h"
#include "../../node/loadcmd/LoadCommand_DYSYMTAB.h"
#include "../../node/loadcmd/LoadCommand_DYLD_INFO.h"
#include "../../node/loadcmd/LoadCommand_LINKEDIT_DATA.h"

MOEX_NAMESPACE_BEGIN

void IntegrityReportViewNode::InitViewDatas()
{
    using namespace moex::util;
    if (!mh_) return;

    auto t = CreateTableView();
    t->SetHeaders({"Severity", "Category", "Check", "Details"});
    t->SetWidths({100, 160, 220, 560});

    int errors = 0;
    int warnings = 0;
    int infos = 0;

    auto add_issue = [&](const std::string &severity,
                         const std::string &category,
                         const std::string &check,
                         const std::string &details) {
        t->AddRow({severity, category, check, details});
        if (severity == "ERROR") ++errors;
        else if (severity == "WARN") ++warnings;
        else ++infos;
    };

    const uint64_t file_size = mh_->ctx()->file_size;

    auto check_range = [&](uint64_t off, uint64_t size, const std::string &category, const std::string &check) {
        if (size == 0) {
            add_issue("INFO", category, check, "empty range");
            return;
        }
        if (off > file_size || size > file_size - off) {
            add_issue("ERROR", category, check,
                      fmt::format("out of file range: off=0x{} size=0x{} file=0x{}",
                                  AsShortHexString(off), AsShortHexString(size), AsShortHexString(file_size)));
        } else {
            add_issue("INFO", category, check,
                      fmt::format("ok: [0x{}, 0x{})",
                                  AsShortHexString(off), AsShortHexString(off + size)));
        }
    };

    mh_->ForEachLoadCommand<LoadCommand_LC_SEGMENT>({LC_SEGMENT}, [&](LoadCommand_LC_SEGMENT *seg, bool &stop) {
        const uint64_t off = seg->cmd()->fileoff;
        const uint64_t sz = seg->cmd()->filesize;
        if (off > file_size || sz > file_size - off) {
            add_issue("ERROR", "segment", seg->segment_name(),
                      fmt::format("segment out of range: off=0x{} size=0x{}",
                                  AsShortHexString(off), AsShortHexString(sz)));
        }
        for (auto &sect : seg->sections_ref()) {
            const uint64_t so = sect->sect().offset();
            const uint64_t ss = sect->sect().size_both();
            if (so > file_size || ss > file_size - so) {
                add_issue("ERROR", "section",
                          fmt::format("{}/{}", sect->sect().segment_name(), sect->sect().section_name()),
                          fmt::format("section out of range: off=0x{} size=0x{}", AsShortHexString(so), AsShortHexString(ss)));
            }
        }
    });

    mh_->ForEachLoadCommand<LoadCommand_LC_SEGMENT_64>({LC_SEGMENT_64}, [&](LoadCommand_LC_SEGMENT_64 *seg, bool &stop) {
        const uint64_t off = seg->cmd()->fileoff;
        const uint64_t sz = seg->cmd()->filesize;
        if (off > file_size || sz > file_size - off) {
            add_issue("ERROR", "segment", seg->segment_name(),
                      fmt::format("segment out of range: off=0x{} size=0x{}",
                                  AsShortHexString(off), AsShortHexString(sz)));
        }
        for (auto &sect : seg->sections_ref()) {
            const uint64_t so = sect->sect().offset();
            const uint64_t ss = sect->sect().size_both();
            if (so > file_size || ss > file_size - so) {
                add_issue("ERROR", "section",
                          fmt::format("{}/{}", sect->sect().segment_name(), sect->sect().section_name()),
                          fmt::format("section out of range: off=0x{} size=0x{}", AsShortHexString(so), AsShortHexString(ss)));
            }
        }
    });

    auto *symtab = mh_->FindLoadCommand<LoadCommand_LC_SYMTAB>({LC_SYMTAB});
    if (symtab != nullptr) {
        check_range(symtab->cmd()->symoff, symtab->GetSymbolTableTotalBytes(), "symtab", "symbol table");
        check_range(symtab->cmd()->stroff, symtab->cmd()->strsize, "symtab", "string table");

        if (symtab->cmd()->strsize > 0) {
            uint64_t bad_strx = 0;
            for (auto &n : symtab->nlists_ref()) {
                if (n->n_strx() >= symtab->cmd()->strsize) ++bad_strx;
            }
            if (bad_strx > 0) {
                add_issue("WARN", "symtab", "string index bounds",
                          fmt::format("{} symbols have n_strx out of strsize", bad_strx));
            } else {
                add_issue("INFO", "symtab", "string index bounds", "all n_strx values are in range");
            }
        }
    } else {
        add_issue("INFO", "symtab", "presence", "LC_SYMTAB not present");
    }

    auto *dysym = mh_->FindLoadCommand<LoadCommand_LC_DYSYMTAB>({LC_DYSYMTAB});
    if (dysym != nullptr && symtab != nullptr) {
        const uint32_t nsyms = symtab->cmd()->nsyms;
        auto check_index_count = [&](uint32_t idx, uint32_t count, const std::string &name) {
            if (idx > nsyms || count > nsyms - idx) {
                add_issue("ERROR", "dysymtab", name,
                          fmt::format("index/count overflow: idx={} count={} nsyms={}", idx, count, nsyms));
            }
        };
        check_index_count(dysym->cmd()->ilocalsym, dysym->cmd()->nlocalsym, "locals");
        check_index_count(dysym->cmd()->iextdefsym, dysym->cmd()->nextdefsym, "extdef");
        check_index_count(dysym->cmd()->iundefsym, dysym->cmd()->nundefsym, "undef");
    }

    auto *dyld_info = mh_->FindLoadCommand<LoadCommand_DYLD_INFO>({LC_DYLD_INFO, LC_DYLD_INFO_ONLY});
    if (dyld_info != nullptr) {
        check_range(dyld_info->cmd()->rebase_off, dyld_info->cmd()->rebase_size, "dyld_info", "rebase");
        check_range(dyld_info->cmd()->bind_off, dyld_info->cmd()->bind_size, "dyld_info", "bind");
        check_range(dyld_info->cmd()->weak_bind_off, dyld_info->cmd()->weak_bind_size, "dyld_info", "weak_bind");
        check_range(dyld_info->cmd()->lazy_bind_off, dyld_info->cmd()->lazy_bind_size, "dyld_info", "lazy_bind");
        check_range(dyld_info->cmd()->export_off, dyld_info->cmd()->export_size, "dyld_info", "export");
    }

    auto check_linkedit = [&](auto *cmd, const std::string &cat) {
        if (cmd == nullptr) return;
        check_range(cmd->cmd()->dataoff, cmd->cmd()->datasize, cat, "data range");
    };

    check_linkedit(mh_->FindLoadCommand<LoadCommand_LC_CODE_SIGNATURE>({LC_CODE_SIGNATURE}), "code_signature");
    check_linkedit(mh_->FindLoadCommand<LoadCommand_LC_FUNCTION_STARTS>({LC_FUNCTION_STARTS}), "function_starts");
    check_linkedit(mh_->FindLoadCommand<LoadCommand_LC_DATA_IN_CODE>({LC_DATA_IN_CODE}), "data_in_code");
    check_linkedit(mh_->FindLoadCommand<LoadCommand_LC_DYLD_EXPORTS_TRIE>({LC_DYLD_EXPORTS_TRIE}), "dyld_exports_trie");
    check_linkedit(mh_->FindLoadCommand<LoadCommand_LC_DYLD_CHAINED_FIXUPS>({LC_DYLD_CHAINED_FIXUPS}), "dyld_chained_fixups");

    t->AddSeparator();
    t->AddRow({"SUMMARY", "integrity", "issues",
               fmt::format("errors={} warnings={} infos={}", errors, warnings, infos)});
}

MOEX_NAMESPACE_END
