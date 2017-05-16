#ifndef COMMON_H
#define COMMON_H

#define MOEX_NAMESPACE_BEGIN namespace moex {
#define MOEX_NAMESPACE_END }

#include <string>
#include <memory>
#include <vector>
#include <list>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <functional>
#include <future>
#include <initializer_list>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>

#include <mach-o/loader.h>
#include <mach-o/swap.h>

#include <type_traits>

#include <cstdlib>
#define moexlog(...) printf(__VA_ARGS__);printf("\n");

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <tuple>


#endif // COMMON_H
