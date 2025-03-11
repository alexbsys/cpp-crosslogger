/**
  \brief  Platform detection header. Detects compiler and features, platform, target OS, C/C++ version, etc

     Detection based on:
      https://sourceforge.net/p/predef/wiki/Architectures/
      https://sourceforge.net/p/predef/wiki/Compilers/
      https://sourceforge.net/p/predef/wiki/OperatingSystems/
      https://stackoverflow.com/questions/28939652/how-to-detect-sse-sse2-avx-avx2-avx-512-avx-128-fma-kcvi-availability-at-compile
      https://stackoverflow.com/questions/142508/how-do-i-check-os-with-a-preprocessor-directive
      https://www.boost.org/doc/libs/develop/libs/predef/doc/index.html#_boost_os_operating_system_macros
      https://web.archive.org/web/20191012035921/http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system

     PLATFORM_BASE -> PLATFORM_SPEC -> PLATFORM_TARGETOS

     PLATFORM_BASE: posix
     PLATFORM_SPEC(PLATFORM_TARGETOS):   unix(unknown,bsd,bsdi,freebsd,netbsd,dragonfly,openbsd)
                                         apple(unknown,ios,macos)
                                         linux(unknown)
                                         cygwin(windows)   <-- cygwin posix compliant mode
                                         sun(unknown,solaris)
                                         android(unknown)
                                         aix(aix)
                                         hpux(hpux)
                                         irix(irix)
                                         qnx(unknown,neutrino)
                                         
     PLATFORM_BASE: windows
     PLATFORM_SPEC(PLaTFORM_TARGETOS):   windows(windows)
                                         cygwin(windows) <-- cygwin non-posix compliant mode

     PLATFORM_BASE: freertos
     PLATFORM_SPEC(PLATFORM_TARGETOS):   freertos(freertos)

     PLATFORM_BASE: asmjs
     PLATFORM_SPEC(PLATFORM_TARGETOS):   asmjs(unknown)

     Latest supported MS Visual Studio 16.10, newer compiler versions will be reported as 16.10
     

  \author alexb@clever.team
*/


#ifndef PDETECT_HEADER
#define PDETECT_HEADER


/****** Configuration section ******/

/* Platform detect verbosity level to compiler messages. 0 - no messages, 1 - info, 2 - debug */
#define PDETECT_CONFIG_VERBOSE 0

/* Use include <sys/param.h> when platform is Unix */
#define PDETECT_CONFIG_UNIX_USE_SYS_PARAM_HEADER 1

/* Use include <TargetConditionals.h> when platform is mac */
#define PDETECT_CONFIG_MAC_USE_TARGET_CONDITIONALS_HEADER 1

/* Define CPU endian detection macros. They can be used only in runtime, not for static checks */
#define PDETECT_CONFIG_CPU_ENDIAN_DETECTION      1

/******* Platform codes for check automation *******/

/* Platform base codes. Can be used to compare with PDETECT_PLATFORM_BASE */
#define PDETECT_PLATFORM_BASECODE_UNKNOWN  0
#define PDETECT_PLATFORM_BASECODE_POSIX    1
#define PDETECT_PLATFORM_BASECODE_WINDOWS  2
#define PDETECT_PLATFORM_BASECODE_IBM      3
#define PDETECT_PLATFORM_BASECODE_ASMJS    4

/* Platform specification codes. Can be used to compare with PDETECT_PLATFORM_SPEC */
#define PDETECT_PLATFORM_SPECCODE_UNKNOWN 0
#define PDETECT_PLATFORM_SPECCODE_WINDOWS 1
#define PDETECT_PLATFORM_SPECCODE_CYGWIN  2
#define PDETECT_PLATFORM_SPECCODE_LINUX   3
#define PDETECT_PLATFORM_SPECCODE_UNIX    4
#define PDETECT_PLATFORM_SPECCODE_HPUX    5
#define PDETECT_PLATFORM_SPECCODE_AIX     6
#define PDETECT_PLATFORM_SPECCODE_IRIX    7
#define PDETECT_PLATFORM_SPECCODE_ANDROID 8
#define PDETECT_PLATFORM_SPECCODE_SUN     9
#define PDETECT_PLATFORM_SPECCODE_APPLE   10
#define PDETECT_PLATFORM_SPECCODE_ASMJS   11

/* Target OS detection codes. Can be used to compare with PDETECT_PLATFORM_TARGETOS */
#define PDETECT_PLATFORM_OSCODE_UNKNOWN   0
#define PDETECT_PLATFORM_OSCODE_WINDOWS   1
#define PDETECT_PLATFORM_OSCODE_IOS       2
#define PDETECT_PLATFORM_OSCODE_MACOS     3
#define PDETECT_PLATFORM_OSCODE_ANDROID   4
#define PDETECT_PLATFORM_OSCODE_FREEBSD   5
#define PDETECT_PLATFORM_OSCODE_NETBSD    6
#define PDETECT_PLATFORM_OSCODE_OPENBSD   7
#define PDETECT_PLATFORM_OSCODE_DRAGONFLYBSD 8
#define PDETECT_PLATFORM_OSCODE_SOLARIS   9
#define PDETECT_PLATFORM_OSCODE_BSDI      10

/* CPU detection codes. Can be used to compare with PDETECT_CPU_TYPE */
#define PDETECT_CPU_TYPECODE_UNKNOWN    0
#define PDETECT_CPU_TYPECODE_INTEL      1
#define PDETECT_CPU_TYPECODE_MIPS       2
#define PDETECT_CPU_TYPECODE_POWERPC    3
#define PDETECT_CPU_TYPECODE_TMS320     4
#define PDETECT_CPU_TYPECODE_ARM        5
#define PDETECT_CPU_TYPECODE_ALPHA      6

/* CPU architecture detection codes. Can be used to compare with PDETECT_CPUARCH_TYPE */
#define PDETECT_CPU_ARCHCODE_UNKNOWN    0
#define PDETECT_CPU_ARCHCODE_IA32       1
#define PDETECT_CPU_ARCHCODE_IA64       2
#define PDETECT_CPU_ARCHCODE_MIPS       3
#define PDETECT_CPU_ARCHCODE_POWERPC    4
#define PDETECT_CPU_ARCHCODE_TMS320     5
#define PDETECT_CPU_ARCHCODE_ARM3       6
#define PDETECT_CPU_ARCHCODE_ARM3M      7
#define PDETECT_CPU_ARCHCODE_ARM5       8
#define PDETECT_CPU_ARCHCODE_ARM5E      9
#define PDETECT_CPU_ARCHCODE_ARM6       10
#define PDETECT_CPU_ARCHCODE_ARM7       11
#define PDETECT_CPU_ARCHCODE_ARM64      12
#define PDETECT_CPU_ARCHCODE_ALPHAEV5   13
#define PDETECT_CPU_ARCHCODE_ALPHAEV6   14
#define PDETECT_CPU_ARCHCODE_8051       15

/* Compiler detection codes. Can be used to compare with PDETECT_COMPILER_TYPE */
#define PDETECT_COMPILER_CODE_UNKNOWN  0
#define PDETECT_COMPILER_CODE_MSVC     1
#define PDETECT_COMPILER_CODE_GCC      2
#define PDETECT_COMPILER_CODE_IBMXLC   3
#define PDETECT_COMPILER_CODE_ICC      4
#define PDETECT_COMPILER_CODE_SOLARIS  5
#define PDETECT_COMPILER_CODE_CLANG    6
#define PDETECT_COMPILER_CODE_LCC      7
#define PDETECT_COMPILER_CODE_MINGW    8
#define PDETECT_COMPILER_CODE_WEBASSEMBLY  9
#define PDETECT_COMPILER_CODE_IAR      10
#define PDETECT_COMPILER_CODE_BORLANDC 11
#define PDETECT_COMPILER_CODE_DIGITALMARS 12
#define PDETECT_COMPILER_CODE_DJGPP    13
#define PDETECT_COMPILER_CODE_METROWERKS  14
#define PDETECT_COMPILER_CODE_TEXASINSTRUMENTS 15
#define PDETECT_COMPILER_CODE_KEIL     16
#define PDETECT_COMPILER_CODE_TINYC    17
#define PDETECT_COMPILER_CODE_TURBOC   18
#define PDETECT_COMPILER_CODE_WATCOM   19

