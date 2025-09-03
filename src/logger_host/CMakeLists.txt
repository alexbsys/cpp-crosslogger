
cmake_minimum_required(VERSION 3.10)

project(logger_host VERSION 0.0.0.1 LANGUAGES C CXX)

set (SOURCE_FILES
  logger_load_dll.c
  logger.cc)

set (LOGGER_LIB_HEADER_FILES
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_c.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_cfg.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_cfgcheck.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_config.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_get_caller_addr.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_interfaces.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_pdefs.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_pdetect.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_singleton.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_stream.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_sysinclib.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_userdefs.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_varargs.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_verbose.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/logger_shared_ptr.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/win32_dll_entrypoint.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/api/logger_user_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/api/logger_user_plugin_api.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_cfgfn.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_impl.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_ini_parser.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_machine_id.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_moddef.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_mt.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_plugin_manager.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_runtime_debugging.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_shmem.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_strutils.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_sysinfo.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_utils.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_win_registry.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/detail/logger_xprintf.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_binary_command_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_colored_console_output_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_crashhandler_command_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_example_command_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_exception_command_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_file_upload_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_ini_config_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_modules_command_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_objmon_command_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_pluginmodule_command_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_register_builtin_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_scroll_file_output_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_stacktrace_command_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_win_config_macro_plugin.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../include/log/plugins/logger_win_registry_config_plugin.h"
)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT /Zi")
#  add_compile_options(/wd4250 /wd4437 /wd5045 /wd4548 /wd4710 /wd4711 /wd4371 /wd4996 /MT /Zi)
endif()

add_library(logger_host STATIC ${SOURCE_FILES} ${LOGGER_LIB_HEADER_FILES})
  
target_include_directories(logger_host PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
					
target_include_directories(logger_host PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
				
target_compile_definitions(logger_host PUBLIC 
  LOG_USE_DLL=1 
  $<$<PLATFORM_ID:Windows>:LOG_DLL_NAME="logger_dll.dll">
  $<$<PLATFORM_ID:Linux>:LOG_DLL_NAME="logger_dll.so">
  $<$<PLATFORM_ID:Darwin>:LOG_DLL_NAME="logger_dll.dylib">
)

install(TARGETS logger_host DESTINATION "${PROJECT_OUTPUT_DIR}")
