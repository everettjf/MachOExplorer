//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "MoexApplication.h"
#include "libmoex/viewnode/ViewNodeDumper.h"

namespace {

struct CliOptions {
    bool show_help = false;
    bool json_output = false;
    bool include_empty_nodes = false;
    size_t max_rows_per_table = 0;
    size_t max_depth = 0;
    std::string input_file;
    std::string output_file;
    std::string root_path;
};

static void PrintCliUsage(const char *program) {
    std::cout
        << "Usage: " << program << " --cli [options] <file>\n"
        << "\n"
        << "Run full MachOExplorer analysis without opening UI and print results.\n"
        << "\n"
        << "Options:\n"
        << "  --cli                  Run in command-line mode (no GUI)\n"
        << "  --format <text|json>   Output format (default: text)\n"
        << "  --output <path>        Write result to file (default: stdout)\n"
        << "  --max-rows <N>         Limit rows printed per table (0 means all)\n"
        << "  --max-depth <N>        Limit analysis tree depth (0 means all)\n"
        << "  --root-path <path>     Dump only the matching node subtree\n"
        << "  --include-empty        Include empty nodes in output\n"
        << "  -h, --help             Show this help\n";
}

static bool ParseSizeValue(const std::string &raw, size_t &value, std::string &error) {
    try {
        const unsigned long long parsed = std::stoull(raw);
        value = static_cast<size_t>(parsed);
        return true;
    } catch (...) {
        error = "invalid numeric value: " + raw;
        return false;
    }
}

static bool ParseCliOptions(int argc, char *argv[], CliOptions &options, std::string &error) {
    std::vector<std::string> positional;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            options.show_help = true;
            continue;
        }
        if (arg == "--format") {
            if (i + 1 >= argc) {
                error = "missing value for --format";
                return false;
            }
            const std::string fmt = argv[++i];
            if (fmt == "text") {
                options.json_output = false;
            } else if (fmt == "json") {
                options.json_output = true;
            } else {
                error = "unsupported format: " + fmt;
                return false;
            }
            continue;
        }
        if (arg.rfind("--format=", 0) == 0) {
            const std::string fmt = arg.substr(9);
            if (fmt == "text") {
                options.json_output = false;
            } else if (fmt == "json") {
                options.json_output = true;
            } else {
                error = "unsupported format: " + fmt;
                return false;
            }
            continue;
        }
        if (arg == "--output") {
            if (i + 1 >= argc) {
                error = "missing value for --output";
                return false;
            }
            options.output_file = argv[++i];
            continue;
        }
        if (arg.rfind("--output=", 0) == 0) {
            options.output_file = arg.substr(9);
            continue;
        }
        if (arg == "--max-rows") {
            if (i + 1 >= argc) {
                error = "missing value for --max-rows";
                return false;
            }
            if (!ParseSizeValue(argv[++i], options.max_rows_per_table, error)) {
                return false;
            }
            continue;
        }
        if (arg.rfind("--max-rows=", 0) == 0) {
            if (!ParseSizeValue(arg.substr(11), options.max_rows_per_table, error)) {
                return false;
            }
            continue;
        }
        if (arg == "--max-depth") {
            if (i + 1 >= argc) {
                error = "missing value for --max-depth";
                return false;
            }
            if (!ParseSizeValue(argv[++i], options.max_depth, error)) {
                return false;
            }
            continue;
        }
        if (arg.rfind("--max-depth=", 0) == 0) {
            if (!ParseSizeValue(arg.substr(12), options.max_depth, error)) {
                return false;
            }
            continue;
        }
        if (arg == "--root-path") {
            if (i + 1 >= argc) {
                error = "missing value for --root-path";
                return false;
            }
            options.root_path = argv[++i];
            continue;
        }
        if (arg.rfind("--root-path=", 0) == 0) {
            options.root_path = arg.substr(12);
            continue;
        }
        if (arg == "--include-empty") {
            options.include_empty_nodes = true;
            continue;
        }
        if (arg == "--cli") {
            continue;
        }
        if (!arg.empty() && arg[0] == '-') {
            error = "unknown option in --cli mode: " + arg;
            return false;
        }
        positional.push_back(arg);
    }

    if (options.show_help) {
        return true;
    }

    if (positional.size() != 1) {
        error = "expected exactly one input file";
        return false;
    }

    options.input_file = positional[0];
    return true;
}

static int RunCliMode(int argc, char *argv[]) {
    CliOptions options;
    std::string error;
    if (!ParseCliOptions(argc, argv, options, error)) {
        std::cerr << "error: " << error << "\n\n";
        PrintCliUsage(argv[0]);
        return 2;
    }

    if (options.show_help) {
        PrintCliUsage(argv[0]);
        return 0;
    }

    moex::ViewNodeDumpOptions dump_options;
    dump_options.json_output = options.json_output;
    dump_options.include_empty_nodes = options.include_empty_nodes;
    dump_options.max_rows_per_table = options.max_rows_per_table;
    dump_options.max_depth = options.max_depth;
    dump_options.root_path = options.root_path;

    std::ofstream fout;
    std::ostream *out = &std::cout;
    if (!options.output_file.empty()) {
        fout.open(options.output_file, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!fout.good()) {
            std::cerr << "error: cannot open output file: " << options.output_file << "\n";
            return 1;
        }
        out = &fout;
    }

    if (!moex::ViewNodeDumper::DumpFile(options.input_file, dump_options, *out, error)) {
        std::cerr << "analysis failed: " << error << "\n";
        return 1;
    }

    return 0;
}

static bool ShouldRunCliMode(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--cli") {
            return true;
        }
    }
    return false;
}

} // namespace

int main(int argc, char *argv[])
{
    if (ShouldRunCliMode(argc, argv)) {
        return RunCliMode(argc, argv);
    }

    Q_INIT_RESOURCE(MachOExplorer);

    MoexApplication app(argc, argv);

    return app.exec();
}
