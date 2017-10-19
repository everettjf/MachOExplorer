#-------------------------------------------------
#
# Project created by QtCreator 2017-10-19T23:56:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlexSample
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += "/Users/everettjf/qt/5.9.1/clang_64/lib/QtCore.framework/Headers/"
INCLUDEPATH += "/Users/everettjf/qt/5.9.1/clang_64/lib/QtCore.framework/Headers/5.9.1/"
INCLUDEPATH += "/Users/everettjf/qt/5.9.1/clang_64/lib/QtCore.framework/Headers/5.9.1/QtCore/"
INCLUDEPATH += "/Users/everettjf/qt/5.9.1/clang_64/lib/QtGui.framework/Headers/"
INCLUDEPATH += "/Users/everettjf/qt/5.9.1/clang_64/lib/QtGui.framework/Headers/5.9.1/"
INCLUDEPATH += "/Users/everettjf/qt/5.9.1/clang_64/lib/QtGui.framework/Headers/5.9.1/QtGui/"
INCLUDEPATH += "/Users/everettjf/qt/5.9.1/clang_64/lib/QtWidgets.framework/Headers/"
INCLUDEPATH += "/Users/everettjf/qt/5.9.1/clang_64/lib/QtWidgets.framework/Headers/5.9.1/"
INCLUDEPATH += "/Users/everettjf/qt/5.9.1/clang_64/lib/QtWidgets.framework/Headers/5.9.1/QtWidgets/"


LIBMOEX_HEADER = $$files(libmoex/*.h,true)
LIBMOEX_SOURCE = $$files(libmoex/*.cpp,true)
MOEX_HEADER = $$files(*.h,true)
MOEX_SOURCE = $$files(*.cpp,true)

SOURCES += $$LIBMOEX_SOURCE $$MOEX_SOURCE
HEADERS  += $$LIBMOEX_HEADER $$MOEX_HEADER
FORMS    +=
RESOURCES += \
    QtFlex.qrc
