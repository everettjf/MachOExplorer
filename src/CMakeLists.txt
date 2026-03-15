cmake_minimum_required(VERSION 3.16)
project(MachOExplorer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment version" FORCE)
endif()

if(WIN32)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

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
    MachOExplorer.icns
)

if(WIN32)
    list(APPEND SOURCE_FILES MachOExplorer.rc)
endif()

set_source_files_properties(MachOExplorer.icns PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
)

if(WIN32)
    add_executable(MachOExplorer WIN32 ${SOURCE_FILES})
else()
    add_executable(MachOExplorer ${SOURCE_FILES})
endif()
target_include_directories(MachOExplorer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(MachOExplorer PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Network
)

if(APPLE)
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/libmoex/ver.h"
        MOEX_VER_LINE REGEX "^#define LIBMOEX_VERSION \".*\"$")
    set(MOEX_APP_VERSION "0.0.0")
    if(MOEX_VER_LINE)
        string(REGEX REPLACE ".*\"([0-9]+\\.[0-9]+\\.[0-9]+)\".*" "\\1"
            MOEX_APP_VERSION "${MOEX_VER_LINE}")
    endif()

    set_target_properties(MachOExplorer PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "MachOExplorer"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.everettjf.machoexplorer"
        MACOSX_BUNDLE_BUNDLE_VERSION "${MOEX_APP_VERSION}"
        MACOSX_BUNDLE_ICON_FILE "MachOExplorer.icns"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${MOEX_APP_VERSION}"
        MACOSX_BUNDLE_INFO_STRING "MachOExplorer ${MOEX_APP_VERSION}"
    )
endif()

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

    add_executable(moex-cache-extract
        ${LIBMOEX_SOURCE}
        ${LIBMOEX_HEADER}
        tools/moex_cache_extract_main.cpp
    )
    target_include_directories(moex-cache-extract PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
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
