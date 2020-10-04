
#ifndef LOGGER_PLATFORM_DETECTION_HEADER
#define LOGGER_PLATFORM_DETECTION_HEADER


//////////////////////////////////////////////////////////////

#ifdef __cplusplus
#define LOG_CPP
#else
#define LOG_PUREC
#endif /*__cplusplus*/

//////////////////// Platform detection //////////////////////

#if defined(unix) || defined(__unix__) || defined(__unix)
#define LOG_PLATFORM_POSIX_BASED

#if defined(__CYGWIN__) && !defined(_WIN32)
#define LOG_PLATFORM_CYGWIN
#endif  // defined(__CYGWIN__) && !defined(_WIN32)

#if !defined(__linux__) && !defined(__linux) && !defined(linux) && !defined(__sun__) && \
    !defined(__sun) && !defined(__CYGWIN__) && !defined(__APPLE__) && !defined(_WIN32)
#define LOG_PLATFORM_UNIX
#include <sys/param.h>

#if defined(BSD)
#define LOG_PLATFORM_BSD
#endif  // BSD

#if defined(__FreeBSD__)
#define LOG_PLATFORM_FREEBSD
#endif  //__FreeBSD__

#if defined(__NetBSD__)
#define LOG_PLATFORM_NETBSD
#endif  //__NetBSD__

#if defined(__DragonFly__)
#define LOG_PLATFORM_DRAGONFLYBSD
#endif  //__DragonFly__

#endif  //! defined(__linux__) && !defined(__linux) && !defined(linux) &&
//! !defined(__sun__) && !defined(__sun) && !defined(__CYGWIN__) &&
//! !defined(__APPLE__) && !defined(_WIN32)

#endif  // defined(unix) || defined(__unix__) || defined(__unix)

#if !defined(LOG_PLATFORM_UNIX) && defined(LOG_PLATFORM_POSIX_BASED)
#if defined(__linux__) || defined(__linux) || defined(__gnu_linux) || defined(linux)
#define LOG_PLATFORM_LINUX
#endif  // defined(__linux__) || defined(__linux) || defined(__gnu_linux) ||
// defined(linux)
#endif  //! defined LOG_PLATFORM_UNIX && defined(LOG_PLATFORM_POSIX_BASED)

#if defined(__APPLE__) && defined(__MACH__)
#define LOG_PLATFORM_POSIX_BASED
#define LOG_PLATFORM_MAC

#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
#define LOG_PLATFORM_IPHONE
#define LOG_PLATFORM_IPHONE_SIMULATOR
#elif TARGET_OS_IPHONE == 1
#define LOG_PLATFORM_IPHONE
#elif TARGET_OS_MAC == 1
#define LOG_PLATFORM_MACOSX
#endif

#endif  // defined(__APPLE__) && defined(__MACH__)

#if defined(LOG_PLATFORM_POSIX_BASED) && \
    (defined(__sun__) || defined(__sun) || defined(__SunOS) || defined(sun))
#define LOG_PLATFORM_POSIX_BASED
#define LOG_PLATFORM_SOLARIS
#endif  // defined(LOG_PLATFORM_POSIX_BASED) && (defined(__sun__) || defined(__sun) ||
// defined(__SunOS) || defined(sun))

#ifdef _WIN32
#undef LOG_PLATFORM_POSIX_BASED  // it can be set if you use CYGWIN with GCC for Windows
// target
#undef LOG_PLATFORM_CYGWIN
#undef LOG_PLATFORM_UNIX

#define LOG_PLATFORM_WINDOWS
#endif  //_WIN32


#if defined(_AMD64_) || defined(__LP64__) || defined(_LP64)
#  define LOG_PLATFORM_64BIT
#else  // defined(_AMD64_) || defined(__LP64__) || defined(_LP64)
#  define LOG_PLATFORM_32BIT
#endif  // defined(_AMD64_) || defined(__LP64__) || defined(_LP64)


