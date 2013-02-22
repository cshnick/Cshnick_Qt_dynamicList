#-------------------------------------------------
#
# Project created by QtCreator 2013-02-22T12:17:53
#
#-------------------------------------------------

TARGET = picturesmodelview
TEMPLATE = lib

DEFINES += PICTURESMODELVIEW_LIBRARY

SOURCES += dynpicturesmodel.cpp

HEADERS += dynpicturesmodel.h\
        picturesmodelview_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
