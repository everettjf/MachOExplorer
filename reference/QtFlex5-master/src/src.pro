#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T14:31:53
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(debug, debug|release) {
    TARGET = Qt5Flexd
} else {
    TARGET = Qt5Flex
}

TEMPLATE = lib

QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++

DEFINES += QT_FLEX_BUILD

INCLUDEPATH += "/opt/qt/clang_64/lib/QtCore.framework/Headers/"
INCLUDEPATH += "/opt/qt/clang_64/lib/QtCore.framework/Headers/5.9.1/"
INCLUDEPATH += "/opt/qt/clang_64/lib/QtCore.framework/Headers/5.9.1/QtCore/"
INCLUDEPATH += "/opt/qt/clang_64/lib/QtGui.framework/Headers/"
INCLUDEPATH += "/opt/qt/clang_64/lib/QtGui.framework/Headers/5.9.1/"
INCLUDEPATH += "/opt/qt/clang_64/lib/QtGui.framework/Headers/5.9.1/QtGui/"
INCLUDEPATH += "/opt/qt/clang_64/lib/QtWidgets.framework/Headers/"
INCLUDEPATH += "/opt/qt/clang_64/lib/QtWidgets.framework/Headers/5.9.1/"
INCLUDEPATH += "/opt/qt/clang_64/lib/QtWidgets.framework/Headers/5.9.1/QtWidgets/"

SOURCES += \
    QtDockGuider.cpp \
    QtDockSide.cpp \
    QtDockSite.cpp \
    QtDockWidget.cpp \
    QtFlexHelper.cpp \
    QtFlexManager.cpp \
    QtFlexStyle.cpp \
    QtFlexWidget.cpp

HEADERS += \
    QtDockGuider.h \
    QtDockSide.h \
    QtDockSite.h \
    QtDockWidget.h \
    QtFlexHelper.h \
    QtFlexManager.h \
    QtFlexStyle.h \
    QtFlexWidget.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    QtFlex.qrc
