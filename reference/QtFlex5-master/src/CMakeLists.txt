CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11 FATAL_ERROR)

set(CMAKE_MACOSX_RPATH 1)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

ADD_DEFINITIONS(-DQT_FLEX_BUILD)

FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)

INCLUDE_DIRECTORIES(${Qt5Gui_PRIVATE_INCLUDE_DIRS} ${Qt5Widgets_PRIVATE_INCLUDE_DIRS})

SET(HEADER_FILES QtDockGuider.h
                 QtDockSide.h
                 QtDockSite.h
                 QtDockWidget.h
                 QtFlexHelper.h
                 QtFlexManager.h
                 QtFlexStyle.h
                 QtFlexWidget.h)

SET(SOURCE_FILES QtDockGuider.cpp
                 QtDockSide.cpp
                 QtDockSite.cpp
                 QtDockWidget.cpp
                 QtFlexHelper.cpp
                 QtFlexManager.cpp
                 QtFlexStyle.cpp
                 QtFlexWidget.cpp
                 QtFlex.qrc)

ADD_LIBRARY(Qt5Flex SHARED ${HEADER_FILES} ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(Qt5Flex Qt5::Core Qt5::Gui Qt5::Widgets)

SET_TARGET_PROPERTIES(Qt5Flex PROPERTIES DEBUG_POSTFIX d)

INSTALL(TARGETS Qt5Flex
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
