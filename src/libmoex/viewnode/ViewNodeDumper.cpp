#include "ViewNodeDumper.h"
#include "libmoex/base/json/json.hpp"
#include <algorithm>
#include <cstdint>
#include <sstream>
#include <vector>

MOEX_NAMESPACE_BEGIN

namespace {

using Json = nlohmann::json;

static std::vector<ViewNode *> CollectChildren(ViewNode *node) {
    std::vector<ViewNode *> children;
    node->ForEachChild([&](ViewNode *child) {
        if (child != nullptr) {
            children.push_back(child);
        }
    });
    return children;
}

static std::string SanitizeCell(const std::string &input) {
    std::string out;
    out.reserve(input.size());
    for (char c : input) {
        if (c == '\n') {
            out += "\\n";
        } else if (c == '\r') {
            out += "\\r";
        } else if (c == '\t') {
            out += "\\t";
        } else {
            out += c;
        }
    }
    return out;
}

static bool NodeHasImmediateContent(ViewNode *node) {
    const auto &table = node->table();
    const auto &binary = node->binary();
    const bool has_rows = table && !table->rows.empty();
    const bool has_binary = binary && !binary->IsEmpty();
    return has_rows || has_binary;
}

static std::string JoinPath(const std::vector<std::string> &segments) {
    std::ostringstream oss;
    for (size_t i = 0; i < segments.size(); ++i) {
        if (i > 0) {
            oss << "/";
        }
        oss << segments[i];
    }
    return oss.str();
}

static const char *ClassifyNodeKind(ViewNode *node) {
    const auto &table = node->table();
    const auto &binary = node->binary();
    const bool has_rows = table && !table->rows.empty();
    const bool has_binary = binary && !binary->IsEmpty();
    if (has_rows && has_binary) {
        return "table+binary";
    }
    if (has_rows) {
        return "table";
    }
    if (has_binary) {
        return "binary";
    }
    return "group";
}

static bool NodeShouldAppear(ViewNode *node, const ViewNodeDumpOptions &options, size_t depth) {
    if (options.include_empty_nodes) {
        return true;
    }

    if (NodeHasImmediateContent(node)) {
        return true;
    }

    if (options.max_depth > 0 && depth >= options.max_depth) {
        return false;
    }

    auto children = CollectChildren(node);
    for (auto *child : children) {
        child->Init();
        if (NodeShouldAppear(child, options, depth + 1)) {
            return true;
        }
    }
    return false;
}

static void DumpTableText(ViewNode *node, const ViewNodeDumpOptions &options, std::ostream &out, const std::string &indent) {
    const auto &table = node->table();
    if (!table || table->rows.empty()) {
        return;
    }

    const size_t total_rows = table->rows.size();
    const size_t limit = options.max_rows_per_table == 0 ? total_rows : std::min(options.max_rows_per_table, total_rows);

    out << indent << "  [table headers] ";
    for (size_t i = 0; i < table->headers.size(); ++i) {
        if (i > 0) out << " | ";
        out << SanitizeCell(table->headers[i]->data);
    }
    out << "\n";

    for (size_t i = 0; i < limit; ++i) {
        const auto &row = table->rows[i];
        out << indent << "  [row " << i << "] ";
        for (size_t col = 0; col < row->items.size(); ++col) {
            if (col > 0) out << " | ";
            out << SanitizeCell(row->items[col]->data);
        }
        out << "\n";
    }

    if (limit < total_rows) {
        out << indent << "  [table truncated] shown=" << limit << " total=" << total_rows << "\n";
    }
}

static Json TableToJson(const TableViewDataPtr &table, const ViewNodeDumpOptions &options) {
    Json j;
    j["headers"] = Json::array();
    for (const auto &header : table->headers) {
        j["headers"].push_back(header->data);
    }

    const size_t total_rows = table->rows.size();
    const size_t limit = options.max_rows_per_table == 0 ? total_rows : std::min(options.max_rows_per_table, total_rows);
    j["totalRows"] = total_rows;
    j["shownRows"] = limit;
    j["rows"] = Json::array();

    for (size_t i = 0; i < limit; ++i) {
        const auto &row = table->rows[i];
        Json r;
        r["values"] = Json::array();
        r["cells"] = Json::object();
        for (const auto &item : row->items) {
            r["values"].push_back(item->data);
        }
        for (size_t col = 0; col < row->items.size(); ++col) {
            const std::string key = col < table->headers.size()
                    ? table->headers[col]->data
                    : ("column" + std::to_string(col));
            r["cells"][key] = row->items[col]->data;
        }
        if (row->size > 0) {
            r["byteLength"] = row->size;
        }
        j["rows"].push_back(r);
    }

    if (limit < total_rows) {
        j["truncated"] = true;
    }
    return j;
}

static void DumpNodeText(ViewNode *node,
                         const ViewNodeDumpOptions &options,
                         size_t depth,
                         std::ostream &out) {
    node->Init();
    if (!NodeShouldAppear(node, options, depth)) {
        return;
    }

    const std::string indent(depth * 2, ' ');
    out << indent << "- " << node->GetDisplayName() << "\n";

    DumpTableText(node, options, out, indent);

    const auto &binary = node->binary();
    if (binary && !binary->IsEmpty()) {
        out << indent << "  [binary] size=" << binary->size
            << " start=0x" << util::AsShortHexString(binary->start_value) << "\n";
    }

    if (options.max_depth > 0 && depth >= options.max_depth) {
        return;
    }

    auto children = CollectChildren(node);
    for (auto *child : children) {
        DumpNodeText(child, options, depth + 1, out);
    }
}

static Json NodeToJson(ViewNode *node,
                       const ViewNodeDumpOptions &options,
                       size_t depth,
                       const std::vector<std::string> &path_segments) {
    node->Init();
    Json j;
    j["name"] = node->GetDisplayName();
    j["depth"] = depth;
    j["path"] = JoinPath(path_segments);
    j["kind"] = ClassifyNodeKind(node);

    const auto &table = node->table();
    if (table && !table->rows.empty()) {
        j["table"] = TableToJson(table, options);
    }

    const auto &binary = node->binary();
    if (binary && !binary->IsEmpty()) {
        j["binary"] = {
            {"size", binary->size},
            {"startValue", binary->start_value}
        };
    }

    j["children"] = Json::array();
    if (options.max_depth == 0 || depth < options.max_depth) {
        auto children = CollectChildren(node);
        for (auto *child : children) {
            child->Init();
            if (!NodeShouldAppear(child, options, depth + 1)) {
                continue;
            }
            auto child_path = path_segments;
            child_path.push_back(child->GetDisplayName());
            j["children"].push_back(NodeToJson(child, options, depth + 1, child_path));
        }
    }

    return j;
}

struct DumpSummary {
    size_t nodes = 0;
    size_t table_nodes = 0;
    size_t binary_nodes = 0;
    size_t total_rows = 0;
};

static void AccumulateSummary(ViewNode *node,
                              const ViewNodeDumpOptions &options,
                              size_t depth,
                              DumpSummary &summary) {
    node->Init();
    if (!NodeShouldAppear(node, options, depth)) {
        return;
    }

    ++summary.nodes;
    const auto &table = node->table();
    if (table && !table->rows.empty()) {
        ++summary.table_nodes;
        summary.total_rows += table->rows.size();
    }

    const auto &binary = node->binary();
    if (binary && !binary->IsEmpty()) {
        ++summary.binary_nodes;
    }

    if (options.max_depth > 0 && depth >= options.max_depth) {
        return;
    }

    auto children = CollectChildren(node);
    for (auto *child : children) {
        AccumulateSummary(child, options, depth + 1, summary);
    }
}

} // namespace

