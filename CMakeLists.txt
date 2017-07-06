cmake_minimum_required(VERSION 3.6)
project(moex)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE LIBMOEX_SOURCE libmoex/*.cpp)
file(GLOB_RECURSE LIBMOEX_HEADER libmoex/*.h)
file(GLOB_RECURSE MOEX_CLI_SOURCE moex-cli/*.cpp)
file(GLOB_RECURSE MOEX_CLI_HEADER moex-cli/*.h)

set(CLISOURCE_FILES
        ${LIBMOEX_SOURCE}
        ${LIBMOEX_HEADER}
        ${MOEX_CLI_SOURCE}
        ${MOEX_CLI_HEADER}
        )

include_directories(./)


# see https://cmake.org/cmake/help/latest/module/FindBoost.html
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.64 REQUIRED COMPONENTS
        program_options
        filesystem
        )
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    add_executable(moex-cli ${CLISOURCE_FILES})
    target_link_libraries(moex-cli ${Boost_LIBRARIES})
endif()
