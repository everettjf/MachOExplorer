cmake_minimum_required(VERSION 3.6)
project(moex)

set(CMAKE_CXX_STANDARD 11)
file(GLOB_RECURSE libMOEXSRC libmoex/*.cpp)
file(GLOB_RECURSE libMOEXHEADERS libmoex/*.h)
file(GLOB_RECURSE GUIMOEXSRC moex-ui/*.cpp)
file(GLOB_RECURSE GUIMOEXHEADERS moex-ui/*.h)
set(CLISOURCE_FILES
        ${libMOEXSRC}
        ${libMOEXHEADERS}
        moex-cli/moex-cli.cpp)
set(GUISOURCE_FILES
                ${libMOEXSRC}
                ${libMOEXHEADERS}
                ${GUIMOEXSRC}
                ${GUIMOEXHEADERS}
                )

include_directories(./)

add_executable(moex-cli ${CLISOURCE_FILES})
add_executable(moex-gui ${GUISOURCE_FILES})

# see https://cmake.org/cmake/help/latest/module/FindBoost.html
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(moex-cli ${Boost_LIBRARIES})