//#define PDETECT_COMPILER_CODE_

/****** Helpers ******/

/* Stringify macro. Can be used in pragma message */
#define PDETECT_XSTR(x) PDETECT_STR(x)
#define PDETECT_STR(x) #x

/****** Language detect ******/

#ifdef __cplusplus
#  define PDETECT_CPP
#else
#  define PDETECT_PUREC
#endif /*__cplusplus*/

/****** Platform detection ******/
/** Result of this section are macros: 
 1. Defined always. Only signle value for each macro

  PDETECT_PLATFORM_BASE      - numeric code PDETECT_PLATFORM_BASECODE_xxxxx or PDETECT_PLATFORM_BASECODE_UNKNOWN when not detected
  PDETECT_PLATFORM_BASE_NAME - string name of platform or empty string when not detected. Lower case

  PDETECT_PLATFORM_SPEC      - numeric code PDETECT_PLATFORM_SPECCODE_xxxxx or PDETECT_PLATFORM_SPECCODE_UNKNOWN when not detected
  PDETECT_PLATFORM_SPEC_NAME - string name of platform specification or empty string when not detected. Lower case

  PDETECT_PLATFORM_SPEC      - numeric code PDETECT_PLATFORM_SPECCODE_xxxxx or PDETECT_PLATFORM_SPECCODE_UNKNOWN when not detected
  PDETECT_PLATFORM_SPEC_NAME - string name of platform specification or empty string when not detected. Lower case

 2. Defined when platform is correspond. Multiple definitions are possible. No values are defined for that macro
  PDETECT_PLATFORM_POSIX_BASED - platform is support POSIX. Defined for all posix platforms include android, linux, unix, asm.js etc. Does not defined for mac, ios
  PDETECT_PLATFORM_CYGWIN      - platform is CYGWIN (posix or not). Defined for all CYGWIN. If defined PDETECT_PLATFORM_POSIX_BASED also,
                                 CYGWIN is in posix mode, if defined PDETECT_PLATFORM_WINDOWS - non-posix
  PDETECT_PLATFORM_UNIX        - platform is UNIX, but not mac, linux, sun or other compatible. Defined for all UNIX
  PDETECT_PLATFORM_BSD         - platform is UNIX BSD compatible

  PDETECT_PLATFORM_BSDI        - platform is UNIX, target OS is BSDi
  PDETECT_PLATFORM_FREEBSD     - platform is UNIX, target OS is FreeBSD
  PDETECT_PLATFORM_NETBSD      - platform is UNIX, target OS is NetBSD
  PDETECT_PLATFORM_DRAGONFLYBSD - platform is UNIX, target OS is DragonFlyBSD
  PDETECT_PLATFORM_OPENBSD     - platform is UNIX, target OS is OpenBSD
  PDETECT_PLATFORM_LINUX       - platform is Linux (always posix). Also defined for Android Platform

  PDETECT_PLATFORM_APPLE       - platform is Apple-based (macos, ios, macintosh, etc). Not posix and not unix
  PDETECT_PLATFORM_IPHONE      - platform is iPhone ios. Apple always
  PDETECT_PLATFORM_IPHONE_SIMULATOR - platform is iPhone simulator. PDETECT_PLATFORM_IPHONE always defined also, Apple always
  PDETECT_PLATFORM_MACOS       - platform is macos. Apple always

  PDETECT_PLATFORM_ANDROID     - platform is Android (always posix and linux)
  
  PDETECT_PLATFORM_SUN         - platform is SUN based (always posix)
  PDETECT_PLATFORM_SOLARIS     - platform is SUN, target OS is Solaris

  PDETECT_PLATFORM_WINDOWS     - platform is Windows based

  PDETECT_PLATFORM_HPUX        - platform is HP-UX

  PDETECT_PLATFORM_QNX         - platform is QNX based, alway posix
  PDETECT_PLATFORM_QNX_NEUTRINO - platform is QNX Neutrino (target OS version > 4.0), always QNX and posix

  PDETECT_PLATFORM_AIX         - platform is AIX, always posix
  PDETECT_PLATFORM_IRIX        - platform is IRIX, always posix
  
 3. Additional macro, defined for specific platform or OS
  PDETECT_PLATFORM_ANDROID_API - android API level, integer value. Defined always when PDETECT_PLATFORM_ANDROID was found

 Examples:

 Check ANDROID support

 #ifdef PDETECT_PLATFORM_ANDROID
 ...
 #endif

 or
 #if PDETECT_PLATFORM_SPEC==PDETECT_PLATFORM_SPECCODE_ANDROID
 ...
 #endif

 Check CYGWIN posix based support
 #if defined(PDETECT_PLATFORM_CYGWIN) && defined(PDETECT_PLATFORM_POSIX_BASED)
 ...
 #endif

 or
 #if PDETECT_PLATFORM_BASE==PDETECT_PLATFORM_BASECODE_POSIX && PDETECT_PLATFORM_SPEC==PDETECT_PLATFORM_SPECCODE_CYGWIN
 ...
 #endif

 Check CYGWIN non-posix (windows)
 #if defined(PDETECT_PLATFORM_CYGWIN) && defined(PDETECT_PLATFORM_WINDOWS)
 ...
 #endif

 or
 #if PDETECT_PLATFORM_BASE==PDETECT_PLATFORM_BASECODE_WINDOWS && PDETECT_PLATFORM_SPEC==PDETECT_PLATFORM_SPECCODE_CYGWIN
 ...
 #endif

*/

/*** Unix -> Not linux */
#if defined(unix) || defined(__unix__) || defined(__unix)
#  define PDETECT_PLATFORM_POSIX_BASED
#  define PDETECT_PLATFORM_BASE_NAME "posix"
#  define PDETECT_PLATFORM_BASE  PDETECT_PLATFORM_BASECODE_POSIX

/* Cygwin without _WIN32 -> POSIX-compliant cygwin on windows */
#  if defined(__CYGWIN__) && (!defined(_WIN32) && !defined(_WIN64))
#    define PDETECT_PLATFORM_CYGWIN
#    define PDETECT_PLATFORM_SPEC_NAME "cygwin"
#    define PDETECT_PLATFORM_SPEC  PDETECT_PLATFORM_SPECCODE_CYGWIN
#    define PDETECT_PLATFORM_TARGETOS_NAME "windows"
#  endif  /* defined(__CYGWIN__) */

/* No linux, sun, cygwin, apple or win32 -> this is other Unix compatible platform */
#  if !defined(__linux__) && !defined(__linux) && !defined(linux) && !defined(__sun__) && \
      !defined(__sun) && !defined(__CYGWIN__) && !defined(__APPLE__) && !defined(_WIN32) && \
      !defined(_AIX) && !defined(__MACH__) && !defined(__sgi)

#    define PDETECT_PLATFORM_UNIX
#    define PDETECT_PLATFORM_SPEC_NAME "unix"
#    define PDETECT_PLATFORM_SPEC  PDETECT_PLATFORM_SPECCODE_UNIX

#    if PDETECT_CONFIG_UNIX_USE_SYS_PARAM_HEADER
#      include <sys/param.h>
#    endif /*PDETECT_UNIX_USE_SYS_PARAM_HEADER*/

