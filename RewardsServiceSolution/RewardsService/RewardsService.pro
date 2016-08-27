#-------------------------------------------------
#
# Project created by QtCreator 2015-07-08T02:51:12
#
#-------------------------------------------------

QT       -= gui

TARGET = RewardsService
TEMPLATE = lib
CONFIG += staticlib c++11

DEFINES += REWARDSSERVICE_LIBRARY

SOURCES += \
    rewardsserviceimpl.cpp

HEADERS +=\
    rewardsserviceimpl.h \
    rewardsserviceimpl_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += $$PWD/../ServiceInterfaces
DESTDIR = $$PWD/../bin
