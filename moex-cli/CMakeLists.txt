cmake_minimum_required(VERSION 3.9)
project(moex_cli)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE LIBMOEX_SOURCE libmoex/*.cpp)
file(GLOB_RECURSE LIBMOEX_HEADER libmoex/*.h)
file(GLOB_RECURSE MOEX_SOURCE src/*.cpp)
file(GLOB_RECURSE MOEX_HEADER src/*.h)

set(SOURCE_FILES
        ${LIBMOEX_SOURCE}
        ${LIBMOEX_HEADER}
        ${MOEX_SOURCE}
        ${MOEX_HEADER}
        )

include_directories(./)

# see https://cmake.org/cmake/help/latest/module/FindBoost.html
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost REQUIRED COMPONENTS
        program_options
        filesystem
        )
include_directories(${Boost_INCLUDE_DIR})
add_executable(moex-cli ${SOURCE_FILES})
target_link_libraries(moex-cli ${Boost_LIBRARIES})
