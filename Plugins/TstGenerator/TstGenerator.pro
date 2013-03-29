#-------------------------------------------------
#
# Project created by QtCreator 2013-02-26T20:49:50
#
#-------------------------------------------------

INCLUDEPATH += $$PWD/../../ThumbnailManager
LIBS += -L$$PWD/../../Documents -lDocuments

CONFIG += plugin

TARGET = TstGenerator
TEMPLATE = lib

SERVICE_DIR =  $$PWD/service
MOC_DIR = $$SERVICE_DIR
OBJECTS_DIR = $$SERVICE_DIR

DEFINES += TSTGENERATOR_LIBRARY

SOURCES += tstgenerator.cpp

HEADERS += tstgenerator.h\
        TstGenerator_global.h

unix:!symbian {
    target.path = /usr/lib
    INSTALLS += target
}