#    if defined(BSD)
#      define PDETECT_PLATFORM_BSD
#    endif  /*BSD*/

#    if defined(__bsdi__)
#      define PDETECT_PLATFORM_BSDI
#      define PDETECT_PLATFORM_TARGETOS   PDETECT_PLATFORM_OSCODE_BSDI
#      define PDETECT_PLATFORM_TARGETOS_NAME "bsdi"
#    endif /*__bsdi__*/

#    if defined(__FreeBSD__)
#      define PDETECT_PLATFORM_FREEBSD
#      define PDETECT_PLATFORM_TARGETOS   PDETECT_PLATFORM_OSCODE_FREEBSD
#      define PDETECT_PLATFORM_TARGETOS_NAME "freebsd"
#    endif  /*__FreeBSD__*/

#    if defined(__NetBSD__)
#      define PDETECT_PLATFORM_NETBSD
#      define PDETECT_PLATFORM_TARGETOS   PDETECT_PLATFORM_OSCODE_NETBSD
#      define PDETECT_PLATFORM_TARGETOS_NAME "netbsd"
#    endif  /*__NetBSD__*/

#    if defined(__DragonFly__)
#      define PDETECT_PLATFORM_DRAGONFLYBSD
#      define PDETECT_PLATFORM_TARGETOS   PDETECT_PLATFORM_OSCODE_DRAGONFLYBSD
#      define PDETECT_PLATFORM_TARGETOS_NAME "dragonfly"
#    endif  /*__DragonFly__*/

#    if defined(__OpenBSD__)
#      define PDETECT_PLATFORM_OPENBSD
#      define PDETECT_PLATFORM_TARGETOS   PDETECT_PLATFORM_OSCODE_OPENBSD
#      define PDETECT_PLATFORM_TARGETOS_NAME "openbsd"
#    endif /*__OpenBSD__*/

#  endif  /* ! defined(__linux__) && !defined(__linux) && !defined(linux) && \
             !defined(__sun__) && !defined(__sun) && !defined(__CYGWIN__) && \
             !defined(__APPLE__) && !defined(_WIN32) */
#endif  /* defined(unix) || defined(__unix__) || defined(__unix) */

/* Unix -> Linux */
#if !defined(PDETECT_PLATFORM_UNIX) && defined(PDETECT_PLATFORM_POSIX_BASED)

#  if !defined(__ANDROID__) && (defined(__linux__) || defined(__linux) || defined(__gnu_linux) || defined(linux) || defined(__gnu_linux__))
#    define PDETECT_PLATFORM_LINUX
#    define PDETECT_PLATFORM_SPEC_NAME "linux"
#    define PDETECT_PLATFORM_SPEC   PDETECT_PLATFORM_SPECCODE_LINUX
#  endif  /* !defined(__ANDROID__) && (defined(__linux__) || defined(__linux) || defined(__gnu_linux) || defined(linux) || defined(__gnu_linux__)) */

#endif  /* !defined PDETECT_PLATFORM_UNIX && defined(PDETECT_PLATFORM_POSIX_BASED) */

/* Platform base: posix, Spec: mac */
#if (defined(__APPLE__) && (defined(__MACH__)))  || defined(macintosh) || defined(Macintosh)
#  define PDETECT_PLATFORM_APPLE

#  undef PDETECT_PLATFORM_SPEC
#  undef PDETECT_PLATFORM_SPEC_NAME

#  define PDETECT_PLATFORM_SPEC   PDETECT_PLATFORM_SPECCODE_APPLE
#  define PDETECT_PLATFORM_SPEC_NAME "apple"

#  if PDETECT_MAC_USE_TARGET_CONDITIONALS_HEADER
#    include <TargetConditionals.h>
#  endif /*PDETECT_MAC_USE_TARGET_CONDITIONALS_HEADER*/

#  if TARGET_IPHONE_SIMULATOR == 1
#    define PDETECT_PLATFORM_IPHONE
#    define PDETECT_PLATFORM_IPHONE_SIMULATOR
#    define PDETECT_PLATFORM_TARGETOS_NAME "ios"
#    define PDETECT_PLATFORM_TARGETOS  PDETECT_PLATFORM_OSCODE_IOS
#  elif defined(TARGET_OS_IPHONE) && ((TARGET_OS_IPHONE == 1) || defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__))
#    define PDETECT_PLATFORM_IPHONE
#    define PDETECT_PLATFORM_TARGETOS_NAME "ios"
#    define PDETECT_PLATFORM_TARGETOS  PDETECT_PLATFORM_OSCODE_IOS
#  elif defined(TARGET_OS_MAC) && (TARGET_OS_MAC == 1)
#    define PDETECT_PLATFORM_MACOS
#    define PDETECT_PLATFORM_TARGETOS_NAME "macos"
#    define PDETECT_PLATFORM_TARGETOS  PDETECT_PLATFORM_OSCODE_MACOS
#  endif
#endif  /* (defined(__APPLE__) && (defined(__MACH__)))  || defined(macintosh) || defined(Macintosh) */

/* Platform base: posix, Spec: Sun */
#if defined(PDETECT_PLATFORM_POSIX_BASED) && \
    (defined(__sun__) || defined(__sun) || defined(__SunOS) || defined(sun))

#  define PDETECT_PLATFORM_SPEC    PDETECT_PLATFORM_SPECCODE_SUN
#  define PDETECT_PLATFORM_SPEC_NAME "sun"
#  define PDETECT_PLATFORM_SUN

#  if defined(__SVR4)
#    define PDETECT_PLATFORM_SOLARIS
#    define PDETECT_PLATFORM_TARGETOS   PDETECT_PLATFORM_OSCODE_SOLARIS
#    define PDETECT_PLATFORM_TARGETOS_NAME "solaris"
#  endif /*__SVR4*/

#endif  /* defined(PDETECT_PLATFORM_POSIX_BASED) && (defined(__sun__) || defined(__sun) || \
          defined(__SunOS) || defined(sun)) */

/* Platform base: Windows */
#ifdef _WIN32

#  define PDETECT_PLATFORM_WINDOWS
#  define PDETECT_PLATFORM_BASE_NAME "windows"
#  define PDETECT_PLATFORM_BASE   PDETECT_PLATFORM_BASECODE_WINDOWS

/* Cygwin + _WIN32 -> Non-posix compliant cygwin mode */
#  if defined(__CYGWIN__)
#    define PDETECT_PLATFORM_SPEC   PDETECT_PLATFORM_SPECCODE_CYGWIN
#    define PDETECT_PLATFORM_SPEC_NAME "cygwin"
#    define PDETECT_PLATFORM_CYGWIN
#  endif /*defined(__CYGWIN__) */

#  define PDETECT_PLATFORM_TARGETOS_NAME "windows"
#  define PDETECT_PLATFORM_TARGETOS  PDETECT_PLATFORM_OSCODE_WINDOWS

#endif  /*_WIN32*/


/* Platform base: posix, Spec: Android */
#if defined(__ANDROID__) && defined(PDETECT_PLATFORM_POSIX_BASED)
#  define PDETECT_PLATFORM_ANDROID

#  undef PDETECT_PLATFORM_SPEC_NAME
#  undef PDETECT_PLATFORM_SPEC

#  define PDETECT_PLATFORM_SPEC_NAME "android"
#  define PDETECT_PLATFORM_SPEC   PDETECT_PLATFORM_SPECCODE_ANDROID

#  define PDETECT_PLATFORM_TARGETOS_NAME "android"
#  define PDETECT_PLATFORM_TARGETOS PDETECT_PLATFORM_OSCODE_ANDROID

