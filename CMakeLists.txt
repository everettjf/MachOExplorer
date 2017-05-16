cmake_minimum_required(VERSION 3.6)
project(moex)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        libmoex/node/common.h
        libmoex/node/fatheader.cpp
        libmoex/node/fatheader.h
        libmoex/node/helper.cpp
        libmoex/node/helper.h
        libmoex/node/loadcommand.h
        libmoex/node/loadcommand_dyld_info.h
        libmoex/node/loadcommand_dylib.h
        libmoex/node/loadcommand_dysymtab.h
        libmoex/node/loadcommand_encryption_info.h
        libmoex/node/loadcommand_factory.h
        libmoex/node/loadcommand_linkedit_data.h
        libmoex/node/loadcommand_load_dylinker.h
        libmoex/node/loadcommand_main.h
        libmoex/node/loadcommand_segment.h
        libmoex/node/loadcommand_source_version.h
        libmoex/node/loadcommand_symtab.h
        libmoex/node/loadcommand_uuid.h
        libmoex/node/loadcommand_version_min.h
        libmoex/node/machheader.h
        libmoex/node/machsection.h
        libmoex/node/magic.h
        libmoex/node/node.h
        libmoex/node/nodeview.cpp
        libmoex/node/nodeview.h
        libmoex/node/table.h
        libmoex/util/singleton.cpp
        libmoex/util/singleton.h
        libmoex/binary.h
        moex-cli/moex-cli.cpp)

include_directories(./)

add_executable(moex ${SOURCE_FILES})

# see https://cmake.org/cmake/help/latest/module/FindBoost.html
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(moex ${Boost_LIBRARIES})