bool ViewNodeDumper::DumpFile(const std::string &filepath,
                              const ViewNodeDumpOptions &options,
                              std::ostream &out,
                              std::string &error) {
    ViewNodeManager manager;
    if (!manager.Init(filepath, error)) {
        return false;
    }

    auto *root = manager.GetRootNode();
    if (root == nullptr) {
        error = "failed to build analysis tree";
        return false;
    }

    if (options.json_output) {
        DumpSummary summary;
        AccumulateSummary(root, options, 0, summary);
        Json payload;
        payload["schemaVersion"] = options.format_version;
        payload["generator"] = "MachOExplorer CLI";
        payload["format"] = "viewnode-json";
        payload["file"] = filepath;
        payload["options"] = {
            {"includeEmptyNodes", options.include_empty_nodes},
            {"maxRowsPerTable", options.max_rows_per_table},
            {"maxDepth", options.max_depth}
        };
        payload["summary"] = {
            {"nodes", summary.nodes},
            {"tableNodes", summary.table_nodes},
            {"binaryNodes", summary.binary_nodes},
            {"totalRows", summary.total_rows}
        };
        payload["analysis"] = NodeToJson(root, options, 0, {root->GetDisplayName()});
        out << payload.dump(2) << "\n";
    } else {
        out << "file: " << filepath << "\n";
        DumpNodeText(root, options, 0, out);
    }

    return true;
}

MOEX_NAMESPACE_END