#  if defined(__ANDROID_API__)
#    define PDETECT_PLATFORM_ANDROID_API __ANDROID_API__
#  else /*__ANDROID_API__*/
#    define PDETECT_PLATFORM_ANDROID_API 0
#  endif /*__ANDROID_API__*/

#endif /*__ANDROID__*/

/* HP-UX */
#if (defined(__hpux) || defined(_hpux) || defined(hpux)) && defined(PDETECT_PLATFORM_POSIX_BASED)
#  define PDETECT_PLATFORM_HPUX

#  undef PDETECT_PLATFORM_SPEC_NAME
#  undef PDETECT_PLATFORM_SPEC

#  define PDETECT_PLATFORM_SPEC_NAME "hp-ux"
#  define PDETECT_PLATFORM_BASE   PDETECT_PLATFORM_SPECCODE_HPUX
#endif /*__hpux*/


/* IRIX */
#if (defined(sgi) || defined(__sgi)) && defined(PDETECT_PLATFORM_POSIX_BASED)
#  define PDETECT_PLATFORM_IRIX

#  undef PDETECT_PLATFORM_SPEC_NAME
#  undef PDETECT_PLATFORM_SPEC

#  define PDETECT_PLATFORM_SPEC_NAME "irix"
#  define PDETECT_PLATFORM_SPEC   PDETECT_PLATFORM_SPECCODE_IRIX
#endif /*defined(sgi) || defined(__sgi)*/

/* IBM AIX */
#if defined(_AIX) && defined(PDETECT_PLATFORM_POSIX_BASED)
#  define PDETECT_PLATFORM_AIX

#  undef PDETECT_PLATFORM_SPEC_NAME
#  undef PDETECT_PLATFORM_SPEC

#  define PDETECT_PLATFORM_SPEC_NAME "aix"
#  define PDETECT_PLATFORM_SPEC   PDETECT_PLATFORM_SPECCODE_AIX
#endif /*_AIX*/

/* QNX */
#if defined(__QNX__) && defined(PDETECT_PLATFORM_POSIX_BASED)
#  define PDETECT_PLATFORM_QNX

#  undef PDETECT_PLATFORM_SPEC_NAME
#  undef PDETECT_PLATFORM_SPEC

#  define PDETECT_PLATFORM_SPEC_NAME "qnx"
#  define PDETECT_PLATFORM_SPEC   PDETECT_PLATFORM_SPECCODE_QNX

#  if defined(__QNXNTO__) || defined(_NTO_VERSION)
#    define PDETECT_PLATFORM_QNX_NEUTRINO
#    define PDETECT_PLATFORM_TARGETOS_NAME  "neutrino"
#  endif /*defined(__QNXNTO__) || defined(_NTO_VERSION)*/

#endif /*defined(__QNX__) && defined(PDETECT_PLATFORM_POSIX_BASED)*/

/* Asm.js WebAssembly */
#if defined(__asmjs__) && defined(__asmjs)
#  define PDETECT_PLATFORM_ASMJS

#  undef PDETECT_PLATFORM_BASE
#  undef PDETECT_PLATFORM_BASE_NAME

#  undef PDETECT_PLATFORM_SPEC_NAME
#  undef PDETECT_PLATFORM_SPEC

#  define PDETECT_PLATFORM_BASE  PDETECT_PLATFORM_BASECODE_ASMJS
#  define PDETECT_PLATFORM_BASE_NAME "asmjs"

#  define PDETECT_PLATFORM_SPEC   PDETECT_PLATFORM_SPECCODE_ASMJS
#  define PDETECT_PLATFORM_SPEC_NAME "asmjs"
#endif /*defined(__asmjs__) && defined(__asmjs)*/

/* Unknown platform */
#if !defined(PDETECT_PLATFORM_TARGETOS)
#  define PDETECT_PLATFORM_TARGETOS   PDETECT_PLATFORM_OSCODE_UNKNOWN
#  define PDETECT_PLATFORM_TARGETOS_NAME ""
#endif /* !defined(PDETECT_PLATFORM_TARGETOS)*/

#if !defined(PDETECT_PLATFORM_SPEC)
#  define PDETECT_PLATFORM_SPEC    PDETECT_PLATFORM_SPECCODE_UNKNOWN
#  define PDETECT_PLATFORM_SPEC_NAME ""
#endif /* !defined(PDETECT_PLATFORM_SPEC)*/

#if !defined(PDETECT_PLATFORM_BASE)
#  define PDETECT_PLATFORM_BASE    PDETECT_PLATFORM_BASECODE_UNKNOWN
#  define PDETECT_PLATFORM_BASE_NAME ""
#endif /* !defined(PDETECT_PLATFORM_BASE)*/

/* Report detected platform when verbosity level >= 1 */
#if PDETECT_CONFIG_VERBOSE>=1
#  pragma message "Detected platform base: '" PDETECT_PLATFORM_BASE_NAME "', code " PDETECT_XSTR(PDETECT_PLATFORM_BASE)
#  pragma message "Detected platform spec: '" PDETECT_PLATFORM_SPEC_NAME "', code " PDETECT_XSTR(PDETECT_PLATFORM_SPEC)
#  pragma message "Detected platform target OS: '" PDETECT_PLATFORM_TARGETOS_NAME "', code " PDETECT_XSTR(PDETECT_PLATFORM_TARGETOS)
#endif /*PDETECT_CONFIG_VERBOSE>=1*/


/****** CPU detection section ******/

/* Endians detection. Used method from https://stackoverflow.com/questions/8978935/detecting-endianness
   Macros PDETECT_CPU_IS_BIG_ENDIAN() and PDETECT_CPU_IS_LITTLE_ENDIAN() can be used only in runtime 
 */
#if PDETECT_CONFIG_CPU_ENDIAN_DETECTION
static int PDETECT_IsBigEndian() { int i=1; return ! *((char*)&i); }
#  define PDETECT_CPU_IS_BIG_ENDIAN() (PDETECT_IsBigEndian())
#  define PDETECT_CPU_IS_LITTLE_ENDIAN() (!PDETECT_IsBigEndian())
#endif /*PDETECT_CONFIG_CPU_ENDIAN_DETECTION*/

/* Detect CPU type and architecture */
#undef PDETECT_CPU_DETECTED

/* Intel amd64: reported as IA32 with 64 bits */
#if defined(_AMD64_) || defined(__amd64__) \
    || defined(__amd64) || defined(__x86_64) || defined(_M_AMD64) || defined(__x86_64__)

#  define PDETECT_PLATFORM_64BIT
#  define PDETECT_CPU_INTEL
#  define PDETECT_CPUARCH_IA32
#  define PDETECT_CPU_ARCH  IA32

#  define PDETECT_CPU_TYPE_NAME "intel"
#  define PDETECT_CPU_TYPE      PDETECT_CPU_TYPECODE_INTEL

#  define PDETECT_CPUARCH_TYPE_NAME "ia32"
#  define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_IA32
#  define PDETECT_CPU_BITS      64

#  define PDETECT_CPU_DETECTED 1
#endif /* Intel amd64 */

/* Intel Itanium IA64: reported as IA64 with 64 bits */
#if (defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) \
    || defined(_M_IA64) || defined(__itanium__)) \
    && !defined(PDETECT_CPU_DETECTED)
#  define PDETECT_PLATFORM_64BIT
#  define PDETECT_CPU_INTEL
#  define PDETECT_CPUARCH_IA64
#  define PDETECT_CPU_ARCH  IA64

#  define PDETECT_CPU_TYPE_NAME "intel"
#  define PDETECT_CPU_TYPE      PDETECT_CPU_TYPECODE_INTEL

#  define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_IA64
#  define PDETECT_CPU_BITS      64

#  define PDETECT_CPU_DETECTED 1
#endif /*Intel itanium IA-64*/

