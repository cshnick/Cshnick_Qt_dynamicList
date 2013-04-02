#-------------------------------------------------
#
# Project created by QtCreator 2013-03-28T15:03:11
#
#-------------------------------------------------

TARGET = PluginManager
TEMPLATE = lib

QT += xml

DEFINES += PLUGINMANAGER_LIBRARY

SOURCES += PluginManager.cpp \
    PInfoHandler.cpp

HEADERS += PluginManager.h\
        PluginManager_global.h \
    ICommonInterface.h \
    PInfoHandler.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

INCLUDEPATH +=  \
                ../Globals

SERVICE_DIR =  $$PWD/service
MOC_DIR = $$SERVICE_DIR
OBJECTS_DIR = $$SERVICE_DIR
