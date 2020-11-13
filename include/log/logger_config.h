

#ifndef LOGGER_CONFIG_HEADER
#define LOGGER_CONFIG_HEADER

//////////////////////////////////////////////////////////////
//////////////  CONFIGURATION SECTION BEGIN  /////////////////

/// USER CONFIGURATION

/// Turn Logger on or off
#ifndef LOG_ENABLED
#define LOG_ENABLED 1
#endif  // LOG_ENABLED

/// Enable Logging only in debug version. In release it will be turned off
/// Otherwise it will be enabled in both release and debug versions
#ifndef LOG_ONLY_DEBUG
#define LOG_ONLY_DEBUG 0
#endif  // LOG_ONLY_DEBUG

/// Show system info on start session
#ifndef LOG_USE_SYSTEMINFO
#define LOG_USE_SYSTEMINFO 1
#endif  // LOG_USE_SYSTEMINFO

/// Use module definition system. It need for macros '$(module)' works, modules dumping
/// and auto-debugging If auto debugging is on it turns on automatically On linux you need
/// to define linker flag -ldl
#ifndef LOG_USE_MODULEDEFINITION
#ifndef __APPLE__
#define LOG_USE_MODULEDEFINITION 1
#else //__APPLE__
#define LOG_USE_MODULEDEFINITION 0
#endif //__APPLE__
#endif  // LOG_USE_MODULEDEFINITION

/// Use auto debugging. It need for stack trace
/// On linux it is strongly recommended to build with -rdynamic linker key
#ifndef LOG_AUTO_DEBUGGING
#define LOG_AUTO_DEBUGGING 1
#endif  // LOG_AUTO_DEBUGGING

/// Catch unhandled exceptions, Logging them and creating mini dumps
/// If auto debugging enabled it show stack trace
#ifndef LOG_UNHANDLED_EXCEPTIONS
#define LOG_UNHANDLED_EXCEPTIONS 1
#endif  // LOG_UNHANDLED_EXCEPTIONS

/// Configure Log from registry. Registry parameters are more priority that configuration
/// from program Registry path must be configured before first access to Log
#ifndef LOG_CONFIGURE_FROM_REGISTRY
#define LOG_CONFIGURE_FROM_REGISTRY 0
#endif  // LOG_CONFIGURE_FROM_REGISTRY

/// Enable configuration from INI file
#ifndef LOG_INI_CONFIGURATION
#define LOG_INI_CONFIGURATION 1
#endif  // LOG_INI_CONFIGURATION

/// Paths which will be used for find INI file. First defined paths are more prioritized
#ifndef LOG_DEFAULT_INI_PATHS
#define LOG_DEFAULT_INI_PATHS                                                    \
  "$(EXEDIR)/$(EXEFILENAME).log.ini;$(WINDOWSPATH)/../$(EXEFILENAME)/"           \
  "$(EXEFILENAME).log.ini;$(MODULEDIR)/$(MODULEFILENAME).log.ini;$(CURRENTDIR)/" \
  "$(EXEFILENAME).log.ini;$(CURRENTDIR)/$(MODULEFILENAME).log.ini"
#endif  // LOG_DEFAULT_INI_PATHS

#ifndef LOG_REGISTRY_DEFAULT_KEY
#define LOG_REGISTRY_DEFAULT_KEY "HKCU\\Software\\$(EXEFILENAME)\\Logging"
#endif  // LOG_REGISTRY_DEFAULT_KEY

/// Enable using RTTI in Log
#ifndef LOG_RTTI_ENABLED
#define LOG_RTTI_ENABLED 1
#endif  // LOG_RTTI_ENABLED

/// Use one shared logging object for process
#ifndef LOG_SHARED
#define LOG_SHARED 1
#endif  // LOG_SHARED

/// Enable or disable logger configuration compiler warnings
#ifndef LOG_COMPILER_WARNINGS
#define LOG_COMPILER_WARNINGS 1
#endif  // LOG_COMPILER_WARNINGS

/// Get logging module code from external DLL
#ifndef LOG_USE_DLL
#define LOG_USE_DLL 1
#endif  // LOG_USE_DLL

#ifndef LOG_MULTITHREADED
#define LOG_MULTITHREADED 1
#endif  // LOG_MULTITHREADED

#ifndef LOG_USE_OBJMON
#define LOG_USE_OBJMON 1
#endif  // LOG_USE_OBJMON

#ifdef LOG_PLATFORM_ANDROID
#ifndef LOG_ANDROID_SYSLOG
#define LOG_ANDROID_SYSLOG 1
#endif  // LOG_ANDROID_SYSLOG
#endif  // LOG_PLATFORM_ANDROID

#ifdef LOG_PLATFORM_ANDROID
#ifndef LOG_ANDROID_DEFAULT_LOG_PATH
#define LOG_ANDROID_DEFAULT_LOG_PATH "/data/local/tmp"
#endif //LOG_ANDROID_DEFAULT_LOG_PATH
#endif // LOG_PLATFORM_ANDROID

