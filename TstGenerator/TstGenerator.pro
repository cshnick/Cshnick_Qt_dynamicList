#-------------------------------------------------
#
# Project created by QtCreator 2013-02-26T20:49:50
#
#-------------------------------------------------

INCLUDEPATH += $$PWD/../picturesmodelview
LIBS += -L$$PWD/../picturesmodelview -lpicturesmodelview

TARGET = TstGenerator
TEMPLATE = lib

DEFINES += TSTGENERATOR_LIBRARY

SOURCES += tstgenerator.cpp

HEADERS += tstgenerator.h\
        TstGenerator_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