/* MIPS */
#if (defined(__mips__) || defined(mips) || defined(__mips) || defined(__MIPS__)) \
    && !defined(PDETECT_CPU_DETECTED)
#  define PDETECT_PLATFORM_32BIT
#  define PDETECT_CPU_MIPS
#  define PDETECT_CPUARCH_MIPS
#  define PDETECT_CPU_ARCH MIPS

#  define PDETECT_CPU_TYPE_NAME "mips"
#  define PDETECT_CPU_TYPE     PDETECT_CPU_TYPECODE_MIPS

#  define PDETECT_CPUARCH_TYPE_NAME "mips"
#  define PDETECT_CPUARCH_TYPE PDETECT_CPU_ARCHCODE_MIPS

#  define PDETECT_CPU_BITS     32

#define PDETECT_CPU_DETECTED 1
#endif /*MIPS*/


/*PowerPC*/
#if (defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__powerpc64__) \
    || defined(__ppc__) || defined(__ppc64__) || defined(__PPC__) || defined(__PPC64__) || defined(_ARCH_PPC) \
    || defined(_ARCH_PPC64) || defined(_M_PPC)) \
    && !defined(PDETECT_CPU_DETECTED)
#  define PDETECT_CPU_POWERPC
#  define PDETECT_CPUARCH_POWERPC
#  define PDETECT_CPU_ARCH     POWERPC

#  if defined(_ARCH_PPC64) || defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__)
#    define PDETECT_PLATFORM_64BIT
#    define PDETECT_CPU_BITS   64
#  else /*PPC64*/
#    define PDETECT_PLATFORM_32BIT
#    define PDETECT_CPU_BITS   32
#  endif /*PPC64*/

#  define PDETECT_CPU_TYPE_NAME "powerpc"
#  define PDETECT_CPU_TYPE     PDETECT_CPU_TYPECODE_POWERPC

#  define PDETECT_CPUARCH_TYPE_NAME "powerpc"
#  define PDETECT_CPUARCH_TYPE PDETECT_CPU_ARCHCODE_POWERPC

#define PDETECT_CPU_DETECTED 1
#endif /*PowerPC*/

/*TMS320*/
#if (defined(_TMS320C2XX) || defined(__TMS320C2000__) || defined(_TMS320C5X) \
    || defined(__TMS320C55X__) || defined(_TMS320C6X) || defined(__TMS320C6X__))
    && !defined(PDETECT_CPU_DETECTED)
#  define PDETECT_CPU_TMS320
#  define PDETECT_CPUARCH_TMS320
#  define PDETECT_CPU_ARCH     TMS320

#  define PDETECT_PLATFORM_32BIT
#  define PDETECT_CPU_BITS     32

#  define PDETECT_CPU_TYPE_NAME "tms320"
#  define PDETECT_CPU_TYPE     PDETECT_CPU_TYPECODE_TMS320

#  define PDETECT_CPUARCH_TYPE_NAME "tms320"
#  define PDETECT_CPUARCH_TYPE PDETECT_CPU_ARCHCODE_TMS320

#  define PDETECT_CPU_DETECTED 1
#endif /*TMS320*/

/* Intel 8051 */
#if defined(__ICC8051__)
  #define PDETECT_CPU_INTEL8051
  #define PDETECT_CPU_TYPE_NAME "intel"
  #define PDETECT_CPU_TYPE  PDETECT_CPU_TYPECODE_INTEL
  #define PDETECT_CPUARCH_TYPE_NAME "8051"
  #define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_8051
#endif /*__ICC8051__*/

/* ARM */
#if (defined (_M_ARM) || defined(__ARM_ARCH) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) \
    || defined(_ARM) || defined(_M_ARMT) || defined(__arm) || defined(__ICCARM__)) \
    && !defined(PDETECT_CPU_DETECTED)

#  define PDETECT_CPU_ARM
#  define PDETECT_CPU_TYPE_NAME "arm"
#  define PDETECT_CPU_TYPE   PDETECT_CPU_TYPECODE_ARM

#  if defined(__ARM_ARCH_3__)
#    define PDETECT_CPU_ARCH ARM3
#    define PDETECT_CPUARCH_ARM3
#    define PDETECT_CPUARCH_TYPE_NAME "armv3"
#    define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_ARM3
#  endif /*__ARM_ARCH_3__*/

#  if defined(__ARM_ARCH_3M__)
#    define PDETECT_CPU_ARCH ARM3M
#    define PDETECT_CPUARCH_ARM3M
#    define PDETECT_CPUARCH_TYPE_NAME "armv3m"
#    define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_ARM3M
#  endif /*__ARM_ARCH_3M__*/

#  if defined(__ARM_ARCH_5__) || (_M_ARM==5)
#    define PDETECT_CPU_ARCH ARM5
#    define PDETECT_CPUARCH_ARM5
#    define PDETECT_CPUARCH_TYPE_NAME "armv5"
#    define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_ARM5
#  endif /*__ARM_ARCH_5__*/

#  if defined(__ARM_ARCH_5E__) || (_M_ARM==5)
#    define PDETECT_CPU_ARCH ARM5E
#    define PDETECT_CPUARCH_ARM5E
#    define PDETECT_CPUARCH_TYPE_NAME "armv5e"
#    define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_ARM5E
#  endif /*__ARM_ARCH_5E__*/

#  if defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) \
    || defined(__ARM_ARCH_6ZK__) || (_M_ARM==6)
#    define PDETECT_CPU_ARCH ARM6
#    define PDETECT_CPUARCH_ARM6
#    define PDETECT_CPUARCH_TYPE_NAME "armv6"
#    define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_ARM6
#endif /*ARM6*/

#  if defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) \
    || defined(__ARM_ARCH_7S__) || (_M_ARM==7) || defined(_M_ARM_ARMV7VE)
#    define PDETECT_CPU_ARCH ARM7
#    define PDETECT_CPUARCH_ARM7
#    define PDETECT_CPUARCH_TYPE_NAME "armv7"
#    define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_ARM7
#endif /*ARM7*/

/* ARM64 */
#  if defined(__aarch64__) || defined(__ARM_ARCH_ISA_A64) || defined(_M_ARM64) /* arm 64 bit ? */
#    define PDETECT_PLATFORM_64BIT
#    define PDETECT_CPU_BITS   64
#    define PDETECT_CPUARCH_TYPE_NAME "arm64"
#    define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_ARM64

#    define PDETECT_CPU_DETECTED 1
#  endif /*__aarch64__*/

/* ARM32 */
#  if (defined(__arm__) || defined(_M_ARM)) && !defined(__arch64__) && !defined(__ARM_ARCH_ISA_A64) && !defined(_M_ARM64) /* arm 32 bit? */
#    define PDETECT_PLATFORM_32BIT
#    define PDETECT_CPU_BITS   32

#    define PDETECT_CPU_DETECTED 1
#  endif /*__arm*__*/

#endif /*_M_ARM || __ARM_ARCH*/

/* Alpha */
#  if (defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)) && !defined(PDETECT_CPU_DETECTED)

#    if defined(__alpha_ev4__) || (_M_ALPHA==4)
#      define PDETECT_CPU_ARCH ALPHAEV4

#      define PDETECT_CPUARCH_TYPE_NAME "alphaev4"
#      define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_ALPHAEV4

#    endif /*__alpha_ev4__*/

#    if defined(__alpha_ev5__) || (_M_ALPHA==5)
#      define PDETECT_CPU_ARCH ALPHAEV5
#      define PDETECT_CPUARCH_ALPHAEV5
#      define PDETECT_CPUARCH_TYPE_NAME "alphaev5"
#      define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_ALPHAEV5
#    endif /*__alpha_ev5__*/

