#-------------------------------------------------
#
# Project created by QtCreator 2015-07-08T02:38:47
#
#-------------------------------------------------

QT       -= gui

TARGET = ServiceInterfaces
TEMPLATE = lib
CONFIG += staticlib c++11

SOURCES +=

HEADERS += \
    irewardsservice.h \
    ieligibilityservice.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

DESTDIR = $$PWD/../bin