// Include CONFIG.H for platform detection (for posix-based systems)
// need to detect: unistd.h sys/syscall.h sys/utsname.h pwd.h pthread sys/mman.h
// WARNING: if some of this dependencies is not specified in autoconf, turn
// LOG_USE_CONFIG_H to 0!
#ifndef LOG_USE_CONFIG_H
#define LOG_USE_CONFIG_H 0
#endif  // LOG_USE_CONFIG_H

//////////////////////////////////////////////////////////////
/// THIN CONFIGURATION
/// DOES NOT NEED TO MODIFICATION IN MOST CASES

/// Use own vsnprintf implementation instead system one. Own implementation is not so fast and smart like system.
/// But sometimes system implementation has buffer length restrictions
#ifndef LOG_USE_OWN_VSNPRINTF
#define LOG_USE_OWN_VSNPRINTF 1

// Replace \n -> \r\n for stupid terminals (1 - enabled, 0 - disabled)
#define LOG_OWN_VSNPRINTF_CR_CRLF 0
#endif //LOG_USE_OWN_VSNPRINTF

/// Open and close file at each write. Causes more stable writes on crashes but can
/// produce performance issue
#ifndef LOG_FLUSH_FILE_EVERY_WRITE
#define LOG_FLUSH_FILE_EVERY_WRITE 0
#endif  // LOG_FLUSH_FILE_EVERY_WRITE

/// Use debug macro and mechanisms in log
#ifndef LOG_CHECKED
#define LOG_CHECKED 1
#endif  // LOG_CHECKED

/// Show message on fatal application crash and do not show window 'this program cause
/// error... send report...'. Do not recommended: logger will be injected to business
/// logic of app crash workflow
#ifndef LOG_SHOW_MESSAGE_ON_FATAL_CRASH
#define LOG_SHOW_MESSAGE_ON_FATAL_CRASH 1
#endif  // LOG_SHOW_MESSAGE_ON_FATAL_CRASH

/// Detection module name by instruction pointer in stacktrace, if it was not found in
/// stack frames Does not recommended, it is really slow and does not need most cases
#ifndef LOG_STACKTRACE_DETECT_MODNAME_IF_NOT_FOUND
#define LOG_STACKTRACE_DETECT_MODNAME_IF_NOT_FOUND 0
#endif  // LOG_STACKTRACE_DETECT_MODNAME_IF_NOT_FOUND

/// Release logger after dump creation before application will crash. Used only if
/// LOG_UNHANDLED_EXCEPTIONS was set. Set this value to 0 can cause log file flush issues
/// but may be useful if you using debugger AFTER crash
#ifndef LOG_RELEASE_ON_APP_CRASH
#define LOG_RELEASE_ON_APP_CRASH 0
#endif  // LOG_RELEASE_ON_APP_CRASH

/// Use modules cache for detect module name by address. Used only if
/// LOG_USE_MODULEDEFINITION. Cache is optimizing performance but it is not support
/// modules unload. If you write system-trick tool or application which very often
/// load-unload DLLs, maybe you need to turn off modules cache
#ifndef LOG_USE_MODULES_CACHE
#define LOG_USE_MODULES_CACHE 1
#endif  // LOG_RELEASE_ON_APP_CRASH

/// Cache headers macro. Turned on by default. Turn off if you have problems with macro
/// header format
#ifndef LOG_USE_MACRO_HEADER_CACHE
#define LOG_USE_MACRO_HEADER_CACHE 1
#endif  // LOG_USE_MACRO_HEADER_CACHE

/// Process macro in logging text same as in header. Cause performance reducing
#ifndef LOG_PROCESS_MACRO_IN_LOG_TEXT
#define LOG_PROCESS_MACRO_IN_LOG_TEXT 0
#endif  // LOG_PROCESS_MACRO_IN_LOG_TEXT

/// Log works but does not create or write to file. Used only for performance analysis, do
/// not use in real applications!
#ifndef LOG_TEST_DO_NOT_WRITE_FILE
#define LOG_TEST_DO_NOT_WRITE_FILE 0
#endif  // LOG_TEST_DO_NOT_WRITE_FILE

/// Use SEH. Windows only
#ifndef LOG_USE_SEH
#define LOG_USE_SEH 1
#endif  // LOG_USE_SEH


/// Multithreaded logger overflow messages and behavior
#if LOG_MULTITHREADED
///    Message buffer max depth (overflow control)
#define LOG_MULTITHREADED_MAX_BUFFERED_MESSAGES    1024
///    Behavior on overflow
///       Stop caller thread until log process all buffered messages
#define LOG_MULTITHREADED_STOP_CALLER_ON_OVERFLOW  1
#endif /*LOG_MULTITHREADED*/


#if LOG_ONLY_DEBUG
#ifdef DEBUG
#undef LOG_ENABLED
#define LOG_ENABLED 0
#endif  // DEBUG
#endif  // LOG_ONLY_DEBUG


////////////////  CONFIGURATION SECTION END  /////////////////
//////////////////////////////////////////////////////////////


#endif /*LOGGER_CONFIG_HEADER*/