#    if defined(__alpha_ev6__) || (_M_ALPHA==6)
#      define PDETECT_CPU_ARCH ALPHAEV6
#      define PDETECT_CPUARCH_ALPHAEV6
#      define PDETECT_CPUARCH_TYPE_NAME "alphaev6"
#      define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_ALPHAEV6
#    endif /*__alpha_ev6__*/

#    define PDETECT_PLATFORM_32BIT
#    define PDETECT_CPU_ALPHA
#    define PDETECT_CPU_TYPE_NAME "alpha"
#    define PDETECT_CPU_TYPE   PDETECT_CPU_TYPECODE_ALPHA
#    define PDETECT_CPU_BITS   32

#    define PDETECT_CPU_DETECTED 1
#  endif /*alpha*/

/* Intel x86 */
#if (defined(__386__) || defined(_M_I386) || defined(__i386) || defined(_M_IX86) \
    || defined(_X86_) || defined(__I86__) || defined(__X86__) || defined(__i686__) || defined(__i586__) || defined(__i486__)) \
    && !defined(PDETECT_CPU_DETECTED)
#  define PDETECT_CPU_ARCH  IA32
#  define PDETECT_CPUARCH_IA32
#  define PDETECT_CPU_INTEL
#  define PDETECT_PLATFORM_32BIT

#  define PDETECT_CPU_TYPE_NAME "intel"
#  define PDETECT_CPU_TYPE      PDETECT_CPU_TYPECODE_INTEL

#  define PDETECT_CPUARCH_TYPE_NAME "ia32"
#  define PDETECT_CPUARCH_TYPE  PDETECT_CPU_ARCHCODE_IA32
#  define PDETECT_CPU_BITS      32

#  define PDETECT_CPU_DETECTED 1
#endif /* Intel X86 */

////////////////// Platform detection end ////////////////////
//////////////////////////////////////////////////////////////

/* Compiler detection */
/* 
 1. Defined always. Only signle value for each macro
    PDETECT_COMPILER_TYPE - one of values PDETECT_COMPILER_CODE_xxxx


 2. Defined when platform is correspond. Multiple definitions are possible
    Compiler types:
      PDETECT_COMPILER_MSVC  - Windows MS Visual C/C++ compiler
      PDETECT_COMPILER_GCC   - GCC-compatible compiler (macro is present for clang, llvm, webassembly, mingw)
      PDETECT_COMPILER_IBM   - IBM C++ compiler
      PDETECT_COMPILER_ICC   - ICC Intel C++ compiler
      PDETECT_COMPILER_MINGW - MinGW compiler
      PDETECT_COMPILER_CLANG - clang compiler
      PDETECT_COMPILER_SOLARIS - SUN c++ compiler
      PDETECT_COMPILER_XLC   - XLC compiler
      PDETECT_COMPILER_WEBASSEMBLY - emscript webassembly c++ compiler (clang)

      PDETECT_COMPILER_GCC_COMPAT  - GCC compatibility compiler

    Features:
      PDETECT_COMPILER_SUPPORT_RTTI     - RTTI is available
      PDETECT_COMPILER_CHAR_UNSIGNED    - chars are unsigned
      PDETECT_COMPILER_SUPPORT_EXCEPTIONS - compiler supports exceptions

      PDETECT_COMPILER_KERNEL_MODE      - kernelmode build (Windows MS Visual C/C++ compiler only)


 3. Specific compiler versions and info
    PDETECT_COMPILER_MSVC_VERSION - defined only with PDETECT_COMPILER_MSVC. Describes VisualStudio version which used with compiler. Integer value
      values:
           100 = MS Visual Studio v.1
           700 = MS Visual Studio v.7
           701 = MS Visual Studio v.7.1
          1600 = MS Visual Studio v.16 (2019)
          1605 = MS Visual Studio v.16.5, etc
    PDETECT_COMPILER_MSVC_CLR - defined only with PDETECT_COMPILER_MSVC. CLR/CLI is available. Otherwise not defined. No value
    PDETECT_COMPILER_GCC_VERSION  - GCC compiler compatibility version level. Integer value.
      values: 30200 for 3.2.0, etc

    PDETECT_COMPILER_CLANG_VERSION - clang compiler version level. Integer value
      values: 10100 for 1.1.0, etc

    PDETECT_COMPILER_MINGW_VERSION - MinGW compiler version level. Integer value
      values: 701 for 7.1, etc

 */

#ifdef _MSC_VER
#  define PDETECT_COMPILER_MSVC
#  define PDETECT_COMPILER_TYPE   PDETECT_COMPILER_CODE_MSVC

/* PDETECT_COMPILER_MSVC_VERSION describes VisualStudio version which used with compiler.
   100 = MS Visual Studio v.1
   700 = MS Visual Studio v.7
   701 = MS Visual Studio v.7.1
  1600 = MS Visual Studio v.16 (2019)
  1605 = MS Visual Studio v.16.5, etc
*/

#  if _MSC_VER==800
#    define PDETECT_COMPILER_MSVC_VERSION 100
#  elif _MSC_VER==900
#    define PDETECT_COMPILER_MSVC_VERSION 300
#  elif _MSC_VER==1000
#    define PDETECT_COMPILER_MSVC_VERSION 400
#  elif _MSC_VER==1020
#    define PDETECT_COMPILER_MSVC_VERSION 401
#  elif _MSC_VER==1100
#    define PDETECT_COMPILER_MSVC_VERSION 500
#  elif _MSC_VER==1200
#    define PDETECT_COMPILER_MSVC_VERSION 600
#  elif _MSC_VER==1300
#    define PDETECT_COMPILER_MSVC_VERSION 700
#  elif _MSC_VER==1310
#    define PDETECT_COMPILER_MSVC_VERSION 701
#  elif _MSC_VER==1400
#    define PDETECT_COMPILER_MSVC_VERSION 800
#  elif _MSC_VER==1500
#    define PDETECT_COMPILER_MSVC_VERSION 900
#  elif _MSC_VER==1600
#    define PDETECT_COMPILER_MSVC_VERSION 1000
#  elif _MSC_VER==1700
#    define PDETECT_COMPILER_MSVC_VERSION 1100
#  elif _MSC_VER==1800
#    define PDETECT_COMPILER_MSVC_VERSION 1200
#  elif _MSC_VER==1900
#    define PDETECT_COMPILER_MSVC_VERSION 1400
#  elif _MSC_VER==1910
#    define PDETECT_COMPILER_MSVC_VERSION 1500
#  elif _MSC_VER==1911
#    define PDETECT_COMPILER_MSVC_VERSION 1503
#  elif _MSC_VER==1912
#    define PDETECT_COMPILER_MSVC_VERSION 1505
#  elif _MSC_VER==1913
#    define PDETECT_COMPILER_MSVC_VERSION 1506
#  elif _MSC_VER==1914
#    define PDETECT_COMPILER_MSVC_VERSION 1507
#  elif _MSC_VER==1915
#    define PDETECT_COMPILER_MSVC_VERSION 1508
#  elif _MSC_VER==1916
#    define PDETECT_COMPILER_MSVC_VERSION 1509
#  elif _MSC_VER==1920
#    define PDETECT_COMPILER_MSVC_VERSION 1600
#  elif _MSC_VER==1921
#    define PDETECT_COMPILER_MSVC_VERSION 1601
#  elif _MSC_VER==1922
#    define PDETECT_COMPILER_MSVC_VERSION 1602
#  elif _MSC_VER==1923
#    define PDETECT_COMPILER_MSVC_VERSION 1603
#  elif _MSC_VER==1924
#    define PDETECT_COMPILER_MSVC_VERSION 1604
#  elif _MSC_VER==1925
#    define PDETECT_COMPILER_MSVC_VERSION 1605
#  elif _MSC_VER==1926
#    define PDETECT_COMPILER_MSVC_VERSION 1606
#  elif _MSC_VER==1927
#    define PDETECT_COMPILER_MSVC_VERSION 1607
#  elif _MSC_VER==1928 && _MSC_FULL_VER < 192829500
#    define PDETECT_COMPILER_MSVC_VERSION 1608
#  elif _MSC_VER==1928 && _MSC_FULL_VER >= 192829500
#    define PDETECT_COMPILER_MSVC_VERSION 1609
#  elif _MSC_VER==1929
#    define PDETECT_COMPILER_MSVC_VERSION 1610
#  endif /*_MSC_VER*/

