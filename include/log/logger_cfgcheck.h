
#ifndef LOGGER_CONFIGURATION_CHECK_HEADER
#define LOGGER_CONFIGURATION_CHECK_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"

////////////////////////////// Precompiled configuration checking START //////////////////////////////
/// Some features are not supported by different platform, or logger configuration parameters may depends 
/// on other parameters. This section is checking configuration

#ifdef LOG_PLATFORM_CYGWIN

#if LOG_UNHANDLED_EXCEPTIONS
#if LOG_COMPILER_WARNINGS

#ifdef LOG_COMPILER_MSVC
#pragma message("LOGGER: CYGWIN is not support LOG_UNHANDLED_EXCEPTIONS")
#else  // LOG_COMPILER_MSVC
#warning("LOGGER: CYGWIN is not support LOG_UNHANDLED_EXCEPTIONS")
#endif  // LOG_COMPILER_MSVC

#endif  // LOG_COMPILER_WARNINGS

#undef LOG_UNHANDLED_EXCEPTIONS
#define LOG_UNHANDLED_EXCEPTIONS 0
#endif  // LOG_UNHANDLED_EXCEPTIONS

#if LOG_AUTO_DEBUGGING
#if LOG_COMPILER_WARNINGS

#ifdef LOG_COMPILER_MSVC
#pragma message("LOGGER: CYGWIN is not support LOG_AUTO_DEBUGGING")
#else  // LOG_COMPILER_MSVC
#warning("LOGGER: CYGWIN is not support LOG_AUTO_DEBUGGING")
#endif  // LOG_COMPILER_MSVC

#endif  // LOG_COMPILER_WARNINGS

#undef LOG_AUTO_DEBUGGING
#define LOG_AUTO_DEBUGGING 0
#endif  // LOG_AUTO_DEBUGGING

#if LOG_USE_MODULEDEFINITION
#if LOG_COMPILER_WARNINGS

#ifdef LOG_COMPILER_MSVC
#pragma message("LOGGER: CYGWIN is not support LOG_USE_MODULEDEFINITION")
#else  // LOG_COMPILER_MSVC
#warning("LOGGER: CYGWIN is not support LOG_USE_MODULEDEFINITION")
#endif  // LOG_COMPILER_MSVC

#endif  // LOG_COMPILER_WARNINGS

#undef LOG_USE_MODULEDEFINITION
#define LOG_USE_MODULEDEFINITION 0
#endif  // LOG_USE_MODULEDEFINITION

#endif  // LOG_PLATFORM_CYGWIN

#if LOG_SHARED && defined(LOG_PLATFORM_POSIX_BASED) && !defined(LOG_HAVE_SYS_MMAN_H)
#if LOG_COMPILER_WARNINGS

#ifdef LOG_COMPILER_MSVC
#pragma message("LOGGER: System has no sys/mman.h - logger cannot be shared (LOG_SHARED)")
#else  // LOG_COMPILER_MSVC
#warning("LOGGER: System has no sys/mman.h - logger cannot be shared (LOG_SHARED)")
#endif  // LOG_COMPILER_MSVC

#endif  // LOG_COMPILER_WARNINGS
#undef LOG_SHARED
#define LOG_SHARED 0
#endif  // LOG_SHARED && defined(LOG_PLATFORM_POSIX_BASED) &&
// !defined(LOG_HAVE_SYS_MMAN_H)

#if LOG_MULTITHREADED && defined(LOG_PLATFORM_POSIX_BASED) && !defined(LOG_HAVE_PTHREAD)
#if LOG_COMPILER_WARNINGS
// warning only for posix-based systems, no MSVC
#warning("LOGGER: System has no pthread - logger cannot be multithreaded (LOG_MULTITHREADED)")
#endif  // LOG_COMPILER_WARNINGS

#undef LOG_MULTITHREADED
#define LOG_MULTITHREADED 0
#endif  // LOG_MULTITHREADED && defined (LOG_PLATFORM_POSIX_BASED) &&
// !defined(LOG_HAVE_PTHREAD)

#if LOG_UNHANDLED_EXCEPTIONS && !LOG_AUTO_DEBUGGING
#if LOG_COMPILER_WARNINGS

#ifdef LOG_COMPILER_MSVC
#pragma message( \
    "LOGGER: Specified LOG_UNHANDLED_EXCEPTIONS without LOG_AUTO_DEBUGGING. Force set LOG_AUTO_DEBUGGING = 1")
