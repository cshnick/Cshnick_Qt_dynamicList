#-------------------------------------------------
#
# Project created by QtCreator 2013-03-21T20:34:24
#
#-------------------------------------------------

TARGET = DocumentManager
TEMPLATE = lib

DEFINES += DOCUMENTMANAGER_LIBRARY

SOURCES += DocumentManager.cpp \
    ExplorerModel.cpp \
    Node.cpp \
    ExplorerView.cpp

HEADERS += DocumentManager.h\
        DocumentManager_global.h \
    ExplorerModel.h \
    Node.h \
    ExplorerView.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
