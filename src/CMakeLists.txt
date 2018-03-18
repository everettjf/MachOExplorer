cmake_minimum_required(VERSION 3.9)
project(MachOExplorer)

set(Qt5Core_DIR /opt/qt/clang_64/lib/cmake/Qt5Core)
set(Qt5Gui_DIR /opt/qt/clang_64/lib/cmake/Qt5Gui)
set(Qt5Widgets_DIR /opt/qt/clang_64/lib/cmake/Qt5Widgets)
set(Qt5Network_DIR /opt/qt/clang_64/lib/cmake/Qt5Network)

set(CMAKE_CXX_STANDARD 14)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)

include_directories(
        ${Qt5Widgets_INCLUDES}
        ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
        ${Qt5Network_INCLUDE_DIRS}
)

file(GLOB_RECURSE LIBMOEX_SOURCE libmoex/*.cpp)
file(GLOB_RECURSE LIBMOEX_HEADER libmoex/*.h)
file(GLOB_RECURSE MOEX_SOURCE src/*.cpp)
file(GLOB_RECURSE MOEX_HEADER src/*.h)

set(SOURCE_FILES
        ${LIBMOEX_SOURCE}
        ${LIBMOEX_HEADER}
        ${MOEX_SOURCE}
        ${MOEX_HEADER}
        MachOExplorer.qrc)


include_directories(./src)
add_executable(MachOExplorer ${SOURCE_FILES})
target_link_libraries(MachOExplorer Qt5::Widgets Qt5::Gui Qt5::Network)
