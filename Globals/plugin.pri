include(../Cshnick_Qt_dynamicList.pri)

defineReplace(includeDependency) {
     return(../../$$1)
}

defineReplace(existingMetaFile) {
    exists($$1) {
        return($$1)
    }
    # check without "lib"
    D_PATH = $$dirname(1)
    F_NAME = $$basename(1)
    F_NAME ~=  s,^lib,,
    ADJUSTED_PATH = $${D_PATH}/$${F_NAME}
    exists($$ADJUSTED_PATH) {
        return($${ADJUSTED_PATH})
    }
    return("")
}

defineTest(post_copy_files) {
    DESTINATION_DIR = $${2}
    SOURCE_FILES = $${1}
    !exists($$DESTINATION_DIR) {
        return(false)
    }
    for(NEXT_FILE, SOURCE_FILES) {
        exists($$NEXT_FILE) {
            QMAKE_POST_LINK += $${QMAKE_COPY} $${NEXT_FILE} $${DESTINATION_DIR}
            message(postLinkAdded)
        }
    }

    export(QMAKE_POST_LINK)
    return(true)
}

DESTDIR = $$PLUGIN_PATH
LIBS *= -L$$DESTDIR


PLUGINS_SRC_DIR = $$TOP_LEVEL_SRC_DIR/Plugins
PINFO_PATH = $$PLUGINS_SRC_DIR/lib$${TARGET}.pinfo
PINFO_PATH = $$existingMetaFile($${PINFO_PATH})
!isEmpty(PINFO_PATH) {
    post_copy_files($$PINFO_PATH, $$PLUGIN_PATH)
}
!isEmpty() {
#    QMAKE_POST_LINK += $${QMAKE_COPY} /home/ilia/Documents/notes /home/ilia/Development/sources/App/own/Cshnick_Qt_dynamicList/build/plugins/notes1
    FLS = 1 2 3 4 5
    DEST = "/home/ilia"
    post_copy_files($${PINFO_PATH}, $$PLUGIN_PATH)
    message(PINFO_FILE $$PINFO_PATH)
}

macx {
    !isEmpty(TIGER_COMPAT_MODE) {
        QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../PlugIns/
    } else {
        QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/PlugIns/
        QMAKE_LFLAGS += -Wl,-rpath,@loader_path/../../,-rpath,@executable_path/../
    }
} else:linux-* {
    #do the rpath by hand since it's not possible to use ORIGIN in QMAKE_RPATHDIR
    QMAKE_RPATHDIR += \$\$ORIGIN
    QMAKE_RPATHDIR += \$\$ORIGIN/..
    QMAKE_RPATHDIR += \$\$ORIGIN/../..
    IDE_PLUGIN_RPATH = $$join(QMAKE_RPATHDIR, ":")
    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${IDE_PLUGIN_RPATH}\'
    QMAKE_RPATHDIR =
}

CONFIG += include_source_dir \
          plugin \
          plugin_with_soname
 
linux*:QMAKE_LFLAGS += $$QMAKE_LFLAGS_NOUNDEF