#  if !defined(PDETECT_COMPILER_MSVC_VERSION)
#    if _MSVC_VER > 1929
#      define PDETECT_COMPILER_MSVC_VERSION 1929
#    elif _MSVC_VER > 1610 /*_MSVC_VER > 1929*/
#      define PDETECT_COMPILER_MSVC_VERSION 1610
#    else
#      define PDETECT_COMPILER_MSVC_VERSION 1000
#    endif /*_MSVC_VER > 1929*/
#  endif

#  if defined(__CLR_VER) && defined(__cplusplus_cli)
#    define PDETECT_COMPILER_MSVC_CLR
#  endif /*defined(__CLR_VER) && defined(__cplusplus_cli)*/

#  if defined(_CHAR_UNSIGNED)
#    define PDETECT_COMPILER_CHAR_UNSIGNED
#  endif /*_CHAR_UNSIGNED*/

#  if defined(_CPPRTTI)
#    define PDETECT_COMPILER_SUPPORT_RTTI
#  endif /*_CPPRTTI*/

#  if defined(__INTELLISENSE__)
#    define PDETECT_COMPILER_INTELLISENSE_MODE
#  endif /*__INTELLISENSE__*/

#  if defined(_KERNEL_MODE)
#    define PDETECT_COMPILER_KERNEL_MODE
#  endif /*_KERNEL_MODE*/

#  if defined(_CPPUNWIND)
#    define PDETECT_COMPILER_SUPPORT_EXCEPTIONS
#  endif /*_CPPUNWIND*/

#endif  /*_MSC_VER*/

#ifdef __GNUC__
#  define PDETECT_COMPILER_GCC
#  define PDETECT_COMPILER_TYPE   PDETECT_COMPILER_CODE_GCC

/* GCC version. 30200 for 3.2.0, etc */
#  define PDETECT_COMPILER_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNU_PATCHLEVEL__)

#  ifdef __CHAR_UNSIGNED__
#    define PDETECT_COMPILER_CHAR_UNSIGNED
#  endif /*__CHAR_UNSIGNED__*/

#  ifdef __GXX_RTTI
#    define PDETECT_COMPILER_SUPPORT_RTTI
#  endif /*__GXX_RTTI*/

#  ifdef __EXCEPTIONS
#    define PDETECT_COMPILER_SUPPORT_EXCEPTIONS
#  endif /*__EXCEPTIONS*/

#endif  /*__GNUC__*/

/* IBM XL C/C++ compiler legacy version: linux, z/OS, etc */
#if defined(__IBMCPP__) || defined(__IBMC__)
#  define PDETECT_COMPILER_IBMXLC
#  define PDETECT_COMPILER_TYPE   PDETECT_COMPILER_CODE_IBMXLC
#endif  /*__IBMCPP__*/

/* Intel C++ compiler */
#if defined(__ICC) || defined(__INTEL_COMPILER)
#  define PDETECT_COMPILER_ICC
#  define PDETECT_COMPILER_TYPE   PDETECT_COMPILER_CODE_ICC
#endif  /* defined(__ICC) || defined(__INTEL_COMPILER) */

/* SUN compiler */
#if defined(__SUNPRO_CC) || defined(__SUNPRO_C)
#  define PDETECT_COMPILER_SOLARIS
#  define PDETECT_COMPILER_TYPE   PDETECT_COMPILER_CODE_SOLARIS
#endif  /* defined(__SUNPRO_CC) || defined(__SUNPRO_C) */

/* MinGW compiler */
#if defined(__MINGW32__) || defined(__MINGW64__)
#  undef  PDETECT_COMPILER_TYPE

#  define PDETECT_COMPILER_MINGW
#  define PDETECT_COMPILER_TYPE   PDETECT_COMPILER_CODE_MINGW

#  if defined (__MINGW32__)
#    define PDETECT_COMPILER_MINGW_VERSION (__MINGW32_VERSION_MAJOR * 100 + __MINGW32_MINOR_VERSION)
#  elif defined(__MINGW64__)
#    define PDETECT_COMPILER_MINGW_VERSION (__MINGW64_VERSION_MAJOR * 100 + __MINGW64_MINOR_VERSION)
#  endif /*__MINGW64__*/

#endif  /* defined(__MINGW32__) || defined(__MINGW64__) */

/* clang compiler */
#if defined(__clang__)
#  define PDETECT_COMPILER_CLANG

#  undef PDETECT_COMPILER_TYPE
#  define PDETECT_COMPILER_TYPE   PDETECT_COMPILER_CODE_CLANG

#  define PDETECT_COMPILER_CLANG_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)

#  if __has_feature(cxx_rtti)
#    define PDETECT_COMPILER_SUPPORT_RTTI
#  endif /*__has_feature(cxx_rtti)*/

#  if __has_feature(cxx_exceptions)
#    define PDETECT_COMPILER_SUPPORT_EXCEPTIONS
#  endif /*__has_feature(cxx_exceptions)*/

#endif /*__clang__*/

/* IBM XL C/C++ compiler (non z/OS, legacy or clang based) */
/*#if defined(__xlc__) || defined(__xlC__) || defined(__ibmxl__)
#  define PDETECT_COMPILER_IBMXLC
#  define PDETECT_COMPILER_TYPE   PDETECT_COMPILER_CODE_IBMXLC
#endif */ /* __xlc__ */

/* WebAssembly compiler */
#if defined(__EMSCRIPTEN__)
#  undef  PDETECT_COMPILER_TYPE

#  define PDETECT_COMPILER_WEBASSEMBLY
#  define PDETECT_COMPILER_TYPE   PDETECT_COMPILER_CODE_WEBASSEMBLY
#endif /*__EMSCRIPTEN__*/

#if defined(__IAR_SYSTEMS_ICC__)
#  undef  PDETECT_COMPILER_TYPE

#  define PDETECT_COMPILER_IAR
#  define PDETECT_COMPILER_TYPE  PDETECT_COMPILER_CODE_IAR

  /* PDETECT_COMPILER_IAR_VERSION returns integer value, e.g. 5011003 - v.5.11.3 */
  #ifdef __ICC8051__
    /* 8051 compiler has version 501 - 5.01, translate it to wide format like in ARM compiler */
    #define  PDETECT_COMPILER_IAR_VERSION ((__VER__/100) * 1000) + (__VER__ % 100))
  #elif defined __ICCARM__
    #define  PDETECT_COMPILER_IAR_VERSION (__VER__)
  #else
    #define PDETECT_COMPILER_IAR_VERSION  0
  #endif /*__ICC8051__*/

#endif /*__IAR_SYSTEMS_ICC__*/

/* Old and not so popular compilers: check that compiler was not detected */



#if defined(__LCC__)
  #define PDETECT_COMPILER_LCC
  #define PDETECT_COMPILER_TYPE  PDETECT_COMPILER_CODE_LCC
#endif /*__LCC__*/

#if defined(__llvm__)
  #define PDETECT_COMPILER_LLVM
#endif /*__llvm__*/

