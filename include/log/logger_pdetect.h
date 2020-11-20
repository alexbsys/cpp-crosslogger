
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

#ifdef __ANDROID__
#define LOG_PLATFORM_ANDROID
#endif /*__ANDROID__*/


/* CPU detection section */

#undef LOG_CPU_DETECTED

/* Intel amd64 */
#if defined(_AMD64_) || defined(__amd64__) \
    || defined(__amd64) || defined(__x86_64) || defined(_M_AMD64) || defined(__x86_64__)

#pragma message "Detected Intel AMD64"
#  define LOG_PLATFORM_64BIT
#  define LOG_CPU_INTEL
#  define LOG_CPUARCH_IA32
#  define LOG_CPU_ARCH  IA32

#  define LOG_CPU_DETECTED 1
#endif /* Intel amd64 */

/* Intel Itanium IA-64 */
#if (defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) \
    || defined(_M_IA64) || defined(__itanium__)) \
    && !defined(LOG_CPU_DETECTED)
#  define LOG_PLATFORM_64BIT
#  define LOG_CPU_INTEL
#  define LOG_CPUARCH_IA64
#  define LOG_CPU_ARCH  IA64

#  define LOG_CPU_DETECTED 1
#endif /*Intel itanium IA-64*/


/* MIPS */
#if (defined(__mips__) || defined(mips) || defined(__mips) || defined(__MIPS__)) \
    && !defined(LOG_CPU_DETECTED)
#  define LOG_PLATFORM_32BIT
#  define LOG_CPU_MIPS
#  define LOG_CPUARCH_MIPS
#  define LOG_CPU_ARCH MIPS

#define LOG_CPU_DETECTED 1
#endif /*MIPS*/


/*PowerPC*/
#if (defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__powerpc64__) \
    || defined(__ppc__) || defined(__ppc64__) || defined(__PPC__) || defined(__PPC64__) || defined(_ARCH_PPC) \
    || defined(_ARCH_PPC64) || defined(_M_PPC)) \
    && !defined(LOG_CPU_DETECTED)
#  define LOG_CPU_POWERPC
#  define LOG_CPUARCH_POWERPC
#  define LOG_CPU_ARCH  POWERPC

#  if defined(_ARCH_PPC64) || defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__)
#    define LOG_PLATFORM_64BIT
#  else /*PPC64*/
#    define LOG_PLATFORM_32BIT
#  endif /*PPC64*/

#define LOG_CPU_DETECTED 1
#endif /*PowerPC*/

/*TMS320*/
#if (defined(_TMS320C2XX) || defined(__TMS320C2000__) || defined(_TMS320C5X) \
    || defined(__TMS320C55X__) || defined(_TMS320C6X) || defined(__TMS320C6X__))
    && !defined(LOG_CPU_DETECTED)
#  define LOG_CPU_TMS320
#  define LOG_CPUARCH_TMS320
#  define LOG_CPU_ARCH  TMS320

#define LOG_PLATFORM_32BIT
#define LOG_CPU_DETECTED 1
#endif /*TMS320*/

/* ARM */
#if (defined (_M_ARM) || defined(__ARM_ARCH) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) \
    || defined(_ARM) || defined(_M_ARMT) || defined(__arm)) \
    && !defined(LOG_CPU_DETECTED)
#pragma message "Detected ARM"

#  define LOG_CPU_ARM

#  if defined(__ARM_ARCH_3__)
#    define LOG_CPU_ARCH ARM3
#    define LOG_CPUARCH_ARM3
#  endif /*__ARM_ARCH_3__*/

#  if defined(__ARM_ARCH_3M__)
#    define LOG_CPU_ARCH ARM3M
#    define LOG_CPUARCH_ARM3M
#  endif /*__ARM_ARCH_3M__*/

#  if defined(__ARM_ARCH_5__) || (_M_ARM==5)
#    define LOG_CPU_ARCH ARM5
#    define LOG_CPUARCH_ARM5
#  endif /*__ARM_ARCH_5__*/

#  if defined(__ARM_ARCH_5E__) || (_M_ARM==5)
#    define LOG_CPU_ARCH ARM5E
#    define LOG_CPUARCH_ARM5E
#  endif /*__ARM_ARCH_5E__*/

#  if defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) \
    || defined(__ARM_ARCH_6ZK__) || (_M_ARM==6)
#    define LOG_CPU_ARCH ARM6
#    define LOG_CPUARCH_ARM6
#endif /*ARM6*/

#  if defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) \
    || defined(__ARM_ARCH_7S__) || (_M_ARM==7)
#    define LOG_CPU_ARCH ARM7
#    define LOG_CPUARCH_ARM7
#endif /*ARM7*/

/* ARM64 */
#  if defined(__aarch64__) || defined(__ARM_ARCH_ISA_A64) /* arm 64 bit ? */
#    define LOG_PLATFORM_64BIT
#    define LOG_CPU_DETECTED 1
#  endif /*__aarch64__*/

/* ARM32 */
#  if defined(__arm__) && !defined(__arch64__) && !defined(__ARM_ARCH_ISA_A64)  /* arm 32 bit? */
#    define LOG_PLATFORM_32BIT
#    define LOG_CPU_DETECTED 1
#  endif /*__arm*__*/
#endif /*_M_ARM || __ARM_ARCH*/

/* Alpha */
#  if (defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)) && !defined(LOG_CPU_DETECTED)

#    if defined(__alpha_ev4__) || (_M_ALPHA==4)
#      define LOG_CPU_ARCH ALPHAEV4
#      define LOG_CPUARCH_ALPHAEV4
#    endif /*__alpha_ev4__*/

#    if defined(__alpha_ev5__) || (_M_ALPHA==5)
#      define LOG_CPU_ARCH ALPHAEV5
#      define LOG_CPUARCH_ALPHAEV5
#    endif /*__alpha_ev5__*/

#    if defined(__alpha_ev6__) || (_M_ALPHA==6)
#      define LOG_CPU_ARCH ALPHAEV6
#      define LOG_CPUARCH_ALPHAEV6
#    endif /*__alpha_ev6__*/

#    define LOG_PLATFORM_32BIT
#    define LOG_CPU_ALPHA
#    define LOG_CPU_DETECTED 1
#  endif /*alpha*/

/* Intel x86 */
#if (defined(__386__) || defined(_M_I386) || defined(__i386) || defined(_M_IX86) \
    || defined(_X86_) || defined(__I86__) || defined(__X86__) || defined(__i686__) || defined(__i586__) || defined(__i486__)) \
    && !defined(LOG_CPU_DETECTED)
#  define LOG_CPU_ARCH  IA32
#  define LOG_CPUARCH_IA32
#  define LOG_CPU_INTEL
#  define LOG_PLATFORM_32BIT
#  define LOG_CPU_DETECTED 1
#endif /* Intel X86 */

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

#if defined(__clang__)
#define LOG_COMPILER_CLANG
#endif /*__clang__*/

#if defined(__xlc__)
#define LOG_COMPILER_XLC
#endif /*__xlc__*/

#if defined(LOG_COMPILER_GCC) || defined(LOG_COMPILER_CLANG) || defined(LOG_COMPILER_XLC)
#define LOG_COMPILER_GCC_COMPAT
#endif /*LOG_COMPILER_GCC || LOG_COMPILER_CLANG || LOG_COMPILER_XLC*/

#ifdef LOG_CPP
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
#define LOG_CPP_X11  1
#else /*__cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)*/
#define LOG_CPP_X11  0
#endif /*__cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)*/
#endif /*LOG_CPP*/

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
