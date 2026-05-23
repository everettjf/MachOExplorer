//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
// C++ (Itanium) symbol demangling via the C++ runtime. No extra dependency:
// abi::__cxa_demangle ships with both libstdc++ and libc++.
#ifndef MOEX_DEMANGLE_H
#define MOEX_DEMANGLE_H

#include <string>
#include <cstdlib>
#include <cxxabi.h>

namespace moex { namespace demangle {

// Returns the demangled C++ name, or an empty string if the input is not an
// Itanium-mangled C++ symbol. Mach-O symbols carry an extra leading underscore
// (e.g. `__Z3fooi`), which is stripped before demangling.
inline std::string DemangleCxx(const std::string &name) {
    const char *m = name.c_str();
    if (name.size() > 1 && name[0] == '_') {
        m = name.c_str() + 1;
    }
    if (!(m[0] == '_' && m[1] == 'Z')) {
        return std::string();
    }
    int status = 0;
    char *demangled = abi::__cxa_demangle(m, nullptr, nullptr, &status);
    std::string out;
    if (status == 0 && demangled != nullptr) {
        out = demangled;
    }
    std::free(demangled);
    return out;
}

}} // namespace moex::demangle

#endif // MOEX_DEMANGLE_H
