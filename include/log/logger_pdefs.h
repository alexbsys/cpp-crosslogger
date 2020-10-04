
#ifndef LOGGER_PLATFORM_DEFINES_HEADER
#define LOGGER_PLATFORM_DEFINES_HEADER

#include "logger_pdetect.h"

/// Use ANSI for varargs
/// Must be set for windows
#ifdef LOG_PLATFORM_WINDOWS 
#define ANSI
#endif /*LOG_PLATFORM_WINDOWS*/

#ifdef LOG_PLATFORM_POSIX_BASED
#define LOG_CDECL
#endif  // LOG_PLATFORM_POSIX_BASED

#ifdef LOG_PLATFORM_WINDOWS
#define LOG_CDECL __cdecl
#endif  // LOG_PLATFORM_WINDOWS

#ifdef LOG_COMPILER_MSVC
#include <inttypes.h>
#define LOG_FMT_I64 "%" PRId64
#else  // LOG_COMPILER_MSVC
#define LOG_FMT_I64 "%lld"
#endif  // LOG_COMPILER_MSVC

////////////////////////////// LOG_SELF_PROC_LINK definition for all platforms START //////////////////////////////
#if defined(LOG_PLATFORM_LINUX) || defined(LOG_PLATFORM_CYGWIN)
#define LOG_SELF_PROC_LINK "/proc/self/exe"
#endif  // defined(LOG_PLATFORM_LINUX) || defined(LOG_PLATFORM_CYGWIN)

#ifdef LOG_PLATFORM_UNIX
#define LOG_SELF_PROC_LINK "/proc/curproc/file"
#endif  // LOG_PLATFORM_UNIX

#ifdef LOG_PLATFORM_SOLARIS
#define LOG_SELF_PROC_LINK "/proc/self/path/a.out"
#endif  // LOG_PLATFORM_SOLARIS
////////////////////////////// LOG_SELF_PROC_LINK definition for all platforms END //////////////////////////////

#endif /*LOGGER_PLATFORM_DEFINES_HEADER*/
