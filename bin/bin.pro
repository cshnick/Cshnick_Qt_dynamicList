QT += gui

INCLUDEPATH += $$PWD/../DocumentManager \
               $$PWD/../picturesmodelview \
               $$PWD/../TstGenerator


LIBS += -L$$PWD/../DocumentManager -lDocumentManager \
        -L$$PWD/../picturesmodelview -lpicturesmodelview \
        -L$$PWD/../TstGenerator -lTstGenerator

SOURCES += \
    main.cpp
