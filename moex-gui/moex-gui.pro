#-------------------------------------------------
#
# Project created by QtCreator 2017-04-05T23:30:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = moex-ui
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

PRECOMPILED_HEADER = stable.h

CONFIG   += precompile_header


SOURCES += moex-gui.cpp\
    utility/utility.cpp \
    view/mainwindow.cpp \
    model/placeholdermodel.cpp \
    controller/workspacemanager.cpp \
    view/skeletonview.cpp \
    view/contentview.cpp \
    view/logview.cpp \
    controller/workspace.cpp \
    controller/skeletoncontroller.cpp \
    controller/basecontroller.cpp \
    ../libmoex/util/singleton.cpp \
    ../libmoex/node/fatheader.cpp \
    ../libmoex/node/nodeview.cpp \
    ../libmoex/node/helper.cpp \
    view/content/binarycontentview.cpp \
    view/content/blankcontentview.cpp \
    view/content/tablecontentview.cpp \
    view/widget/hexeditwidget.cpp \
    controller/tablecontentcontroller.cpp

HEADERS  += \
    ../libmoex/node/fatheader.h \
    ../libmoex/node/loadcommand.h \
    ../libmoex/node/loadcommand_dyld_info.h \
    ../libmoex/node/loadcommand_dylib.h \
    ../libmoex/node/loadcommand_dysymtab.h \
    ../libmoex/node/loadcommand_encryption_info.h \
    ../libmoex/node/loadcommand_factory.h \
    ../libmoex/node/loadcommand_linkedit_data.h \
    ../libmoex/node/loadcommand_load_dylinker.h \
    ../libmoex/node/loadcommand_main.h \
    ../libmoex/node/loadcommand_segment.h \
    ../libmoex/node/loadcommand_source_version.h \
    ../libmoex/node/loadcommand_symtab.h \
    ../libmoex/node/loadcommand_uuid.h \
    ../libmoex/node/loadcommand_version_min.h \
    ../libmoex/node/machheader.h \
    ../libmoex/node/machsection.h \
    ../libmoex/node/magic.h \
    ../libmoex/node/node.h \
    ../libmoex/util/singleton.h \
    ../libmoex/binary.h \
    ../libmoex/node/common.h \
    ../libmoex/node/helper.h \
    ../libmoex/node/nodeview.h \
    utility/utility.h \
    view/mainwindow.h \
    model/placeholdermodel.h \
    controller/workspacemanager.h \
    view/skeletonview.h \
    view/contentview.h \
    view/logview.h \
    stable.h \
    controller/workspace.h \
    controller/skeletoncontroller.h \
    controller/basecontroller.h \
    view/content/binarycontentview.h \
    view/content/blankcontentview.h \
    view/content/tablecontentview.h \
    view/widget/hexeditwidget.h \
    controller/tablecontentcontroller.h

FORMS    +=


macx{
    _BOOSTPATH = /usr/local/Cellar/boost/1.64.0_1
    INCLUDEPATH += "$${_BOOSTPATH}/include/"
    LIBS += -L$${_BOOSTPATH}/lib
}

INCLUDEPATH += ./../

