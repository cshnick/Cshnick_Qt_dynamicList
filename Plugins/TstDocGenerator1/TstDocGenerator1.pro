#-------------------------------------------------
#
# Project created by QtCreator 2013-03-26T10:01:20
#
#-------------------------------------------------

TARGET = TstDocGenerator1
TEMPLATE = lib
CONFIG += plugin

LIB_ALIAS = lib

INCLUDEPATH += $$PWD/../../Documents \
               $$PWD/../../ThumbnailManager \
               $$PWD/../../TstGenerator \
               $$PWD/../../PluginManager


LIBS += -L$$PWD/../../Documents -lDocuments \

SERVICE_DIR =  $$PWD/service
MOC_DIR = $$SERVICE_DIR
OBJECTS_DIR = $$SERVICE_DIR

DEFINES += TSTDOCGENERATOR1_LIBRARY

QT += xml

SOURCES += tstdocgenerator1.cpp

HEADERS += tstdocgenerator1.h\
        TstDocGenerator1_global.h

unix:!symbian {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    res.qrc
