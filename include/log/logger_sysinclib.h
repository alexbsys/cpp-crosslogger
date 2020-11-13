
#ifndef LOGGER_SYSTEM_INCLUDES_AND_LIBRARIES_HEADER
#define LOGGER_SYSTEM_INCLUDES_AND_LIBRARIES_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"

////////////////////////////// Includes and libs START //////////////////////////////

#define LOG_INCLUDE_DBGHELP 0

#if (LOG_AUTO_DEBUGGING || LOG_UNHANDLED_EXCEPTIONS) && !LOG_INCLUDE_DBGHELP
#undef LOG_INCLUDE_DBGHELP
#define LOG_INCLUDE_DBGHELP 1
#endif  //(LOG_AUTO_DEBUGGING || LOG_UNHANDLED_EXCEPTIONS) && !INCLUDE_DBGHELP

#ifdef LOG_COMPILER_MSVC
#pragma warning(disable : 4996)
#endif  // LOG_COMPILER_MSVC

#include <stdio.h>
#include <string>
#include <sstream>
#include <ostream>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <time.h>
#include <sys/types.h>
#ifndef LOG_PLATFORM_ANDROID
#include <sys/timeb.h>
#endif /*LOG_PLATFORM_ANDROID*/
#include <list>
#include <string.h>
#include <stdlib.h>

#if LOG_RTTI_ENABLED
#include <typeinfo>
#endif  // LOG_RTTI_ENABLED

#if LOG_CHECKED
#include <assert.h>
#endif  // LOG_CHECKED

#ifdef LOG_PLATFORM_WINDOWS
#include <windows.h>
#include <Sddl.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")
#else  // LOGGER_PLATFORM_WINDOWS

#ifdef LOG_HAVE_UNISTD_H
#include <unistd.h>
#endif  // LOG_HAVE_UNISTD_H

#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <sys/time.h>

#ifdef LOG_HAVE_SYS_SYSCALL_H
#include <sys/syscall.h>
#endif  // LOG_HAVE_SYS_SYSCALL_H

#if LOG_MULTITHREADED && defined(LOG_HAVE_PTHREAD)
#include <pthread.h>
#endif  // LOG_MULTITHREADED && defined(LOG_HAVE_PTHREAD)

#ifdef LOG_PLATFORM_MAC
#  include <mach-o/dyld.h>
#endif // defined(LOG_PLATFORM_MAC)

#if LOG_USE_SYSTEMINFO
#ifdef LOG_HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif  // LOG_HAVE_SYS_UTSNAME_H

#ifdef LOG_HAVE_PWD_H
#include <pwd.h>
#endif  // LOG_HAVE_PWD_H

#ifdef LOG_HAVE_SYS_SYSINFO_H
#include <sys/sysinfo.h>
#endif  // LOG_HAVE_SYS_SYSINFO_H
#endif  // LOG_USE_SYSTEMINFO

#endif  // LOG_PLATFORM_WINDOWS

#ifdef LOG_PLATFORM_WINDOWS

#if LOG_USE_MODULEDEFINITION
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "version.lib")
#endif  // LOG_USE_MODULEDEFINITION

#if LOG_INCLUDE_DBGHELP
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif  // LOG_INCLUDE_DBGHELP

#endif  // LOG_PLATFORM_WINDOWS

#if LOG_ANDROID_SYSLOG
#include <android/log.h>
#endif  // LOG_ANDROID_SYSLOG

#if !defined(LOG_PLATFORM_WINDOWS) && !defined(LOG_PLATFORM_MAC) && LOG_USE_MODULEDEFINITION
#include <dlfcn.h>
#include <link.h>
#endif  //! defined(LOG_PLATFORM_WINDOWS) && !defined(LOG_PLATFORM_MAC) && LOG_USE_MODULEDEFINITION

#if !defined(LOG_PLATFORM_WINDOWS) && LOG_AUTO_DEBUGGING
#include <cxxabi.h>
#include <execinfo.h>
#endif  //! defined(LOG_PLATFORM_WINDOWS) && LOG_AUTO_DEBUGGING

#if !defined(LOG_PLATFORM_WINDOWS) && LOG_UNHANDLED_EXCEPTIONS
#include <signal.h>
#include <ucontext.h>
#endif  //! defined(LOG_PLATFORM_WINDOWS) && LOG_UNHANDLED_EXCEPTIONS

//#endif /*defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))*/

////////////////////////////// Includes and libs END //////////////////////////////

#endif /*LOGGER_SYSTEM_INCLUDES_AND_LIBRARIES_HEADER*/
