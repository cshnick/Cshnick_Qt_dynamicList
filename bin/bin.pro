QT += gui

INCLUDEPATH += $$PWD/../DocumentManager \
               $$PWD/../picturesmodelview \
               $$PWD/../TstGenerator \
               $$PWD/../TstDocGenerator1


LIBS += \
        -L$$PWD/../DocumentManager -lDocumentManager \
        -L$$PWD/../picturesmodelview -lpicturesmodelview \
        -L$$PWD/../TstGenerator -lTstGenerator \
        -L$$PWD/../TstDocGenerator1 -lTstDocGenerator1

SOURCES += \
    main.cpp
