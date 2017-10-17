#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T14:13:28
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtGuider

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++

SOURCES += \
    ./main.cpp \
    ./QtGuider.cpp

HEADERS  += \
    ./QtGuider.h

FORMS    += \
    ./QtGuider.ui

RESOURCES += \
    ./QtGuider.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../src/release/ -lQt5Flex
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../src/debug/ -lQt5Flexd
else:unix:CONFIG(release,debug|release): LIBS += -L$$OUT_PWD/../../src/ -lQt5Flex
else:unix:CONFIG(debug,debug|release): LIBS += -L$$OUT_PWD/../../src/ -lQt5Flexd

INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src
