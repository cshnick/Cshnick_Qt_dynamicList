#-------------------------------------------------
#
# Project created by QtCreator 2013-03-21T20:34:24
#
#-------------------------------------------------

TARGET = Documents
TEMPLATE = lib

include($$PWD/../ThumbnailManager/ThumbnailManager.pri)

SERVICE_DIR =  $$PWD/service
MOC_DIR = $$SERVICE_DIR
OBJECTS_DIR = $$SERVICE_DIR

DESTDIR = $$PWD

DEFINES += DOCUMENTS_LIBRARY

SOURCES += DocumentManager.cpp \
    ExplorerModel.cpp \
    Node.cpp \
    ExplorerView.cpp \
    IDocumentGenerator.cpp

QT += core gui

HEADERS += DocumentManager.h\
        DocumentManager_global.h \
    ExplorerModel.h \
    Node.h \
    ExplorerView.h \
    IDocumentGenerator.h

unix:!symbian {
   target.path = /usr/lib
   INSTALLS += target
}

INCLUDEPATH += \
               ../ThumbnailManager \
               ../PluginManager


LIBS += \
#        -L$$PWD/../TstDocGenerator1 -lTstDocGenerator1
