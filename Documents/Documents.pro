TARGET = Documents
TEMPLATE = lib

DEFINES += DOCUMENTS_LIBRARY

include($$PWD/../ThumbnailManager/ThumbnailManager.pri)
include($$PWD/../Globals/shared_lib.pri)

SOURCES += DocumentManager.cpp \
           ExplorerModel.cpp \
           Node.cpp \
           ExplorerView.cpp \
           IDocumentGenerator.cpp

HEADERS += DocumentManager.h\
        DocumentManager_global.h \
        ExplorerModel.h \
        Node.h \
        ExplorerView.h \
        IDocumentGenerator.h

INCLUDEPATH += $$includeDependency(Globals) \
               $$includeDependency(PluginManager) \
               $$includeDependency(ThumbnailManager)

LIBS += $$libraryDependency(PluginManager)


