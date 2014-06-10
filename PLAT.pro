#-------------------------------------------------
#
# Project created by QtCreator 2014-06-09T17:24:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PLAT
TEMPLATE = app


SOURCES += main.cpp\
        platmainw.cpp

HEADERS  += platmainw.h

FORMS    += platmainw.ui


INCLUDEPATH += $$PWD

win32:QMAKE_CXXFLAGS += /MD
win32:QMAKE_CXXFLAGS += /MT

#osip2
win32: LIBS += -L$$PWD/osip2_Dlib/ -losip2
INCLUDEPATH += $$PWD/osip2
DEPENDPATH += $$PWD/osip2
win32: PRE_TARGETDEPS += $$PWD/osip2_Dlib/osip2.lib
unix:LIBS += -losip2

#osipparser2
win32: LIBS += -L$$PWD/osip2_Dlib/ -losipparser2
INCLUDEPATH += $$PWD/osipparser2
DEPENDPATH += $$PWD/osipparser2
win32: PRE_TARGETDEPS += $$PWD/osip2_Dlib/osipparser2.lib
unix:LIBS += -losipparser2

#exosip
win32: LIBS += -L$$PWD/exosip2_lib/ -leXosip
INCLUDEPATH += $$PWD/eXosip2
DEPENDPATH += $$PWD/eXosip2
win32: PRE_TARGETDEPS += $$PWD/exosip2_lib/eXosip.lib
unix:LIBS += -leXosip2

#win32 extlibs
win32:LIBS += -lws2_32
win32:LIBS += -lDnsAPI
win32:LIBS += -lIPHlpApi

DEFINES += QT_5
