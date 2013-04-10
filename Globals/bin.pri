include(../Cshnick_Qt_dynamicList.pri)

defineReplace(includeDependency) {
     return(../$$1)
}

DESTDIR = $$EXEC_PATH

include(rpath.pri)

INCLUDEPATH *= $$OUT_PWD