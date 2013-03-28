QT += gui

INCLUDEPATH += $$PWD/../Documents \
               $$PWD/../ThumbnailManager \
               $$PWD/../Plugins/TstGenerator \
               $$PWD/../Plugins/TstDocGenerator1 \
               $$PWD/../PluginManager

LIBS += \
        -L$$PWD/../Documents -lDocuments \
        -L$$PWD/../Plugins/TstGenerator -lTstGenerator \
        -L$$PWD/../Plugins/TstDocGenerator1 -lTstDocGenerator1 \
        -L$$PWD/../PluginManager -lPluginManager

SOURCES += \
    main.cpp