////////////////// Platform detection end ////////////////////
//////////////////////////////////////////////////////////////

// Compiler detection

#ifdef _MSC_VER
#define LOG_COMPILER_MSVC
#endif  //_MSC_VER

#ifdef __GNUC__
#define LOG_COMPILER_GCC
#endif  //__GNUC__

#ifdef __IBMCPP__
#define LOG_COMPILER_IBM
#endif  //__IBMCPP__

#if defined(__ICC) || defined(__INTEL_COMPILER)
#define LOG_COMPILER_ICC
#endif  // defined(__ICC) || defined(__INTEL_COMPILER)

#if defined(__SUNPRO_CC) || defined(__SUNPRO_C)
#define LOG_COMPILER_SOLARIS
#endif  // defined(__SUNPRO_CC) || defined(__SUNPRO_C)

#if defined(__MINGW32__) || defined(__MINGW64__)
#define LOG_COMPILER_MINGW
#endif  // defined(__MINGW32__) || defined(__MINGW64__)

//////////////////////////////////////////////////////////////
//////////// POSIX BASED PLATFORM CONFIGURATION //////////////

#ifdef LOG_PLATFORM_POSIX_BASED

#if !LOG_USE_CONFIG_H

// User can change this section if autoconf is not used
#define LOG_HAVE_UNISTD_H
#define LOG_HAVE_SYS_SYSCALL_H
#define LOG_HAVE_SYS_UTSNAME_H
#define LOG_HAVE_PWD_H
#define LOG_HAVE_PTHREAD
#define LOG_HAVE_SYS_MMAN_H

#ifndef LOG_PLATFORM_MAC
#  define LOG_HAVE_SYS_SYSINFO_H
#endif //LOG_PLATFORM_MAC

// Platform-based detection
#ifdef LOG_PLATFORM_CYGWIN
#undef LOG_HAVE_SYS_SYSCALL_H
#endif  // LOG_PLATFORM_CYGWIN

#else  //! LOG_USE_CONFIG_H

#ifdef HAVE_UNISTD_H
#define LOG_HAVE_UNISTD_H
#endif  // HAVE_UNISTD_H

#ifdef HAVE_SYS_SYSCALL_H
#define LOG_HAVE_SYS_SYSCALL_H
#endif  // HAVE_SYS_SYSCALL_H

#ifdef HAVE_SYS_UTSNAME_H
#define LOG_HAVE_SYS_UTSNAME_H
#endif  // HAVE_SYS_UTSNAME_H

#ifdef HAVE_PWD_H
#define LOG_HAVE_PWD_H
#endif  // HAVE_PWD_H

#ifdef HAVE_PTHREAD
#define LOG_HAVE_PTHREAD
#endif  // HAVE_PTHREAD

#ifdef HAVE_SYS_MMAN_H
#define LOG_HAVE_SYS_MMAN_H
#endif  // HAVE_SYS_MMAN_H

#ifdef HAVE_SYS_SYSINFO_H
#define LOG_HAVE_SYS_SYSINFO_H
#endif  // HAVE_SYS_SYSINFO_H

#endif  // LOG_USE_CONFIG_H

#endif  // LOG_PLATFORM_POSIX_BASED

////////// POSIX BASED PLATFORM CONFIGURATION END ////////////
//////////////////////////////////////////////////////////////

#if (!defined(LOG_PLATFORM_WINDOWS) && !defined(LOG_PLATFORM_POSIX_BASED)) || \
    (defined(LOG_PLATFORM_WINDOWS) && defined(LOG_PLATFORM_POSIX_BASED))
#error "Unsupported platform detected"
#endif  //(!defined(LOG_PLATFORM_WINDOWS) && !defined(LOG_PLATFORM_POSIX_BASED)) ||
//(defined(LOG_PLATFORM_WINDOWS) && defined(LOG_PLATFORM_POSIX_BASED))

#endif /*LOGGER_PLATFORM_DETECTION_HEADER*/
