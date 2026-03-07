cmake_minimum_required(VERSION 3.16)
project(MachOExplorer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Network)

find_path(CAPSTONE_INCLUDE_DIR capstone/capstone.h)
find_library(CAPSTONE_LIBRARY capstone)

file(GLOB_RECURSE LIBMOEX_SOURCE libmoex/*.cpp)
file(GLOB_RECURSE LIBMOEX_HEADER libmoex/*.h)
file(GLOB_RECURSE MOEX_SOURCE src/*.cpp)
file(GLOB_RECURSE MOEX_HEADER src/*.h)

option(MOEX_BUILD_PARSER_TOOL "Build parser CLI for crash-regression tests" ON)
option(MOEX_ENABLE_FUZZ "Build libFuzzer target for parser fuzzing" OFF)

set(SOURCE_FILES
    ${LIBMOEX_SOURCE}
    ${LIBMOEX_HEADER}
    ${MOEX_SOURCE}
    ${MOEX_HEADER}
    MachOExplorer.qrc
)

add_executable(MachOExplorer ${SOURCE_FILES})
target_include_directories(MachOExplorer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(MachOExplorer PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Network
)

if(CAPSTONE_INCLUDE_DIR AND CAPSTONE_LIBRARY)
    message(STATUS "Capstone enabled: ${CAPSTONE_LIBRARY}")
    target_compile_definitions(MachOExplorer PRIVATE MOEX_HAS_CAPSTONE=1)
    target_include_directories(MachOExplorer PRIVATE ${CAPSTONE_INCLUDE_DIR})
    target_link_libraries(MachOExplorer PRIVATE ${CAPSTONE_LIBRARY})
else()
    message(STATUS "Capstone not found; disassembly view will show unavailable message")
    target_compile_definitions(MachOExplorer PRIVATE MOEX_HAS_CAPSTONE=0)
endif()

if(MOEX_BUILD_PARSER_TOOL)
    add_executable(moex-parse
        ${LIBMOEX_SOURCE}
        ${LIBMOEX_HEADER}
        tools/moex_parse_main.cpp
    )
    target_include_directories(moex-parse PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    add_executable(moex-diff
        ${LIBMOEX_SOURCE}
        ${LIBMOEX_HEADER}
        tools/moex_diff_main.cpp
    )
    target_include_directories(moex-diff PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    add_executable(moex-cache-list
        ${LIBMOEX_SOURCE}
        ${LIBMOEX_HEADER}
        tools/moex_cache_list_main.cpp
    )
    target_include_directories(moex-cache-list PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()

if(MOEX_ENABLE_FUZZ)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_executable(moex-fuzz-parse
            ${LIBMOEX_SOURCE}
            ${LIBMOEX_HEADER}
            fuzz/fuzz_macho_parse.cpp
        )
        target_include_directories(moex-fuzz-parse PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
        target_compile_options(moex-fuzz-parse PRIVATE -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer)
        target_link_options(moex-fuzz-parse PRIVATE -fsanitize=fuzzer,address,undefined)
    else()
        message(WARNING "MOEX_ENABLE_FUZZ requires Clang/libFuzzer; skipping moex-fuzz-parse")
    endif()
endif()
