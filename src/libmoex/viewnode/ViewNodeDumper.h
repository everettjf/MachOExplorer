#ifndef MOEX_VIEWNODE_DUMPER_H
#define MOEX_VIEWNODE_DUMPER_H

#include "ViewNodeManager.h"
#include <iosfwd>

MOEX_NAMESPACE_BEGIN

struct ViewNodeDumpOptions {
    bool json_output = false;
    bool include_empty_nodes = false;
    size_t max_rows_per_table = 0;
    size_t max_depth = 0;
    std::string format_version = "1.1";
    std::string root_path;
};

class ViewNodeDumper {
public:
    static bool DumpFile(const std::string &filepath,
                         const ViewNodeDumpOptions &options,
                         std::ostream &out,
                         std::string &error);
};

MOEX_NAMESPACE_END

#endif // MOEX_VIEWNODE_DUMPER_H
