
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
#define LOG_FMT_U64 "%" PRIu64
#else  // LOG_COMPILER_MSVC
#define LOG_FMT_I64 "%lld"
#define LOG_FMT_U64 "%llu"
#endif  // LOG_COMPILER_MSVC

#ifdef LOG_CPP_X11
#  define LOG_METHOD_OVERRIDE  override
#else /*LOG_CPP_X11*/
#  define LOG_METHOD_OVERRIDE
#endif /*LOG_CPP_X11*/


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

#if defined LOG_CPP

namespace logging {
typedef unsigned long uint32_t;

#if defined(LOG_COMPILER_GCC) || defined(LOG_COMPILER_MINGW) || \
    defined(LOG_COMPILER_ICC) || defined(LOG_COMPILER_SOLARIS)
typedef unsigned long long uint64_t;
#endif  // defined(LOG_COMPILER_GCC) || defined(LOG_COMPILER_MINGW) ||
// defined(LOG_COMPILER_ICC) || defined(LOG_COMPILER_SOLARIS)

#ifdef LOG_COMPILER_MSVC
typedef unsigned __int64 uint64_t;
typedef __int64 int64_t;
#endif  // LOG_COMPILER_MSVC

typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef long int32_t;
typedef short int16_t;
typedef char int8_t;

#if defined(LOG_PLATFORM_64BIT)
typedef uint64_t intptr_t;
#elif defined (LOG_PLATFORM_32BIT)
typedef unsigned int intptr_t;
#endif  /*LOG_PLATFORM_32BIT*/

}//namespace logging

#endif /*LOG_CPP*/

#endif /*LOGGER_PLATFORM_DEFINES_HEADER*/
