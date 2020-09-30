# Check if the config file exists
! include( ../../../common-config.pri ) {
    error( "Couldn't find the common-config.pri file!" )
}

#Include local paths
! include( ../local-config.pri ) {
    error( "Couldn't find the local-config.pri file!" )
}

logger_host.depends = $$processDependencies( MODULE_LOGGER_HOST )

# Turn LOG_USE_DLL for usage logger only if DLL will be present
LOG_USE_DLL=1

QT       -= gui core

TARGET = logger_host
TEMPLATE = lib
CONFIG += staticlib

# remove target file on clean
$$extraClean($$outputFileName())

contains(LOG_USE_DLL,1) {
  logger_dll_name=$$localLib(logger_dll)

  win32 {
    logger_dll_prefix=""
    logger_dll_ext=".dll"
  }

  unix:!macx {
    logger_dll_prefix="lib"
    logger_dll_ext=".so"
  }

  macx {
    logger_dll_prefix="lib"
    logger_dll_ext=".dylib"
  }

  DEFINES += LOG_USE_DLL=1 LOG_DLL_NAME="\\\"$${logger_dll_prefix}$$localLib(logger_dll)$${logger_dll_ext}\\\""
}

contains(LOG_USE_DLL,0) {
  DEFINES += LOG_USE_DLL=0 LOG_DLL_NAME=""
}

unix: LIBS += -ldl
android: DEFINES += LOG_PLATFORM_ANDROID=1

INCLUDEPATH += include

HEADERS_GLOBAL += \

HEADERS_GLOBAL_SHARED += \

HEADERS_LOCAL += \

HEADERS_LOCAL_SHARED_EXTERNAL += \
  include/log/logger.h

HEADERS_LOCAL_SHARED_INTERNAL += \

HEADERS += \
  $${HEADERS_GLOBAL} \
  $${HEADERS_GLOBAL_SHARED} \
  $${HEADERS_LOCAL} \
  $${HEADERS_LOCAL_SHARED_EXTERNAL} \
  $${HEADERS_LOCAL_SHARED_INTERNAL} \

SOURCES += \
    logger_load_dll.c \
    logger.cpp


DISTFILES += \
    module-interface.pri


logger_host_share_local_include.path = $${CMF_INSTALL_HOST_SRC_DIR}/logger_host/include/log
logger_host_share_local_include.files += $${HEADERS_LOCAL_SHARED_EXTERNAL}

logger_host_share_src.path = $${CMF_INSTALL_HOST_SRC_DIR}/logger_host
logger_host_share_src.files += $${HEADERS_LOCAL_SHARED_INTERNAL} $${SOURCES}

logger_host_share_lib.path = $${CMF_INSTALL_LIB_DIR}
logger_host_share_lib.CONFIG += no_check_exist

win32 {
  logger_host_share_lib.files += $$BUILD_DIR/$${TARGET}.lib
}

!android {
  INSTALLS += \
    logger_host_share_local_include \
    logger_host_share_src \
    logger_host_share_lib \
}

