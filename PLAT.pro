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
        platmainw.cpp \
    videoview.cpp \
    videofiltersmodel.cpp \
    sipevtthr.cpp \
    settings.cpp \
    ptzinfo.cpp \
    xmlmsgwriter.cpp

HEADERS  += platmainw.h \
    videoview.h \
    videofiltersmodel.h \
    sipevtthr.h \
    settings.h \
    ptzinfo.h \
    xmlmsgwriter.h \
    ptzdefines.h

FORMS    += platmainw.ui


INCLUDEPATH += $$PWD
unix:INCLUDEPATH += /usr/local/include
unix:DEPENDPATH += /usr/local/lib

win32:CONFIG(debug, debug|release) {
    message("debug")
    win32:QMAKE_CXXFLAGS += /MDd
    win32:QMAKE_CXXFLAGS += /D _CRT_SECURE_NO_DEPRECATE

    #osip2
    win32: LIBS += -L$$PWD/osip2_Dlib/ -losip2
    INCLUDEPATH += $$PWD/osip2
    DEPENDPATH += $$PWD/osip2
    win32: PRE_TARGETDEPS += $$PWD/osip2_Dlib/osip2.lib

    #osipparser2
    win32: LIBS += -L$$PWD/osip2_Dlib/ -losipparser2
    INCLUDEPATH += $$PWD/osipparser2
    DEPENDPATH += $$PWD/osipparser2
    win32: PRE_TARGETDEPS += $$PWD/osip2_Dlib/osipparser2.lib

    #exosip
    win32: LIBS += -L$$PWD/exosip2_Dlib/ -leXosip
    INCLUDEPATH += $$PWD/eXosip2
    DEPENDPATH += $$PWD/eXosip2
    win32: PRE_TARGETDEPS += $$PWD/exosip2_Dlib/eXosip.lib

} else {
    message("release")
    win32:QMAKE_CXXFLAGS += /MD
    win32:QMAKE_CXXFLAGS += /D _CRT_SECURE_NO_DEPRECATE
    #osip2
    win32: LIBS += -L$$PWD/osip2_Rlib/ -losip2
    INCLUDEPATH += $$PWD/osip2
    DEPENDPATH += $$PWD/osip2
    win32: PRE_TARGETDEPS += $$PWD/osip2_Rlib/osip2.lib
    #osipparser2
    win32: LIBS += -L$$PWD/osip2_Rlib/ -losipparser2
    INCLUDEPATH += $$PWD/osipparser2
    DEPENDPATH += $$PWD/osipparser2
    win32: PRE_TARGETDEPS += $$PWD/osip2_Rlib/osipparser2.lib
    #exosip
    win32: LIBS += -L$$PWD/exosip2_Rlib/ -leXosip
    INCLUDEPATH += $$PWD/eXosip2
    DEPENDPATH += $$PWD/eXosip2
    win32: PRE_TARGETDEPS += $$PWD/exosip2_Rlib/eXosip.lib
}

unix:LIBS += -L/usr/local/lib -losip2
unix:LIBS += -L/usr/local/lib -losipparser2
unix:LIBS += -L/usr/local/lib -leXosip2


#win32 extlibs
win32:LIBS += -lws2_32
win32:LIBS += -lDnsAPI
win32:LIBS += -lIPHlpApi


#vlc for windows
win32: LIBS += -L$$PWD/vlc_lib/ -llibvlc
win32:INCLUDEPATH += $$PWD/vlc_include
win32:DEPENDPATH += $$PWD/vlc_include
win32: PRE_TARGETDEPS += $$PWD/vlc_lib/libvlc.lib

win32: LIBS += -L$$PWD/vlc_lib/ -llibvlccore
win32:INCLUDEPATH += $$PWD/vlc_include
win32:DEPENDPATH += $$PWD/vlc_include
win32: PRE_TARGETDEPS += $$PWD/vlc_lib/libvlccore.lib
#vlc for linux
unix:LIBS += -lvlc

RESOURCES += \
    res.qrc

DEFINES += QT_5
DEFINES += ENABLE_TRACE

OTHER_FILES += \
    plat.ini \
    tmplog.txt

win32:RC_ICONS += $$PWD/icons/app_icon.ico

