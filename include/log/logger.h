
// Smart logger library for C/C++
/// alexb 2009-2020

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// LOG MACROSES:
// $(yyyy) $(yy) - year
// $(MM) $(M) - month
// $(dd) $(d) - day
// $(hh) $(h) - hour
// $(mm) $(m) - min
// $(ss) $(s) - sec
// $(ttt) $(t) - milliseconds
// $(V) $(v) - verbose
// $(MODULE) - current module name full
// $(module) - current module name short
// $(function) - function name
// $(line) - line number
// $(srcfile) - source file
// $(PID) - process ID
// $(TID) - thread ID

// CONFIG PATH & NAMES MACROSES:
// $(SYSTEMPATH) - path to Windows\System
// $(WINDOWSPATH) - path to Windows
// $(COMMONAPPDATA) - path to common application data
// $(USERAPPDATA) - path to current user application data
// $(DESKTOPDIR) - path to desktop
// $(TEMPPATH) - path to temp directory
// $(CURRENTDIR) - current process directory
// $(EXEDIR) - path to process EXE
// $(EXEFILENAME) - process EXE file name without extension
// $(EXEFULLFILENAME) - process EXE file name with extension

// only if LOG_USE_MODULEDEFINITION
// $(MODULEDIR) - module directory
// $(MODULEFILENAME) - module file name without extension
// $(MODULEFULLFILENAME) - module file name with extension

// Sample INI file

// [logger]
// LogPath=$(USERAPPDATA)\$(EXEFILENAME)\Log
// Verbose=255
// HeaderFormat=$(V)|$(dd).$(MM).$(yyyy)|$(hh):$(mm):$(ss).$(ttt) ($(module)!$(function))
// LogFileName=$(EXEFILENAME).log
// LogSysInfo=1
// ScrollFileCount=70
// ScrollFileSize=1638400
// ScrollFileEveryRun=1
// RegistryConfigPath=HKCU\Software\$(EXEFILENAME)\Logging

#ifndef __LOGGER_HEADER
#define __LOGGER_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_pdefs.h"

#ifdef LOG_COMPILER_MSVC
#pragma once
#endif /*LOG_COMPILER_MSVC*/


#include "logger_userdefs.h"

#if !LOG_ENABLED

#else  // LOG_ENABLED

#  include "logger_cfgcheck.h"

#  include "logger_varargs.h"
#  include "logger_get_caller_addr.h"

#  include "logger_verbose.h"


#if defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))
/// Inproc logger implementation

#include "logger_sysinclib.h"

#if LOG_SHARED
#include "logger_shmem.h"
#endif /*LOG_SHARED*/


#include "logger_interfaces.h"
#include "logger_shared_ptr.hpp"

//#define LOG_SET_VERBOSE_LEVEL(l) logging::configurator.set_verbose_level(l)

#if LOG_USE_OWN_VSNPRINTF
#include "logger_xprintf.h"
#endif //LOG_USE_OWN_VSNPRINTF

#include "logger_strutils.h"

//#if LOG_INI_CONFIGURATION
//#include "logger_ini_parser.h"
//#endif  // LOG_INI_CONFIGURATION

#include "logger_cfgfn.h"
#include "logger_utils.h"

#if LOG_USE_MODULEDEFINITION
//#include "logger_moddef.h"
#endif  // LOG_USE_MODULEDEFINITION


//#if LOG_CONFIGURE_FROM_REGISTRY && defined(LOG_PLATFORM_WINDOWS)
//#include "logger_win_registry.h"

//#endif  //(LOG_CONFIGURE_FROM_REGISTRY || LOG_USE_SYSTEMINFO) && defined(LOG_PLATFORM_WINDOWS)


#if LOG_AUTO_DEBUGGING
//#include "logger_runtime_debugging.h"
#endif  // LOG_AUTO_DEBUGGING

#if LOG_USE_SYSTEMINFO
//#include "logger_sysinfo.h"
#endif  // LOG_USE_SYSTEMINFO

//#include "logger_mt.h"
//#include "logger_plugin_manager.h"


#include "logger_singleton.h"

#include "logger_impl.h"

namespace logging {

////////////////////    Helpers     ////////////////////
//namespace detail {
//  class logger;
//}//namespace detail

extern logger_singleton_interface<logger_interface>* _logger;

//////////////////////////////////////////////////////////////

//namespace detail {

//////////////////////////////////////////////////////////////


//}//namespace detail

 //LogSysInfo
 //Verbose
 //HeaderFormat

