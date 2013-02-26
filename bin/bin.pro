QT += gui

INCLUDEPATH += $$PWD/../picturesmodelview \
               $$PWD/../TstGenerator

LIBS += -L$$PWD/../picturesmodelview -lpicturesmodelview \
        -L$$PWD/../TstGenerator -lTstGenerator

SOURCES += \
    main.cpp
