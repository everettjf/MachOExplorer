#-------------------------------------------------
#
# Project created by QtCreator 2017-04-05T23:30:56
#
#-------------------------------------------------

QT       += core gui

QT += widgets network

TARGET = MachOExplorer
TEMPLATE = app
ICON = MachOExplorer.icns

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


#INCLUDEPATH += "/opt/qt/clang_64/lib/QtCore.framework/Headers/"
#INCLUDEPATH += "/opt/qt/clang_64/lib/QtCore.framework/Headers/5.9.1/"
#INCLUDEPATH += "/opt/qt/clang_64/lib/QtCore.framework/Headers/5.9.1/QtCore/"
#INCLUDEPATH += "/opt/qt/clang_64/lib/QtGui.framework/Headers/"
#INCLUDEPATH += "/opt/qt/clang_64/lib/QtGui.framework/Headers/5.9.1/"
#INCLUDEPATH += "/opt/qt/clang_64/lib/QtGui.framework/Headers/5.9.1/QtGui/"
#INCLUDEPATH += "/opt/qt/clang_64/lib/QtWidgets.framework/Headers/"
#INCLUDEPATH += "/opt/qt/clang_64/lib/QtWidgets.framework/Headers/5.9.1/"
#INCLUDEPATH += "/opt/qt/clang_64/lib/QtWidgets.framework/Headers/5.9.1/QtWidgets/"


LIBMOEX_HEADER = $$files(libmoex/*.h,true)
LIBMOEX_SOURCE = $$files(libmoex/*.cpp,true)
MOEX_HEADER = $$files(src/*.h,true)
MOEX_SOURCE = $$files(src/*.cpp,true)

SOURCES += $$LIBMOEX_SOURCE $$MOEX_SOURCE
HEADERS  += $$LIBMOEX_HEADER $$MOEX_HEADER \
    src/controller/windowshareddata.h
FORMS    += \
    src/dialog/checkupdatedialog.ui \
    src/dialog/aboutdialog.ui

macx{
    _BOOSTPATH = /usr/local/Cellar/boost/1.65.1
    INCLUDEPATH += "$${_BOOSTPATH}/include/"
    LIBS += -L$${_BOOSTPATH}/lib
}

INCLUDEPATH += ./src/

RESOURCES += \
    MachOExplorer.qrc

