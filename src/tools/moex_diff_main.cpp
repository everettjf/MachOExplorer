#include "libmoex/node/Binary.h"
#include "libmoex/node/loadcmd/LoadCommand_SEGMENT.h"
#include "libmoex/node/loadcmd/LoadCommand_SYMTAB.h"
#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

static moex::MachHeaderPtr FirstHeader(moex::Binary &bin) {
    moex::MachHeaderPtr out;
    bin.ForEachHeader([&](moex::MachHeaderPtr h) {
        if (!out) out = h;
    });
    return out;
}

static std::vector<std::string> CollectSections(const moex::MachHeaderPtr &mh) {
    std::vector<std::string> out;
    if (!mh) return out;
    mh->ForEachLoadCommand<moex::LoadCommand_LC_SEGMENT>({LC_SEGMENT}, [&](moex::LoadCommand_LC_SEGMENT *seg, bool &stop) {
        for (auto &s : seg->sections_ref()) {
            out.push_back(s->sect().segment_name() + "/" + s->sect().section_name());
        }
    });
    mh->ForEachLoadCommand<moex::LoadCommand_LC_SEGMENT_64>({LC_SEGMENT_64}, [&](moex::LoadCommand_LC_SEGMENT_64 *seg, bool &stop) {
        for (auto &s : seg->sections_ref()) {
            out.push_back(s->sect().segment_name() + "/" + s->sect().section_name());
        }
    });
    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
    return out;
}

static std::vector<std::string> CollectSymbols(const moex::MachHeaderPtr &mh) {
    std::vector<std::string> out;
    if (!mh) return out;
    auto *sym = mh->FindLoadCommand<moex::LoadCommand_LC_SYMTAB>({LC_SYMTAB});
    if (!sym) return out;
    for (auto &n : sym->nlists_ref()) {
        if (n->n_strx() == 0) continue;
        std::string name = sym->GetStringByStrX(n->n_strx());
        if (!name.empty()) out.push_back(name);
    }
    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
    return out;
}

static void PrintSetDiff(const std::string &title, const std::vector<std::string> &a, const std::vector<std::string> &b) {
    std::set<std::string> sa(a.begin(), a.end());
    std::set<std::string> sb(b.begin(), b.end());
    int only_a = 0;
    int only_b = 0;
    for (const auto &v : sa) if (sb.count(v) == 0) ++only_a;
    for (const auto &v : sb) if (sa.count(v) == 0) ++only_b;
    std::cout << title << ": onlyA=" << only_a << " onlyB=" << only_b
              << " common=" << (sa.size() + sb.size() - only_a - only_b) / 2 << "\n";
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "usage: moex-diff <fileA> <fileB>\n";
        return 2;
    }

    try {
        moex::Binary bin_a(argv[1]);
        moex::Binary bin_b(argv[2]);

        auto mh_a = FirstHeader(bin_a);
        auto mh_b = FirstHeader(bin_b);
        if (!mh_a || !mh_b) {
            std::cerr << "unable to resolve Mach-O headers from one of the files\n";
            return 1;
        }

        std::cout << "A arch=" << mh_a->GetArch() << " ncmds=" << mh_a->data_ptr()->ncmds
                  << " sizeofcmds=" << mh_a->data_ptr()->sizeofcmds << "\n";
        std::cout << "B arch=" << mh_b->GetArch() << " ncmds=" << mh_b->data_ptr()->ncmds
                  << " sizeofcmds=" << mh_b->data_ptr()->sizeofcmds << "\n";

        auto sections_a = CollectSections(mh_a);
        auto sections_b = CollectSections(mh_b);
        auto symbols_a = CollectSymbols(mh_a);
        auto symbols_b = CollectSymbols(mh_b);

        PrintSetDiff("Sections", sections_a, sections_b);
        PrintSetDiff("Symbols", symbols_a, symbols_b);
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "moex-diff failed: " << ex.what() << "\n";
        return 1;
    }
}
