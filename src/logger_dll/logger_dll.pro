# Check if the config file exists
! include( ../../../common-config.pri ) {
    error( "Couldn't find the common-config.pri file!" )
}

#Include local paths
! include( ../local-config.pri ) {
    error( "Couldn't find the local-config.pri file!" )
}

# Check dependent modules
logger_dll.depends = $$processDependencies( MODULE_LOGGER_DLL )

QT       -= core gui
TARGET = logger_dll
TEMPLATE = lib
CONFIG += no_batch unversioned_libname

# remove target file on clean
$$extraClean($$outputFileName())

win32 {
  $$extraClean($${TARGET}.lib)
  $$extraClean($${TARGET}.pdb)

  DEF_FILE = logger_dll.def
  QMAKE_LFLAGS+="/DEF:$$_PRO_FILE_PWD_/$${DEF_FILE} /IMPLIB:$$DESTDIR/$${TARGET}.lib"
} else {
  LIBS += -L/usr/local/lib -ldl
}

android: LIBS += -llog

DEFINES += LOGGERDLL_LIBRARY LOG_THIS_IS_DLL=1 LOG_DLL_NAME="$$localLib(logger_dll)"

SOURCES += \
  ../logger_host/logger.cpp

win32 {
  SOURCES += logger_dll_win.cpp
  DISTFILES += $${DEF_FILE}
}

HEADERS +=

DISTFILES += module-interface.pri

logger_dll_share_bin.path = $${CMF_INSTALL_BIN_DIR}
logger_dll_share_bin.CONFIG += no_check_exist
win32 {
  logger_dll_share_bin.files += $$BUILD_DIR/$$outputFileName()
  logger_dll_share_bin.files += $$BUILD_DIR/$${TARGET}.pdb
}

unix:!android {
  logger_dll_share_bin.extra += cp -P $$BUILD_DIR/$$installUnixLibPattern($${TARGET}) $${CMF_INSTALL_BIN_DIR}
}

logger_dll_share_lib.path = $${CMF_INSTALL_LIB_DIR}
logger_dll_share_lib.CONFIG += no_check_exist
win32 {
  logger_dll_share_lib.files += \
    $${TARGET}.def \
    $$BUILD_DIR/$${TARGET}.lib
}

android {
  # Old qt libraries have problem with std c++
  !versionAtLeast(QT_VERSION, 5.14.0) {
    !versionAtMost(QT_VERSION, 5.11.0) {
      QMAKE_LFLAGS += -nostdlib++
    }
  }
}

!android {
  INSTALLS += \
    logger_dll_share_bin \
    logger_dll_share_lib \
}