 //LogPath
 //LogFileName
 //ScrollFileCount
 //ScrollFileSize
 //ScrollFileEveryRun
 //RegistryConfigPath


#if LOG_TEST_DO_NOT_WRITE_FILE
#include <sstream>
#undef LOG_FLUSH_FILE_EVERY_WRITE
#define LOG_FLUSH_FILE_EVERY_WRITE 1l
#endif  // LOG_TEST_DO_NOT_WRITE_FILE

//////////////////////////////////////////////////////////////
namespace detail {

/**
 * \class    log_stream helper. Implements logger stream interface. User must not use it directly
 */


////////////////////  Logger implementation  //////////////////////////



//////////////////////////////////////////////////////////////

}//namespace detail


};  // namespace logging

///////////////   C tail  ///////////////////////

#else  // defined(__cplusplus) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))


#if LOG_USE_DLL
extern void(LOG_CDECL* __c_logger_log_modules)(int verbose_level, void* caller_addr,
                                               const char* function, const char* file,
                                               int line);
#endif  // LOG_USE_DLL


#if LOG_USE_MODULEDEFINITION
#if !LOG_USE_DLL
void __c_logger_log_modules(int verbose_level, void* caller_addr, const char* function,
                            const char* file, int line);
#endif  //! LOG_USE_DLL
#endif  // LOG_USE_MODULEDEFINITION

#if LOG_USE_DLL
extern void(LOG_CDECL* __c_logger_log_stack_trace)(int verbose_level, void* caller_addr,
                                                   const char* function, const char* file,
                                                   int line);
#endif  // LOG_USE_DLL

//#if LOG_AUTO_DEBUGGING
//#if !LOG_USE_DLL
//void __c_logger_log_stack_trace(int verbose_level, void* caller_addr,
//                                const char* function, const char* file, int line);
//#endif  //! LOG_USE_DLL
//#endif  // LOG_AUTO_DEBUGGING

#if LOG_USE_DLL
extern void(LOG_CDECL* __c_logger_log)(int verbose_level, void* caller_addr,
                                       const char* function, const char* file, int line,
                                       const char* format, ...);
extern void(LOG_CDECL* __c_logger_log_args)(int verbose_level, void* caller_addr,
                                            const char* function, const char* file,
                                            int line, const char* format, va_list args);

extern void(LOG_CDECL* __c_logger_log_cmd)(int cmd_id, int verbose_level, void* caller_addr,
  const char* function, const char* file,
  int line, const void* vparam, int iparam);

extern void(LOG_CDECL* __c_logger_log_cmd_args)(int cmd_id, int verbose_level, void* caller_addr,
  const char* function, const char* file,
  int line, const void* vparam, int iparam, va_list args);

/*
extern void(LOG_CDECL* __c_logger_log_binary)(int verbose_level, void* caller_addr,
                                              const char* function, const char* file,
                                              int line, const char* data, int len);

extern void(LOG_CDECL* __c_logger_set_verbose_level)(int verbose_level);
*/
extern void(LOG_CDECL* __c_logger_objmon_register)(size_t hash_code,
                                                   const char* type_name, void* ptr);
extern void(LOG_CDECL* __c_logger_objmon_unregister)(size_t hash_code, void* ptr);
extern void(LOG_CDECL* __c_logger_objmon_dump)(int verbose_level);

extern void(LOG_CDECL* __c_logger_set_current_thread_name)(const char* thread_name);

extern void(LOG_CDECL* __c_logger_set_config_param)(const char* key, const char* value);
extern int(LOG_CDECL* __c_logger_get_config_param)(const char* key, char* value, int buffer_size);


#else  // LOG_USE_DLL
void __c_logger_log(int verbose_level, void* caller_addr, const char* function,
                    const char* file, int line, const char* format, ...);
void __c_logger_log_args(int verbose_level, void* caller_addr, const char* function,
                         const char* file, int line, const char* format, va_list args);

void __c_logger_log_cmd(int cmd_id, int verbose_level, void* caller_addr, const char* function,
  const char* file, int line, const void* vparam, int iparam);

void __c_logger_log_cmd_args(int cmd_id, int verbose_level, void* caller_addr, const char* function,
  const char* file, int line, const void* vparam, int iparam, va_list args);

//void __c_logger_log_binary(int verbose_level, void* caller_addr, const char* function,
//                           const char* file, int line, const char* data, int len);

//void __c_logger_objmon_register(size_t hash_code, const char* type_name, void* ptr);
//void __c_logger_objmon_unregister(size_t hash_code, void* ptr);
//void __c_logger_objmon_dump(int verbose_level);

void __c_logger_set_config_param(const char* key, const char* value);
int __c_logger_get_config_param(const char* key, char* value, int buffer_size);


void __c_logger_set_current_thread_name(const char* thread_name);

#endif  // LOG_USE_DLL

#if defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#endif  // defined(__cplusplus) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))

#endif  // LOG_ENABLED

#endif  //__LOGGER_HEADER