#else  // LOG_COMPILER_MSVC
#warning("LOGGER: Specified LOG_UNHANDLED_EXCEPTIONS without LOG_AUTO_DEBUGGING. Force set LOG_AUTO_DEBUGGING = 1")
#endif  // LOG_COMPILER_MSVC

#endif  // LOG_COMPILER_WARNINGS

#undef LOG_AUTO_DEBUGGING
#define LOG_AUTO_DEBUGGING 1
#endif  // LOG_UNHANDLED_EXCEPTIONS && !LOG_AUTO_DEBUGGING

#if LOG_AUTO_DEBUGGING && !LOG_USE_MODULEDEFINITION
#if LOG_COMPILER_WARNINGS

#ifdef LOG_COMPILER_MSVC
#pragma message( \
    "LOGGER: Specified LOG_AUTO_DEBUGGING without LOG_USE_MODULEDEFINITION. Force set LOG_USE_MODULEDEFINITION = 1")
#else  // LOG_COMPILER_MSVC
#warning("LOGGER: Specified LOG_AUTO_DEBUGGING without LOG_USE_MODULEDEFINITION. Force set LOG_USE_MODULEDEFINITION = 1")
#endif  // LOG_COMPILER_MSVC

#endif  // LOG_COMPILER_WARNINGS

#undef LOG_USE_MODULEDEFINITION
#define LOG_USE_MODULEDEFINITION 1
#endif  // LOG_AUTO_DEBUGGING && !LOG_USE_MODULEDEFINITION

#if LOG_RELEASE_ON_APP_CRASH && !LOG_UNHANDLED_EXCEPTIONS
#if LOG_COMPILER_WARNINGS

#ifdef LOG_COMPILER_MSVC
#pragma message( \
    "LOGGER: Log configuration issue. LOG_RELEASE_ON_APP_CRASH is enabled but LOG_UNHANDLED_EXCEPTIONS is disabled. No actions will be taken")
#else  // LOG_COMPILER_MSVC
#warning("LOGGER: Log configuration issue. LOG_RELEASE_ON_APP_CRASH is enabled but LOG_UNHANDLED_EXCEPTIONS is disabled. No actions will be taken")
#endif  // LOG_COMPILER_MSVC

#endif  // LOG_COMPILER_WARNINGS
#endif  // LOG_RELEASE_ON_APP_CRASH && !LOG_UNHANDLED_EXCEPTIONS

#if LOG_UNHANDLED_EXCEPTIONS && !LOG_FLUSH_FILE_EVERY_WRITE && !LOG_RELEASE_ON_APP_CRASH
#if LOG_COMPILER_WARNINGS

#ifdef LOG_COMPILER_MSVC
#pragma message( \
    "LOGGER: Log configuration issue. LOG_FLUSH_FILE_EVERY_WRITE is unset and LOG_RELEASE_ON_APP_CRASH is unset. You can obtain corrupted log file if process will crash")
#else  // LOG_COMPILER_MSVC
#warning("LOGGER: Log configuration issue. LOG_FLUSH_FILE_EVERY_WRITE is unset and LOG_RELEASE_ON_APP_CRASH is unset. You can obtain corrupted log file if process will crash")
#endif  // LOG_COMPILER_MSVC

#endif  // LOG_COMPILER_WARNINGS
#endif  // LOG_UNHANDLED_EXCEPTIONS && !LOG_FLUSH_FILE_EVERY_WRITE &&
// !LOG_RELEASE_ON_APP_CRASH

#if defined(LOG_PLATFORM_WINDOWS) && LOG_USE_SEH && !defined(LOG_COMPILER_MSVC)
#if LOG_COMPILER_WARNINGS
#warning("LOGGER: LOG_USE_SEH can be used only with Microsoft Visual C++ compiler")
#endif  // LOG_COMPILER_WARNINGS

#undef LOG_USE_SEH
#define LOG_USE_SEH 0

#endif  // defined(LOG_PLATFORM_WINDOWS) && LOG_USE_SEH && !defined(LOG_COMPILER_MSVC)

////////////////////////////// Precompiled configuration checking END //////////////////////////////

#endif /*LOGGER_CONFIGURATION_CHECK_HEADER*/
