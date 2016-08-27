#-------------------------------------------------
#
# Project created by QtCreator 2015-07-08T02:05:38
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = rewardsservicetests
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += rewardsservicetests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += $$PWD/../ThirdParty/gmock-1.7.0/fused-src/gmock-gtest-all.cc
INCLUDEPATH += $$PWD/../ThirdParty/gmock-1.7.0/fused-src
INCLUDEPATH += $$PWD/../ThirdParty/CRZ
INCLUDEPATH += $$PWD/../ServiceInterfaces

unix:!macx: LIBS += -L$$PWD/../bin/ -lRewardsService

INCLUDEPATH += $$PWD/../RewardsService
DEPENDPATH += $$PWD/../RewardsService

unix:!macx: PRE_TARGETDEPS += $$PWD/../bin/libRewardsService.a

DESTDIR = $$PWD/../bin
