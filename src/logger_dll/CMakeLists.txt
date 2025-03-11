
cmake_minimum_required(VERSION 3.10)

project(logger_dll VERSION 0.0.0.1 LANGUAGES C CXX)

set (SOURCE_FILES
  logger_dll_win.cc
  logger_dll.def
  "${CMAKE_CURRENT_SOURCE_DIR}/../logger_host/logger.cc"
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

add_compile_definitions(LOGGERDLL_LIBRARY=1 LOG_THIS_IS_DLL=1 LOG_DLL_NAME="logger_dll")

if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEF:${CMAKE_CURRENT_SOURCE_DIR}\\logger_dll.def")
  add_compile_options(/wd4250 /wd4437 /wd5045 /wd4548 /wd4710 /wd4711 /wd4371 /wd4996 /MT /Zi)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /MT /Zi")
endif()

add_library(logger_dll SHARED ${SOURCE_FILES})

if(MSVC)
  SET_TARGET_PROPERTIES(logger_dll PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}\\logger_dll.def")
endif()

set_source_files_properties(logger_dll.def PROPERTIES HEADER_FILE_ONLY TRUE)
install(TARGETS logger_dll DESTINATION "${PROJECT_OUTPUT_DIR}")