#if defined(__MWERKS__)
  #define PDETECT_COMPILER_METROWERKS
  #define PDETECT_COMPILER_TYPE  PDETECT_COMPILER_CODE_METROWERKS
#endif /*__MWERKS__*/

#if defined(__BORLANDC__)
  #undef  PDETECT_COMPILER_TYPE

  #define PDETECT_COMPILER_BORLANDC
  #define PDETECT_COMPILER_TYPE PDETECT_COMPILER_CODE_BORLANDC

#endif /*__BORLANDC__*/

#if defined(__TURBOC__)
  #define PDETECT_COMPILER_TURBOC
  #define PDETECT_COMPILER_TYPE PDETECT_COMPILER_CODE_TURBOC
#endif /*__TURBOC__*/

#if defined(__WATCOMC__)
  #define PDETECT_COMPILER_WATCOM
  #define PDETECT_COMPILER_TYPE PDETECT_COMPILER_CODE_WATCOM
#endif /*__WATCOMC__*/

#if defined(__CA__) && defined(__KEIL__)
 /* keil ARM */
  #define PDETECT_COMPILER_KEIL
  #define PDETECT_COMPILER_TYPE PDETECT_COMPILER_CODE_KEIL
#endif /*__CA__  && defined(__KEIL__)*/

#if defined(__C166__)
 /* KEIL C166 */
  #define PDETECT_COMPILER_KEIL
  #define PDETECT_COMPILER_TYPE PDETECT_COMPILER_CODE_KEIL
#endif /*__C166__*/

#if defined(__C51__) && defined (__CX51__)
 /* KEIL C51 */
  #define PDETECT_COMPILER_KEIL
  #define PDETECT_COMPILER_TYPE PDETECT_COMPILER_CODE_KEIL
#endif /*defined(__C51__) && defined (__CX51__)*/

#if defined(__DJGPP__)
  #define PDETECT_COMPILER_DJGPP
  #define PDETECT_COMPILER_TYPE PDETECT_COMPILER_CODE_DJGPP
#endif /*__DJGPP__*/

#if defined(__DMC__)
  #define PDETECT_COMPILER_DIGITALMARS
  #define PDETECT_COMPILER_TYPE PDETECT_COMPILER_CODE_DIGITALMARS
#endif /*__DMC__*/

#if defined(PDETECT_COMPILER_GCC) || defined(PDETECT_COMPILER_CLANG) || defined(PDETECT_COMPILER_XLC) || defined(PDETECT_COMPILER_WEBASSEMBLY) || defined(PDETECT_COMPILER_MINGW)
#define PDETECT_COMPILER_GCC_COMPAT
#endif /*defined(PDETECT_COMPILER_GCC) || defined(PDETECT_COMPILER_CLANG) || defined(PDETECT_COMPILER_XLC) || defined(PDETECT_COMPILER_WEBASSEMBLY) || defined(PDETECT_COMPILER_MINGW)*/




/* Instruction set detect */
#if defined(__AVX__)
#  define PDETECT_COMPILER_SUPPORT_AVX
#endif /*__AVX__*/

#if defined(__AVX2__)
#  define PDETECT_COMPILER_SUPPORT_AVX2
#endif /*__AVX2__*/

#if defined(__AVX512BW__) || defined(__AVX512CD__) || defined(__AVX512DQ__) || defined(__AVX512F__) || defined(__AVX512VL__)
#  define PDETECT_COMPILER_SUPPORT_AVX512
#endif /*if defined(__AVX512BW__) || defined(__AVX512CD__) || defined(__AVX512DQ__) || defined(__AVX512F__) || defined(__AVX512VL__)*/

#if defined(__SSE__)
#  define PDETECT_COMPILER_SUPPORT_SSE
#endif /*__SSE__*/

#if defined(__SSE2__)
#  define PDETECT_COMPILER_SUPPORT_SSE2
#endif /*__SSE2__*/

#if defined(__SSE3__)
#  define PDETECT_COMPILER_SUPPORT_SSE3
#endif /*__SSE3__*/

#if defined(__SSE4_1__)
#  define PDETECT_COMPILER_SUPPORT_SSE41
#endif /*__SSE4_1__*/

#if defined(__SSE4_2__)
#  define PDETECT_COMPILER_SUPPORT_SSE42
#endif /*__SSE4_2__*/


/* C++ version detect */
/* 
 1. Defined always. Only signle value for each macro
    PDETECT_CPP_VERSION - contains version number. 0 - if c++ is not supported or C++98/03. 11 - c++11, 14 - c++14, 17 - c++17, 20 - c++2a

    PDETECT_CPP_X11 = 1   when c++11 is supported, 0 - is not supported
    PDETECT_CPP_X14 = 1   when c++14 is supported, 0 - is not supported
    PDETECT_CPP_X17 = 1   when c++17 is supported, 0 - is not supported
    PDETECT_CPP_X2A = 1   when c++2a is supported, 0 - is not supported

 2. Defined when platform is correspond. Multiple definitions are possible
    PDETECT_CPP_HAS_X11  - defined when compiler has c++11 support
    PDETECT_CPP_HAS_X14  - defined when compiler has c++14 support
    PDETECT_CPP_HAS_X17  - defined when compiler has c++17 support
    PDETECT_CPP_HAS_X2A  - defined when compiler has c++2a support
 */
#undef  PDETECT_CPP_VERSION
#define PDETECT_CPP_VERSION  0

#ifdef PDETECT_CPP

#  if (__cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1900) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
#    define PDETECT_CPP_X11  1
#    define PDETECT_CPP_HAS_X11

#    undef  PDETECT_CPP_VERSION
#    define PDETECT_CPP_VERSION  11
#  else /*(__cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1900) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)*/
#    define PDETECT_CPP_X11  0
#  endif /*(__cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1900) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)*/

#  if (__cplusplus >= 201402L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201402L)) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201402L))
#    define PDETECT_CPP_X14  1
#    define PDETECT_CPP_HAS_X14

#    undef  PDETECT_CPP_VERSION
#    define PDETECT_CPP_VERSION  14
#  else /*(__cplusplus >= 201402L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201402L)) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201402L))*/
#    define PDETECT_CPP_X14  0
#  endif /*(__cplusplus >= 201402L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201402L)) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201402L))*/

#  if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201710L))
#    define PDETECT_CPP_X17  1
#    define PDETECT_CPP_HAS_X17

#    undef  PDETECT_CPP_VERSION
#    define PDETECT_CPP_VERSION  17
#  else /*(__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201710L))*/
#    define PDETECT_CPP_X17  0
#  endif /*(__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201710L))*/

#  if (__cplusplus > 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG > 201703L)) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ > 201710L))
#    define PDETECT_CPP_X2A  1
#    define PDETECT_CPP_HAS_X2A

#    undef  PDETECT_CPP_VERSION
#    define PDETECT_CPP_VERSION  20
#  else /*(__cplusplus > 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG > 201703L)) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ > 201710L))*/
#    define PDETECT_CPP_X2A  0
#  endif /*(__cplusplus > 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG > 201703L)) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ > 201710L))*/


#endif /*PDETECT_CPP*/





#if (!defined(PDETECT_PLATFORM_WINDOWS) && !defined(PDETECT_PLATFORM_POSIX_BASED) && !defined(PDETECT_PLATFORM_APPLE)) || \
    (defined(PDETECT_PLATFORM_WINDOWS) && defined(PDETECT_PLATFORM_POSIX_BASED))
#error "Unsupported platform detected"
#endif  //(!defined(PDETECT_PLATFORM_WINDOWS) && !defined(PDETECT_PLATFORM_POSIX_BASED)) ||
//(defined(PDETECT_PLATFORM_WINDOWS) && defined(PDETECT_PLATFORM_POSIX_BASED))

#endif /*PDETECT_HEADER*/
