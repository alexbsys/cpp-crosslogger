
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

#if LOG_ONLY_DEBUG
#ifdef DEBUG
#undef LOG_ENABLED
#define LOG_ENABLED 0
#endif  // DEBUG
#endif  // LOG_ONLY_DEBUG

#if !LOG_ENABLED
#define LOG_INFO(...)
#define LOG_WARNING(...)
#define LOG_DEBUG(...)
#define LOG_ERROR(...)
#define LOG_FATAL(...)

#define LOG_BINARY_INFO(p, stack_frame)
#define LOG_BINARY_WARNING(p, stack_frame)
#define LOG_BINARY_DEBUG(p, stack_frame)
#define LOG_BINARY_ERROR(p, stack_frame)
#define LOG_BINARY_FATAL(p, stack_frame)

#define LOG_EXCEPTION_DEBUG(e)
#define LOG_EXCEPTION_WARNING(e)
#define LOG_EXCEPTION_INFO(e)
#define LOG_EXCEPTION_ERROR(e)
#define LOG_EXCEPTION_FATAL(e)

#define LOG_EXCEPTION_TEXT_DEBUG(e)
#define LOG_EXCEPTION_TEXT_WARNING(e)
#define LOG_EXCEPTION_TEXT_INFO(e)
#define LOG_EXCEPTION_TEXT_ERROR(e)
#define LOG_EXCEPTION_TEXT_FATAL(e)

#define LOG_SET_CURRENT_THREAD_NAME(name)

#define LOG_MODULES_DEBUG
#define LOG_MODULES_INFO
#define LOG_MODULES_WARNING
#define LOG_MODULES_ERROR
#define LOG_MODULES_FATAL

#define LOG_STACKTRACE_DEBUG
#define LOG_STACKTRACE_INFO
#define LOG_STACKTRACE_WARNING
#define LOG_STACKTRACE_ERROR
#define LOG_STACKTRACE_FATAL

#define DEFINE_LOGGER()

#define LOG_SET_VERBOSE_LEVEL(l)
#define LOG_SET_REG_CONFIG_PATH(p)
#define LOG_SET_INI_CONFIG_PATHS(p)

#define LOGGER_VERBOSE_FATAL 0
#define LOGGER_VERBOSE_ERROR 0
#define LOGGER_VERBOSE_INFO 0
#define LOGGER_VERBOSE_WARNING 0
#define LOGGER_VERBOSE_DEBUG 0
#define LOGGER_VERBOSE_OPTIMAL 0
#define LOGGER_VERBOSE_MUTE 0
#define LOGGER_VERBOSE_FATAL_ERROR 0
#define LOGGER_VERBOSE_ALL 0
#define LOGGER_VERBOSE_NORMAL 0

#else  // LOG_ENABLED

#include "logger_cfgcheck.h"

#include "logger_varargs.h"
#include "logger_get_caller_addr.h"


// Logger verbose level declaration both for C++ and C code. 
// For C++ enum logger_verbose_level is in 'logging' namespace.
// For pure C it is just enum

#if defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))
namespace logging {
#endif  // defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))

enum logger_verbose_level {
  logger_verbose_fatal = 1,
  logger_verbose_error = 2,
  logger_verbose_warning = 4,
  logger_verbose_info = 8,
  logger_verbose_debug = 16,

  logger_verbose_fatal_error = logger_verbose_fatal | logger_verbose_error,
  logger_verbose_all = logger_verbose_fatal | logger_verbose_error |
                       logger_verbose_warning | logger_verbose_info |
                       logger_verbose_debug,

  logger_verbose_normal = logger_verbose_fatal | logger_verbose_error |
                          logger_verbose_warning | logger_verbose_info,
  logger_verbose_mute = 0,

#if defined(_DEBUG) || LOG_PLATFORM_ANDROID
  logger_verbose_optimal = logger_verbose_normal | logger_verbose_debug
#else   //_DEBUG
  logger_verbose_optimal = logger_verbose_normal
#endif  //_DEBUG
}; //enum logger_verbose_level

#if defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))
}// namespace logging
#endif  // defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))

// Declare macroses LOGGER_VERBOSE_XXXX depends on C or C++ compiler
#if defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))
#define LOGGER_VERBOSE_FATAL logging::logger_verbose_fatal
#define LOGGER_VERBOSE_ERROR logging::logger_verbose_error
#define LOGGER_VERBOSE_INFO logging::logger_verbose_info
#define LOGGER_VERBOSE_WARNING logging::logger_verbose_warning
#define LOGGER_VERBOSE_DEBUG logging::logger_verbose_debug
#define LOGGER_VERBOSE_OPTIMAL logging::logger_verbose_optimal
#define LOGGER_VERBOSE_MUTE logging::logger_verbose_mute
#define LOGGER_VERBOSE_FATAL_ERROR logging::logger_verbose_fatal_error
#define LOGGER_VERBOSE_ALL logging::logger_verbose_all
#define LOGGER_VERBOSE_NORMAL logging::logger_verbose_normal
#else  // defined(__cplusplus) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))
#define LOGGER_VERBOSE_FATAL logger_verbose_fatal
#define LOGGER_VERBOSE_ERROR logger_verbose_error
#define LOGGER_VERBOSE_INFO logger_verbose_info
#define LOGGER_VERBOSE_WARNING logger_verbose_warning
#define LOGGER_VERBOSE_DEBUG logger_verbose_debug
#define LOGGER_VERBOSE_OPTIMAL logger_verbose_optimal
#define LOGGER_VERBOSE_MUTE logger_verbose_mute
#define LOGGER_VERBOSE_FATAL_ERROR logger_verbose_fatal_error
#define LOGGER_VERBOSE_ALL logger_verbose_all
#define LOGGER_VERBOSE_NORMAL logger_verbose_normal
#endif  // defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))




#if LOG_USE_MODULEDEFINITION
#define LOG_GET_CALLER_ADDR logging_get_caller_address()
#else
#define LOG_GET_CALLER_ADDR 0L
#endif  // LOG_USE_MODULEDEFINITION

#if defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))
/// Inproc logger implementation

#include "logger_sysinclib.h"

#if LOG_SHARED
#include "logger_shmem.h"
#endif /*LOG_SHARED*/




#define LOG_SET_VERBOSE_LEVEL(l) logging::configurator.set_verbose_level(l)

/*  TODO: remove this after plugins implementation
#if LOG_CONFIGURE_FROM_REGISTRY
#define LOG_SET_REG_CONFIG_PATH(p) logging::configurator.set_reg_config_path(p)
#else  // LOG_CONFIGURE_FROM_REGISTRY
#define LOG_SET_REG_CONFIG_PATH(p)
#endif  // LOG_CONFIGURE_FROM_REGISTRY

#if LOG_INI_CONFIGURATION
#define LOG_SET_INI_CONFIG_PATHS(p) logging::configurator.set_ini_file_find_paths(p)
#else  // LOG_INI_CONFIGURATION
#define LOG_SET_INI_CONFIG_PATHS(p)
#endif  // LOG_INI_CONFIGURATION
*/

#define LOG_CMD(cmdid,v,p,l) \
  logging::_logger->log_cmd((cmdid), (v), LOG_GET_CALLER_ADDR, __FUNCTION__, \
                        __FILE__, __LINE__, (p), (l) )


#define LOG_INFO(...)                                                                    \
  logging::_logger->log(logging::logger_verbose_info, LOG_GET_CALLER_ADDR, __FUNCTION__, \
                        __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)                                                      \
  logging::_logger->log(logging::logger_verbose_debug, LOG_GET_CALLER_ADDR, \
                        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...)                                                      \
  logging::_logger->log(logging::logger_verbose_warning, LOG_GET_CALLER_ADDR, \
                        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)                                                      \
  logging::_logger->log(logging::logger_verbose_error, LOG_GET_CALLER_ADDR, \
                        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)                                                      \
  logging::_logger->log(logging::logger_verbose_fatal, LOG_GET_CALLER_ADDR, \
                        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

#define LOGS_DEBUG() \
  logging::_logger->stream(logging::logger_verbose_debug, LOG_GET_CALLER_ADDR, \
                        __FUNCTION__, __FILE__, __LINE__)

#define LOG_BINARY_INFO(p, l)                                           \
     LOG_CMD(0x1001, logging::logger_verbose_info, (p), (l))
//  logging::_logger->log_binary(logging::logger_verbose_info, LOG_GET_CALLER_ADDR, \
//                               __FUNCTION__, __FILE__, __LINE__, p, stack_frame)
#define LOG_BINARY_DEBUG(p, l) \
     LOG_CMD(0x1001, logging::logger_verbose_debug, (p), (l))


//  logging::_logger->log_binary(logging::logger_verbose_debug, LOG_GET_CALLER_ADDR, \
//                               __FUNCTION__, __FILE__, __LINE__, p, stack_frame)
#define LOG_BINARY_WARNING(p, l)                                           \
     LOG_CMD(0x1001, logging::logger_verbose_warning, (p), (l))


//logging::_logger->log_binary(logging::logger_verbose_warning, LOG_GET_CALLER_ADDR, \
//                               __FUNCTION__, __FILE__, __LINE__, p, stack_frame)

#define LOG_BINARY_ERROR(p, l)                                           \
     LOG_CMD(0x1001, logging::logger_verbose_error, (p), (l))

//logging::_logger->log_binary(logging::logger_verbose_error, LOG_GET_CALLER_ADDR, \
//                               __FUNCTION__, __FILE__, __LINE__, p, stack_frame)

#define LOG_BINARY_FATAL(p, l)                                           \
     LOG_CMD(0x1001, logging::logger_verbose_fatal, (p), (l))

//logging::_logger->log_binary(logging::logger_verbose_fatal, LOG_GET_CALLER_ADDR, \
//                               __FUNCTION__, __FILE__, __LINE__, p, stack_frame)

#define LOG_EXCEPTION_DEBUG(e)                                                        \
  logging::_logger->log_exception(logging::logger_verbose_debug, LOG_GET_CALLER_ADDR, \
                                  __FUNCTION__, __FILE__, __LINE__, e)
#define LOG_EXCEPTION_WARNING(e)                                                        \
  logging::_logger->log_exception(logging::logger_verbose_warning, LOG_GET_CALLER_ADDR, \
                                  __FUNCTION__, __FILE__, __LINE__, e)
#define LOG_EXCEPTION_INFO(e)                                                        \
  logging::_logger->log_exception(logging::logger_verbose_info, LOG_GET_CALLER_ADDR, \
                                  __FUNCTION__, __FILE__, __LINE__, e)
#define LOG_EXCEPTION_ERROR(e)                                                        \
  logging::_logger->log_exception(logging::logger_verbose_error, LOG_GET_CALLER_ADDR, \
                                  __FUNCTION__, __FILE__, __LINE__, e)
#define LOG_EXCEPTION_FATAL(e)                                                        \
  logging::_logger->log_exception(logging::logger_verbose_fatal, LOG_GET_CALLER_ADDR, \
                                  __FUNCTION__, __FILE__, __LINE__, e)


#define LOG_EXCEPTION_TEXT_DEBUG(msg)                                                        \
  logging::_logger->log_exception(logging::logger_verbose_debug, LOG_GET_CALLER_ADDR, \
                                  __FUNCTION__, __FILE__, __LINE__, reinterpret_cast<const char*>(msg))
#define LOG_EXCEPTION_TEXT_WARNING(e)                                                        \
  logging::_logger->log_exception(logging::logger_verbose_warning, LOG_GET_CALLER_ADDR, \
                                  __FUNCTION__, __FILE__, __LINE__, reinterpret_cast<const char*>(msg))
#define LOG_EXCEPTION_TEXT_INFO(e)                                                        \
  logging::_logger->log_exception(logging::logger_verbose_info, LOG_GET_CALLER_ADDR, \
                                  __FUNCTION__, __FILE__, __LINE__, reinterpret_cast<const char*>(msg))
#define LOG_EXCEPTION_TEXT_ERROR(e)                                                        \
  logging::_logger->log_exception(logging::logger_verbose_error, LOG_GET_CALLER_ADDR, \
                                  __FUNCTION__, __FILE__, __LINE__, reinterpret_cast<const char*>(msg))
#define LOG_EXCEPTION_TEXT_FATAL(e)                                                        \
  logging::_logger->log_exception(logging::logger_verbose_fatal, LOG_GET_CALLER_ADDR, \
                                  __FUNCTION__, __FILE__, __LINE__, reinterpret_cast<const char*>(msg))

#define LOG_SET_CURRENT_THREAD_NAME(name) logging::utils::set_current_thread_name(name)

#if LOG_USE_OBJMON
#  define LOG_OBJMON_REGISTER_INSTANCE()                                                   \
    logging::_logger->log_objmon_register(typeid(*this).hash_code(), typeid(*this).name(), \
                                        this)
#  define LOG_OBJMON_UNREGISTER_INSTANCE() \
    logging::_logger->log_objmon_unregister(typeid(*this).hash_code(), this)
#  define LOG_OBJMON_DUMP_INFO() \
    logging::_logger->log_objmon_dump(logging::logger_verbose_info)
#  define LOG_OBJMON_DUMP_DEBUG() \
    logging::_logger->log_objmon_dump(logging::logger_verbose_debug)
#  define LOG_OBJMON_DUMP_WARNING() \
    logging::_logger->log_objmon_dump(logging::logger_verbose_warning)
#  define LOG_OBJMON_DUMP_ERROR() \
    logging::_logger->log_objmon_dump(logging::logger_verbose_error)
#else  // LOG_USER_OBJMON
#  define LOG_OBJMON_REGISTER_INSTANCE()
#  define LOG_OBJMON_UNREGISTER_INSTANCE()
#  define LOG_OBJMON_DUMP_INFO()
#  define LOG_OBJMON_DUMP_DEBUG()
#  define LOG_OBJMON_DUMP_WARNING()
#  define LOG_OBJMON_DUMP_ERROR()
#endif  // LOG_USE_OBJMON

#if LOG_USE_MODULEDEFINITION
#  define LOG_MODULES_DEBUG()                                                           \
     LOG_CMD(0x1002, logging::logger_verbose_debug, NULL, 0)



//  logging::_logger->log_modules(logging::logger_verbose_debug, LOG_GET_CALLER_ADDR, \
//                                __FUNCTION__, __FILE__, __LINE__)
#  define LOG_MODULES_INFO()                                                           \
     LOG_CMD(0x1002, logging::logger_verbose_info, NULL, 0)

//logging::_logger->log_modules(logging::logger_verbose_info, LOG_GET_CALLER_ADDR, \
//                                __FUNCTION__, __FILE__, __LINE__)

#  define LOG_MODULES_WARNING()                                                           \
     LOG_CMD(0x1002, logging::logger_verbose_warning, NULL, 0)

//logging::_logger->log_modules(logging::logger_verbose_warning, LOG_GET_CALLER_ADDR, \
//                                __FUNCTION__, __FILE__, __LINE__)
#  define LOG_MODULES_ERROR()                                                           \
     LOG_CMD(0x1002, logging::logger_verbose_error, NULL, 0)

//logging::_logger->log_modules(logging::logger_verbose_error, LOG_GET_CALLER_ADDR, \
//                                __FUNCTION__, __FILE__, __LINE__)
#  define LOG_MODULES_FATAL()                                                           \
     LOG_CMD(0x1002, logging::logger_verbose_fatal, NULL, 0)

//logging::_logger->log_modules(logging::logger_verbose_fatal, LOG_GET_CALLER_ADDR, \
//                                __FUNCTION__, __FILE__, __LINE__)
#else  // LOG_USE_MODULEDEFINITION
#  define LOG_MODULES_DEBUG()
#  define LOG_MODULES_INFO()
#  define LOG_MODULES_WARNING()
#  define LOG_MODULES_ERROR()
#  define LOG_MODULES_FATAL()
#endif  // LOG_USE_MODULEDEFINITION

#if LOG_AUTO_DEBUGGING
#  define LOG_STACKTRACE_DEBUG()                                                            \
     LOG_CMD(0x1003, logging::logger_verbose_debug, NULL, 0)

//logging::_logger->log_stack_trace(logging::logger_verbose_debug, LOG_GET_CALLER_ADDR, \
//                                    __FUNCTION__, __FILE__, __LINE__)

#  define LOG_STACKTRACE_INFO()                                                            \
     LOG_CMD(0x1003, logging::logger_verbose_info, NULL, 0)
//logging::_logger->log_stack_trace(logging::logger_verbose_info, LOG_GET_CALLER_ADDR, \
//                                    __FUNCTION__, __FILE__, __LINE__)
#  define LOG_STACKTRACE_WARNING()                                                   \
     LOG_CMD(0x1003, logging::logger_verbose_warning, NULL, 0)

//logging::_logger->log_stack_trace(logging::logger_verbose_warning,             \
//                                    LOG_GET_CALLER_ADDR, __FUNCTION__, __FILE__, \
//                                    __LINE__)
#  define LOG_STACKTRACE_ERROR()                                                            \
     LOG_CMD(0x1003, logging::logger_verbose_error, NULL, 0)

//logging::_logger->log_stack_trace(logging::logger_verbose_error, LOG_GET_CALLER_ADDR, \
//                                    __FUNCTION__, __FILE__, __LINE__)

#  define LOG_STACKTRACE_FATAL()                                                            \
     LOG_CMD(0x1003, logging::logger_verbose_fatal, NULL, 0)

//logging::_logger->log_stack_trace(logging::logger_verbose_fatal, LOG_GET_CALLER_ADDR, \
//                                    __FUNCTION__, __FILE__, __LINE__)
#else  // LOG_AUTO_DEBUGGING
#  define LOG_STACKTRACE_DEBUG()
#  define LOG_STACKTRACE_INFO()
#  define LOG_STACKTRACE_WARNING()
#  define LOG_STACKTRACE_ERROR()
#  define LOG_STACKTRACE_FATAL()
#endif  // LOG_AUTO_DEBUGGING

#if LOG_UNHANDLED_EXCEPTIONS && defined(LOG_PLATFORM_WINDOWS)
#  define DEFINE_LOG_UNHANDLED_EXCEPTIONS_MEMBERS \
  LPTOP_LEVEL_EXCEPTION_FILTER                  \
      logging::detail::unhandled_exceptions_processor::prev_exception_filter_ = NULL;
#else  // LOG_UNHANDLED_EXCEPTIONS && defined(LOG_PLATFORM_WINDOWS)
#  define DEFINE_LOG_UNHANDLED_EXCEPTIONS_MEMBERS
#endif  // LOG_UNHANDLED_EXCEPTIONS && defined(LOG_PLATFORM_WINDOWS)

// Logger instance definition macro
#if LOG_SHARED
#  define DEFINE_LOGGER()                                                            \
    logging::log_configurator logging::configurator;                                 \
    logging::singleton<logging::logger_interface, logging::detail::logger> logging::_logger( \
      &logging::logger_interface::ref, &logging::logger_interface::deref,          \
      &logging::logger_interface::ref_counter,                                     \
      (logging::logger_interface*)logging::detail::shared_obj::try_find_shared_object(0), \
      false);                                                                      \
  DEFINE_LOG_UNHANDLED_EXCEPTIONS_MEMBERS
#else  // LOG_SHARED
#  define DEFINE_LOGGER()                                                              \
    logging::log_configurator logging::configurator;                                 \
    logging::singleton<logging::logger_interface, logging::detail::logger> logging::_logger; \
    DEFINE_LOG_UNHANDLED_EXCEPTIONS_MEMBERS
#endif  // LOG_SHARED


//////////// Multithreading defines
#if LOG_MULTITHREADED

#ifdef LOG_PLATFORM_WINDOWS
#define LOG_MT_EVENT_TYPE HANDLE
#define LOG_MT_THREAD_HANDLE_TYPE   HANDLE

#define LOG_MT_MUTEX CRITICAL_SECTION
#define LOG_MT_MUTEX_INIT(x, y) InitializeCriticalSection(x)
#define LOG_MT_MUTEX_LOCK(x) EnterCriticalSection(x)
#define LOG_MT_MUTEX_UNLOCK(x) LeaveCriticalSection(x)
#define LOG_MT_MUTEX_DESTROY(x) DeleteCriticalSection(x)
#define LOG_MT_THREAD_EXIT(x) ExitThread(x)
#else  // LOG_PLATFORM_WINDOWS

#define LOG_MT_EVENT_TYPE pthread_cond_t
#define LOG_MT_THREAD_HANDLE_TYPE   pthread_t

#define LOG_MT_MUTEX pthread_mutex_t
#define LOG_MT_MUTEX_INIT pthread_mutex_init
#define LOG_MT_MUTEX_LOCK pthread_mutex_lock
#define LOG_MT_MUTEX_UNLOCK pthread_mutex_unlock
#define LOG_MT_MUTEX_DESTROY pthread_mutex_destroy
#define LOG_MT_THREAD_EXIT pthread_exit

#endif  // LOG_PLATFORM_WINDOWS

#endif  // LOG_MULTITHREADED


#if LOG_USE_OWN_VSNPRINTF
#include "logger_xprintf.h"
#endif //LOG_USE_OWN_VSNPRINTF


#include "logger_strutils.h"

#if LOG_INI_CONFIGURATION
#include "logger_ini_parser.h"
#endif  // LOG_INI_CONFIGURATION

#include "logger_cfgfn.h"

#include "logger_interfaces.h"

#include "logger_utils.h"

#if LOG_USE_MODULEDEFINITION
#include "logger_moddef.h"
#endif  // LOG_USE_MODULEDEFINITION


#if LOG_CONFIGURE_FROM_REGISTRY && defined(LOG_PLATFORM_WINDOWS)
#include "logger_win_registry.h"

#endif  //(LOG_CONFIGURE_FROM_REGISTRY || LOG_USE_SYSTEMINFO) && defined(LOG_PLATFORM_WINDOWS)


#if LOG_AUTO_DEBUGGING

#include "logger_runtime_debugging.h"
#endif  // LOG_AUTO_DEBUGGING

#if LOG_USE_SYSTEMINFO
#include "logger_sysinfo.h"
#endif  // LOG_USE_SYSTEMINFO



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






////////////////////    Helpers     ////////////////////

/// Singleton template implementation
template <typename _TIf, typename _TImpl = _TIf>
class singleton {
 public:
  singleton(void (_TIf::*ref_fn)() = NULL, void (_TIf::*deref_fn)() = NULL,
            int (_TIf::*ref_cnt_fn)() = NULL, _TIf* ptr = NULL, bool need_delete = false)
      : ptr_(NULL),
        need_delete_(false),
        ref_fn_(ref_fn),
        deref_fn_(deref_fn),
        ref_cnt_fn_(ref_cnt_fn) {
    if (ptr) reset(ptr, need_delete);
  }

  ~singleton() { release(); }

  __inline _TIf* operator->() { return get(); }

  _TIf* get() {
    if (!ptr_) {
      ptr_ = new _TImpl();
      need_delete_ = true;

      if (ref_fn_) (ptr_->*ref_fn_)();
    }

    return ptr_;
  }

  void reset(_TIf* ptr, bool need_delete = true) {
    release();

    need_delete_ = need_delete;
    ptr_ = ptr;

    if (ref_fn_) (ptr_->*ref_fn_)();
  }

  void release() {
    if (deref_fn_ && ptr_) (ptr_->*deref_fn_)();

    if (ref_cnt_fn_ && ptr_) {
      if ((ptr_->*ref_cnt_fn_)()) ptr_ = NULL;
    }

    if (ptr_ && need_delete_) delete ptr_;
    ptr_ = NULL;
    need_delete_ = false;
  }

 private:
  _TIf* ptr_;
  bool need_delete_;

  void (_TIf::*ref_fn_)();
  void (_TIf::*deref_fn_)();
  int (_TIf::*ref_cnt_fn_)();
};

namespace detail {
  class logger;
}//namespace detail

extern singleton<logger_interface, detail::logger> _logger;



namespace detail {




//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
#if LOG_UNHANDLED_EXCEPTIONS
static void init_unhandled_exceptions_handler();
#endif  // LOG_UNHANDLED_EXCEPTIONS

//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////

////////////////////////////////////////////

static const char* default_hdr_format =
    "[$(V)] $(dd).$(MM).$(yyyy) $(hh):$(mm):$(ss).$(ttt) [$(PID):$(TID)] "
    "[$(module)!$(function)]";

}//namespace detail

 //LogSysInfo
 //Verbose
 //HeaderFormat

 //LogPath
 //LogFileName
 //ScrollFileCount
 //ScrollFileSize
 //ScrollFileEveryRun
 //RegistryConfigPath







class log_configurator {
 public:
  log_configurator()
      : log_file_name_(detail::utils::get_process_file_name() + ".log"),
        log_path_(detail::utils::get_process_file_path()),
        hdr_format_(detail::default_hdr_format),
        need_sys_info_(true),
        verb_level_(logger_verbose_optimal),
        scroll_file_size_(10485760),
        scroll_file_count_(15),
        scroll_file_every_run_(false)
#if LOG_CONFIGURE_FROM_REGISTRY
        ,
        reg_config_path_("")
#endif  // LOG_CONFIGURE_FROM_REGISTRY

#if LOG_INI_CONFIGURATION
//        ,
//        ini_file_find_paths_(process_config_macro(LOG_DEFAULT_INI_PATHS))
#endif  // LOG_INI_CONFIGURATION
  {
#ifdef LOG_PLATFORM_ANDROID
    log_path_ = LOG_ANDROID_DEFAULT_LOG_PATH;
#endif

#if LOG_UNHANDLED_EXCEPTIONS
    detail::init_unhandled_exceptions_handler();
#endif  // LOG_UNHANDLED_EXCEPTIONS
  }

/*  const std::string& get_hdr_format() const { return hdr_format_; }
  void set_hdr_format(const std::string& headerFormat) {
    hdr_format_ = process_config_macro(headerFormat);
  };*/

#if LOG_USE_SYSTEMINFO
//  void set_need_sys_info(bool need_sys_info) { need_sys_info_ = need_sys_info; }
//  bool get_need_sys_info() const { return need_sys_info_; };
#endif  // LOG_USE_SYSTEMINFO

/*  void set_log_file_name(std::string file_name) {
    log_file_name_ = process_config_macro(file_name);
    cached_log_file_path_.clear();
  }
  std::string get_log_file_name() const { return log_file_name_; }
  */
  void set_verbose_level(int verb_level) { verb_level_ = verb_level; }
//  int get_verbose_level() const { return verb_level_; }

/*  void set_log_path(std::string logPath) {
    log_path_ = process_config_macro(logPath);
    cached_log_file_path_.clear();
  }
  std::string get_log_path() const { return log_path_; }

  void set_log_scroll_file_size(size_t scroll_file_size) {
    scroll_file_size_ = scroll_file_size;
  }
  size_t get_log_scroll_file_size() const { return scroll_file_size_; }

  void set_log_scroll_file_count(size_t scroll_file_count) {
    scroll_file_count_ = scroll_file_count;
  }
  size_t get_log_scroll_file_count() const { return scroll_file_count_; }

  void set_log_scroll_file_every_run(bool force_scroll) {
    scroll_file_every_run_ = force_scroll;
  }
  bool get_log_scroll_file_every_run() const { return scroll_file_every_run_; }

  const std::string& get_full_log_file_path() {
    if (!cached_log_file_path_.size())
      cached_log_file_path_ = get_log_path() + detail::utils::get_path_separator() + get_log_file_name();

    return cached_log_file_path_;
  }
  */
#if LOG_CONFIGURE_FROM_REGISTRY
  void set_reg_config_path(std::string registryConfigurationPath) {
    reg_config_path_ = process_config_macro(registryConfigurationPath);
  }
  std::string get_reg_config_path() const { return reg_config_path_; }
#endif  // LOG_CONFIGURE_FROM_REGISTRY

#if LOG_INI_CONFIGURATION
//  void set_ini_file_find_paths(std::string ini_file_find_paths) {
//    ini_file_find_paths_ = process_config_macro(ini_file_find_paths);
//  }
//  std::string get_ini_file_find_paths() const { return ini_file_find_paths_; }
#endif  // LOG_INI_CONFIGURATION

 private:

  /**
   * \brief    Process configuration macroses
   * \param    str     configuration string with macroses
   * \return   String with replaced macroses
   */
/*  static std::string process_config_macro(std::string str) {
    using namespace logging::detail;

    if (str::contains(str.c_str(), "$(CURRENTDIR)")) {
#ifdef LOG_PLATFORM_WINDOWS
      char current_dir[MAX_PATH + 1];
      GetCurrentDirectoryA(MAX_PATH, current_dir);
#else  // LOG_PLATFORM_WINDOWS
      char current_dir[512];

#ifdef LOG_HAVE_UNISTD_H
      if (getcwd(current_dir, 512)) current_dir[0] = 0;
#else   // LOG_HAVE_UNISTD_H
      strcpy(current_dir, "./");
#endif  // LOG_HAVE_UNISTD_H

#endif  // LOG_PLATFORM_WINDOWS

      str = str::replace(str, "$(CURRENTDIR)", current_dir);
    }

    if (str::contains(str.c_str(), "$(TEMPPATH)")) {
#ifdef LOG_PLATFORM_WINDOWS
      char temp_path[MAX_PATH + 1];
      GetTempPathA(MAX_PATH, temp_path);
#else   // LOG_PLATFORM_WINDOWS
      const char* temp_path = P_tmpdir;
#endif  // LOG_PLATFORM_WINDOWS

      str = str::replace(str, "$(TEMPPATH)", temp_path);
    }

#ifdef LOG_PLATFORM_WINDOWS
    if (str::contains(str.c_str(), "$(SYSTEMPATH)")) {
      char system_path[MAX_PATH + 1];
      GetSystemDirectoryA(system_path, MAX_PATH);

      str = str::replace(str, "$(SYSTEMPATH)", system_path);
    }

    if (str::contains(str.c_str(), "$(WINDOWSPATH)")) {
      char windows_path[MAX_PATH + 1];
      GetWindowsDirectoryA(windows_path, MAX_PATH);
      str = str::replace(str, "$(WINDOWSPATH)", windows_path);
    }

    if (str::contains(str.c_str(), "$(COMMONAPPDATA)")) {
      char appdata_path[MAX_PATH];
      appdata_path[0] = 0;
      SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, appdata_path);
      str = str::replace(str, "$(COMMONAPPDATA)", appdata_path);
    }

    if (str::contains(str.c_str(), "$(USERAPPDATA)")) {
      char appdata_path[MAX_PATH];
      appdata_path[0] = 0;
      SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appdata_path);
      str = str::replace(str, "$(USERAPPDATA)", appdata_path);
    }

    if (str::contains(str.c_str(), "$(DESKTOPDIR)")) {
      char desktop_path[MAX_PATH];
      desktop_path[0] = 0;
      SHGetFolderPathA(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, desktop_path);
      str = str::replace(str, "$(DESKTOPDIR)", desktop_path);
    }
#endif  // LOG_PLATFORM_WINDOWS

    str = str::replace(str, "$(EXEDIR)", utils::get_process_file_path());
    str = str::replace(str, "$(EXEFILENAME)", utils::get_process_file_name());
    str = str::replace(str, "$(EXEFULLFILENAME)", utils::get_process_full_file_name());

#if LOG_USE_MODULEDEFINITION
    std::string module_path =
        module_definition::module_name_by_addr((void*)&process_config_macro);

    size_t last_delim = module_path.find_last_of("\\/");
    std::string module_dir = module_path.substr(0, last_delim);
    if (last_delim != std::string::npos) last_delim++;

    std::string module_full_file_name = module_path.substr(last_delim, std::string::npos);
    size_t count = module_path.find_last_of(".");
    if (count != std::string::npos) count -= last_delim;

    std::string module_file_name = module_path.substr(last_delim, count);

    str = str::replace(str, "$(MODULEDIR)", module_dir);
    str = str::replace(str, "$(MODULEFULLFILENAME)", module_full_file_name);
    str = str::replace(str, "$(MODULEFILENAME)", module_file_name);
#endif  // LOG_USE_MODULEDEFINITION

    return str;
  }
  */
  std::string log_file_name_;
  std::string log_path_;
  std::string hdr_format_;
  bool need_sys_info_;
  int verb_level_;
  size_t scroll_file_size_;
  size_t scroll_file_count_;
  bool scroll_file_every_run_;

  std::string cached_log_file_path_;

#if LOG_CONFIGURE_FROM_REGISTRY
  std::string reg_config_path_;
#endif  // LOG_CONFIGURE_FROM_REGISTRY

#if LOG_INI_CONFIGURATION
  std::string ini_file_find_paths_;
#endif  // LOG_INI_CONFIGURATION
};

#if LOG_TEST_DO_NOT_WRITE_FILE
#include <sstream>
#undef LOG_FLUSH_FILE_EVERY_WRITE
#define LOG_FLUSH_FILE_EVERY_WRITE 1l
#endif  // LOG_TEST_DO_NOT_WRITE_FILE

extern log_configurator configurator;

//////////////////////////////////////////////////////////////


#if LOG_CONFIGURE_FROM_REGISTRY

class log_registry_configurator {
 public:
  static bool configure(std::string key_path) {
    if (key_path == "") key_path = LOG_REGISTRY_DEFAULT_KEY;

    size_t storage_delim = key_path.find_first_of("\\/");
    if (storage_delim == key_path.npos) return false;

    std::string storage = key_path.substr(0, storage_delim);
    HKEY base_key = log_registry_helper::reg_base_key_by_string(storage);

    std::string path = key_path.substr(storage_delim + 1);

    std::string log_path;
    if (log_registry_helper::get_reg_sz_value(base_key, path, "LogPath", log_path))
      configurator.set_log_path(log_path);

    std::string hdr_format;
    if (log_registry_helper::get_reg_sz_value(base_key, path, "HeaderFormat", hdr_format))
      configurator.set_hdr_format(hdr_format);

    unsigned long verbose_level;
    if (log_registry_helper::get_reg_dword_value(base_key, path, "Verbose",
                                                 verbose_level))
      configurator.set_verbose_level(verbose_level);

#if LOG_USE_SYSTEMINFO
    unsigned long log_sys_info;
    if (log_registry_helper::get_reg_dword_value(base_key, path, "LogSystemInfo",
                                                 log_sys_info))
      configurator.set_need_sys_info(log_sys_info ? true : false);
#endif  // LOG_USE_SYSTEMINFO

    unsigned long log_scroll_file_size;
    if (log_registry_helper::get_reg_dword_value(base_key, path, "ScrollFileSize",
                                                 log_scroll_file_size))
      configurator.set_log_scroll_file_size(log_scroll_file_size);

    unsigned long log_scroll_file_count;
    if (log_registry_helper::get_reg_dword_value(base_key, path, "ScrollFileCount",
                                                 log_scroll_file_count))
      configurator.set_log_scroll_file_count(log_scroll_file_count);

    unsigned long log_scroll_file_every_run;
    if (log_registry_helper::get_reg_dword_value(base_key, path, "ScrollFileEveryRun",
                                                 log_scroll_file_every_run))
      configurator.set_log_scroll_file_every_run(log_scroll_file_every_run ? true
                                                                           : false);

    unsigned long log_enabled;
    if (log_registry_helper::get_reg_dword_value(base_key, path, "LogEnabled",
                                                 log_enabled)) {
      if (!log_enabled) configurator.set_verbose_level(logger_verbose_mute);
    }

    return true;
  }
};

#endif  // LOG_CONFIGURE_FROM_REGISTRY

//////////////////////////////////////////////////////////////



namespace detail {

/**
 * \class    log_stream helper. Implements logger stream interface. User must not use it directly
 */
class log_stream {
public:
  log_stream(logger_interface* logger_obj, int verb_level, void* addr,
    const char* function_name, const char* source_file, int line_number)
    : logger_obj_(logger_obj)
    , verb_level_(verb_level)
    , addr_(addr)
    , function_name_(function_name)
    , source_file_(source_file)
    , line_number_(line_number) {
    logger_obj_->ref();
  }

  log_stream(const log_stream& stream)
    : logger_obj_(stream.logger_obj_)
    , verb_level_(stream.verb_level_)
    , addr_(stream.addr_)
    , function_name_(stream.function_name_)
    , source_file_(stream.source_file_), line_number_(stream.line_number_) {
    ss_.str(stream.ss_.str());
    logger_obj_->ref();
  }

  virtual ~log_stream() {
    logger_obj_->log(verb_level_, addr_, function_name_, source_file_, line_number_, "%s", ss_.str().c_str());
    logger_obj_->deref();
  }

  template<typename T>
  log_stream& operator <<(const T& t) {
    ss_ << t;
    return *this;
  }

private:
  logger_interface* logger_obj_;
  std::stringstream ss_;
  int verb_level_;
  void* addr_;
  const char* function_name_;
  const char* source_file_;
  int line_number_;
};


//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////

////////////////////  Logger implementation  //////////////////////////



/**
 * \class   Logger main class
 */
class logger : public logger_interface {
 private:
  struct plugin_data {
    logger_plugin_interface* plugin_;
    logger_plugin_factory_interface* factory_;
    void(*plugin_delete_fn_)(logger_interface*,logger_plugin_interface*);
    plugin_data() : plugin_(NULL), factory_(NULL), plugin_delete_fn_(NULL) {}
  };

  struct log_config {
    bool process_macro_in_log_text_;
    std::string header_format_;
    bool log_sys_info_;
    int filter_verbose_level_;
    bool load_plugins_from_config_;
    std::string load_plugins_;

    log_config() : process_macro_in_log_text_(false), log_sys_info_(true), filter_verbose_level_(logger_verbose_all)
      ,load_plugins_from_config_(true)
    {}
  };

  log_config log_config_;
  std::map<int, std::list<plugin_data> > plugins_;
  std::list<logger_plugin_factory_interface*> plugins_factories_;

  std::map<int, logger_plugin_interface*> cmd_refs_;

  int ref_counter_;
  bool first_write_;
  cfg::KeyValueTypeList config_;

  cfg::KeyValueTypeList user_code_config_;

#if LOG_SHARED
  bool shared_master_;
  void* shared_obj_ptr_;
#endif  // LOG_SHARED

#if LOG_USE_OBJMON
  struct obj_info_t {
    void* ptr_;
    const char* type_name_;
  };

  std::map<size_t, std::list<obj_info_t> > objmon_objs_;

#if LOG_MULTITHREADED
  LOG_MT_MUTEX mutex_objmon_;
#endif  // LOG_MULTITHREADED

  __inline void lock_objmon() {
#if LOG_MULTITHREADED
    LOG_MT_MUTEX_LOCK(&mutex_objmon_);
#endif  // LOG_MULTITHREADED
  }

  __inline void unlock_objmon() {
#if LOG_MULTITHREADED
    LOG_MT_MUTEX_UNLOCK(&mutex_objmon_);
#endif  // LOG_MULTITHREADED
  }
#endif /*LOG_USE_OBJMON*/


#if LOG_MULTITHREADED
  /** Log record message structure */
  struct log_record {
    int verb_level_;
    std::string hdr_;
    std::string text_;
    log_record() : verb_level_(logger_verbose_mute) {}
  };

  /** buffer lock object */
  LOG_MT_MUTEX mt_buffer_lock_;

  /** Multithreaded buffer */
  std::list<log_record> mt_buffer_;

  LOG_MT_EVENT_TYPE write_event_;
  LOG_MT_EVENT_TYPE buffer_devastated_;
  LOG_MT_THREAD_HANDLE_TYPE  log_thread_handle_;

  /** Logger terminating flag */
  bool mt_terminating_;


private:

  /** 
   * \brief    Logger thread function
   * \param    data    Pointer to logger* object
   * \return   thread exit code
   */
  static unsigned long
#ifdef LOG_PLATFORM_WINDOWS
      __stdcall
#endif  // LOG_PLATFORM_WINDOWS
      log_thread_fn(void* data) {
    utils::set_current_thread_name("logger_thread");
    logger* log = reinterpret_cast<logger*>(data);
    
    while (true) {
      std::list<log_record> log_records;
      std::list<plugin_data> outputs;
      bool mt_terminating;

      // mutex locked section
      {
        LOG_MT_MUTEX_LOCK(&log->mt_buffer_lock_);

        if (log->mt_buffer_.size() == 0) {
          log->wait_event(&log->write_event_, &log->mt_buffer_lock_, true, 250);
        }

        // copy all records from log to local and clear log buffer
        std::copy(log->mt_buffer_.begin(), log->mt_buffer_.end(), std::back_inserter(log_records));
        log->mt_buffer_.clear();
        outputs = log->plugins_[kLogPluginTypeOutput];

        mt_terminating = log->mt_terminating_;
        LOG_MT_MUTEX_UNLOCK(&log->mt_buffer_lock_);
      }

      // notify that log buffer has been devastated
      log->fire_event(&log->buffer_devastated_);

      while (log_records.size()) {
        const log_record& record = *log_records.begin();

        // call registered outputs
        for (std::list<plugin_data>::const_iterator it = outputs.cbegin();
          it != outputs.cend(); it++) {
          logger_output_plugin_interface* output_plugin = dynamic_cast<logger_output_plugin_interface*>(it->plugin_);
          if (output_plugin)
            output_plugin->write(record.verb_level_, record.hdr_, record.text_);
        }
        log_records.pop_front();
      }

      if (mt_terminating) {
        for (std::list<plugin_data>::const_iterator it = outputs.cbegin();
          it != outputs.cend(); it++) {
          logger_output_plugin_interface* output_plugin = dynamic_cast<logger_output_plugin_interface*>(it->plugin_);
          if (output_plugin) {
            output_plugin->flush();
            output_plugin->close();
          }
        }

        break;
      }
    }

    LOG_MT_THREAD_EXIT(0);
    return 0;
  }

  /**
   * \brief    Handle situation when multithreaded buffer has overflow. mt_buffer_lock_ mutex must be locked before call
   */
  void handle_buffer_overflow(bool stop_caller) {
    if (stop_caller) {
      // Wait until log process messages
      while (mt_buffer_.size() > LOG_MULTITHREADED_MAX_BUFFERED_MESSAGES) {
        wait_event(&buffer_devastated_, &mt_buffer_lock_, true, 30);
      }
    } else {  
      // Remove messages from buffer
      size_t remove_msgs = (mt_buffer_.size() / 10) + 1;
      if (remove_msgs > mt_buffer_.size())
        remove_msgs = mt_buffer_.size();

      for (size_t i = 0; i < remove_msgs; i++)
        mt_buffer_.pop_front();

      // notify in log about overflow
      log_record overflow_msg;
      overflow_msg.verb_level_ = logger_verbose_fatal;
      overflow_msg.text_ = str::stringformat("*** Log overflow. Removed %d log messages", static_cast<int>(remove_msgs));
      mt_buffer_.push_back(overflow_msg);
    }
  }



  /** Multithreaded put_to_stream */
  void put_to_stream(int verb_level, const std::string& hdr, const std::string& what) {
    handle_first_write();

    LOG_MT_MUTEX_LOCK(&mt_buffer_lock_);

    if (LOG_MULTITHREADED_MAX_BUFFERED_MESSAGES &&
        mt_buffer_.size() > LOG_MULTITHREADED_MAX_BUFFERED_MESSAGES) {
      handle_buffer_overflow(LOG_MULTITHREADED_STOP_CALLER_ON_OVERFLOW);
    }

    log_record record;
    record.hdr_ = hdr;
    record.text_ = what;
    record.verb_level_ = verb_level;

    mt_buffer_.push_back(record);
    fire_event(&write_event_);
	  LOG_MT_MUTEX_UNLOCK(&mt_buffer_lock_);
  }

  static void fire_event(LOG_MT_EVENT_TYPE* evt) {
#ifdef LOG_PLATFORM_WINDOWS
    SetEvent(*evt);
#else   // LOG_PLATFORM_WINDOWS
    pthread_cond_signal(evt);
#endif  // LOG_PLATFORM_WINDOWS
  }

  static bool wait_event(LOG_MT_EVENT_TYPE* evt, LOG_MT_MUTEX* mutex, bool is_mutex_locked, int wait_ms) {
#ifdef LOG_PLATFORM_WINDOWS
    if (is_mutex_locked) {
      LOG_MT_MUTEX_UNLOCK(mutex);
    }

    bool result = WaitForSingleObject(*evt, wait_ms) == WAIT_OBJECT_0;

    if (is_mutex_locked) {
      LOG_MT_MUTEX_LOCK(mutex);
    }

    return result;
#endif  // LOG_PLATFORM_WINDOWS

#ifndef LOG_PLATFORM_WINDOWS
    struct timeval tv;
    struct timespec ts;

    gettimeofday(&tv, NULL);
    ts.tv_sec = time(NULL) + wait_ms / 1000;
    ts.tv_nsec = tv.tv_usec * 1000 + 1000 * 1000 * (wait_ms % 1000);
    ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
    ts.tv_nsec %= (1000 * 1000 * 1000);

    if (!is_mutex_locked) {
      LOG_MT_MUTEX_LOCK(mutex);
    }

    int ret = pthread_cond_timedwait(evt, &mt_buffer_lock_, &ts);
    if (!is_mutex_locked) {
      LOG_MT_MUTEX_UNLOCK(mutex);
    }

    return ret == 0;
#endif  // LOG_PLATFORM_WINDOWS
  }


#else  // LOG_MULTITHREADED
  /** Single threaded put_to_stream */
  __inline void put_to_stream(int verb_level, const std::string& hdr, const std::string& what) {
    handle_first_write();
    std::string text = hdr + std::string(" ") + what + std::string("\n");

    // call registered outputs
    for (std::list<plugin_data>::const_iterator it = plugins_.at(kLogPluginTypeOutput).cbegin();
      it != plugins_.at(kLogPluginTypeOutput).cend(); it++) {
      logger_output_plugin_interface* output_plugin = dynamic_cast<logger_output_plugin_interface*>(it->plugin_);
      if (output_plugin)
        output_plugin->write(verb_level, hdr, what);
    }
    /*
#if LOG_ANDROID_SYSLOG
    __android_log_write(ANDROID_LOG_INFO, "LOGGER", text.c_str());
#else  // LOG_ANDROID_SYSLOG

#if LOG_FLUSH_FILE_EVERY_WRITE
#if !LOG_TEST_DO_NOT_WRITE_FILE
    std::ofstream stream(configurator.get_full_log_file_path().c_str(), std::ios::app);
    stream << what;
#endif  // LOG_TEST_DO_NOT_WRITE_FILE
#else   // LOG_FLUSH_FILE_EVERY_WRITE
    if (!stream_.is_open())
      stream_.open(configurator.get_full_log_file_path().c_str(), std::ios::app);

    stream_ << text;
#endif  // LOG_FLUSH_FILE_EVERY_WRITE
#endif  // LOG_ANDROID_SYSLOG

    cur_file_size_ += static_cast<int>(what.size());
    */
  }
#endif  // LOG_MULTITHREADED

  /** Process first write in session */
  void handle_first_write() {
    if (first_write_) {
      first_write_ = false;
      put_to_stream(logger_verbose_all, "", "\n");
#if LOG_USE_SYSTEMINFO
      if (log_config_.log_sys_info_) 
        put_to_stream(logger_verbose_all, std::string(), query_system_info());
#endif  // LOG_USE_SYSTEMINFO
    }
  }

  std::string process_config_macros_value(std::string val) const {
    while (true) {
      bool replaced_some = false;
      for (std::list<plugin_data>::const_iterator it = plugins_.at(kLogPluginTypeConfigMacro).cbegin();
        it != plugins_.at(kLogPluginTypeConfigMacro).cend(); it++) {
        logger_config_macro_plugin_interface* output_plugin = dynamic_cast<logger_config_macro_plugin_interface*>(it->plugin_);
        if (output_plugin)
          replaced_some |= output_plugin->process(val);
      }

      if (!replaced_some)
        break;
    }

    return val;
  }

  template<typename TPlug>
  struct find_plugin_type : std::unary_function<TPlug*, bool> {
    std::string type_;
    find_plugin_type(const std::string& type) :type_(type) { }
    bool operator()(TPlug* const& obj) const {
      return str::compare(type_.c_str(), obj->type());
    }
  };

  template<typename TCmdPlugin>
  void register_command_plugin(TCmdPlugin* cmd_plugin) {
    const int kMaxCmdsPerPlugin = 32;

    int cmd_ids[kMaxCmdsPerPlugin];
    memset(cmd_ids, 0, sizeof(cmd_ids));
    cmd_plugin->get_cmd_ids(cmd_ids, kMaxCmdsPerPlugin);

    for (int i = 0; i < kMaxCmdsPerPlugin; i++) {
      if (!cmd_ids[i])
        continue;

      if (cmd_refs_.find(cmd_ids[i]) == cmd_refs_.end()) {
        cmd_refs_.insert(std::make_pair(cmd_ids[i], cmd_plugin));
      }
    }
  }

  bool attach_plugin_internal(logger_plugin_interface* plugin_interface, logger_plugin_factory_interface* plugin_factory,
    void(*plugin_delete_fn)(logger_interface*, logger_plugin_interface*)) {

    plugin_data attached_plugin;
    attached_plugin.plugin_ = plugin_interface;
    attached_plugin.factory_ = plugin_factory;
    attached_plugin.plugin_delete_fn_ = plugin_delete_fn;

    if (plugin_interface->plugin_type() < kLogPluginTypeMin || plugin_interface->plugin_type() > kLogPluginTypeMax)
      return false;

    if (!plugin_interface->attach(this))
      return false;

    plugins_.at(plugin_interface->plugin_type()).push_back(attached_plugin);

    // register plugin as command
    if (plugin_interface->plugin_type() == kLogPluginTypeCommand) {
      logger_command_plugin_interface* cmd_plugin = dynamic_cast<logger_command_plugin_interface*>(plugin_interface);
      if (cmd_plugin) {
        register_command_plugin<logger_command_plugin_interface>(cmd_plugin);
      }
    }

    if (plugin_interface->plugin_type() == kLogPluginTypeArgsCommand) {
      logger_args_command_plugin_interface* cmd_plugin = dynamic_cast<logger_args_command_plugin_interface*>(plugin_interface);
      if (cmd_plugin) {
        register_command_plugin<logger_args_command_plugin_interface>(cmd_plugin);
      }
    }

    return true;
  }


  bool create_and_attach_plugin_from_factory(const std::string& type, const std::string& name) {
    if (is_plugin_attached(type.c_str(), name.c_str()))
      return false;

    std::list<logger_plugin_factory_interface*>::const_iterator it =
      std::find_if(plugins_factories_.begin(), plugins_factories_.end(), find_plugin_type<logger_plugin_factory_interface>(type));

    if (it == plugins_factories_.end())
      return false;

    logger_plugin_interface* plugin_interface = (*it)->create(name.c_str());
    if (!plugin_interface)
      return false;

    bool result = attach_plugin_internal(plugin_interface, *it, NULL);
    if (result)
      plugin_interface->config_updated(config_);
    else {
      (*it)->destroy(plugin_interface);
    }

    return result;
  }

 public:
  void ref() { ref_counter_++; }
  void deref() { ref_counter_--; }
  int ref_counter() { return ref_counter_; }


  void set_config_param(const char* key, const char* value) {
    std::string val(value ? value : "");
    cfg::set_value(user_code_config_, key, process_config_macros_value(val));

    update_config();
  }

  // TODO: maybe need to add remove_config_param_override ?

  const char* get_config_param(const char* key) const {
    return cfg::get_value(config_, key).c_str();
  }


  bool register_plugin_factory(logger_plugin_factory_interface* plugin_factory_interface) {
    if (!plugin_factory_interface)
      return false;

    int plugin_type = plugin_factory_interface->plugin_type();
    if (plugin_type < kLogPluginTypeMin || plugin_type > kLogPluginTypeMax)
      return false;
    
    plugins_factories_.push_back(plugin_factory_interface);

    return true;
  }

  bool unregister_plugin_factory(logger_plugin_factory_interface* plugin_factory_interface) {
    return false;
  }


  void flush() {
#if LOG_MULTITHREADED
    LOG_MT_MUTEX_LOCK(&mt_buffer_lock_);

    // flush multithreaded buffer
    while(mt_buffer_.size()) {
      if (wait_event(&buffer_devastated_, &mt_buffer_lock_, true, 100))
        break;
    }
#endif  // LOG_MULTITHREADED

    // flush registered outputs
    for (std::list<plugin_data>::const_iterator it = plugins_.at(kLogPluginTypeOutput).cbegin();
      it != plugins_.at(kLogPluginTypeOutput).cend(); it++) {
      logger_output_plugin_interface* output_plugin = dynamic_cast<logger_output_plugin_interface*>(it->plugin_);
      if (output_plugin)
        output_plugin->flush();
    }

#if LOG_MULTITHREADED
    LOG_MT_MUTEX_UNLOCK(&mt_buffer_lock_);
#endif  // LOG_MULTITHREADED
  }

  /** Add logger output interface */
  bool attach_plugin(logger_plugin_interface* plugin_interface, void(*plugin_delete_fn)(logger_interface*, logger_plugin_interface*)) {
    if (!plugin_interface)
      return false;

    int plugin_type = plugin_interface->plugin_type();
    if (plugin_type < kLogPluginTypeMin || plugin_type > kLogPluginTypeMax)
      return false;

    // check that plugin was not added already
    for (std::list<plugin_data>::iterator it = plugins_.at(plugin_type).begin(); it != plugins_.at(plugin_type).end(); it++) {
      if (it->plugin_ == plugin_interface)
        return false;
    }

    bool result = attach_plugin_internal(plugin_interface, NULL, plugin_delete_fn);
    if (result) {
      plugin_interface->config_updated(config_);
    }

    return result;
  }

  /** Remove logger output interface */
  bool detach_plugin(logger_plugin_interface* plugin_interface, bool delete_plugin) {
    if (!plugin_interface)
      return false;

    int plugin_type = plugin_interface->plugin_type();
    if (plugin_type < kLogPluginTypeMin || plugin_type > kLogPluginTypeMax)
      return false;
    
    bool result = false;

    for (std::list<plugin_data>::iterator it = plugins_.at(plugin_type).begin(); it != plugins_.at(plugin_type).end(); it++) {
      if (it->plugin_ != plugin_interface)
        continue;

      it->plugin_->detach(this);

      if (delete_plugin) {
        if (it->factory_)
          it->factory_->destroy(it->plugin_);
        else if (it->plugin_delete_fn_)
          it->plugin_delete_fn_(this, it->plugin_);
      }

      plugins_.at(plugin_type).erase(it);
      result = true;
      break;
    }

    return result;
  }

  bool is_plugin_attached(const char* type, const char* name) const {
    for (int i = kLogPluginTypeMin; i <= kLogPluginTypeMax; i++) {
      for (std::list<plugin_data>::const_iterator it = plugins_.at(i).begin(); it != plugins_.at(i).end(); it++) {
        if (type && !str::compare(it->plugin_->type(), type))
          continue;

        if (name && !str::compare(it->plugin_->name(), name))
          continue;

        return true;
      }
    }

    return false;
  }

  log_stream stream(int verb_level, void* addr, const char* function_name, const char* source_file, int line_number) {
    return log_stream(this, verb_level, addr, function_name, source_file, line_number);
  }



#if LOG_USE_OBJMON
  void log_objmon_register(size_t hash_code, const char* type_name, void* ptr) {
    obj_info_t obj_info;
    obj_info.ptr_ = ptr;
    obj_info.type_name_ = type_name;

    lock_objmon();

    if (objmon_objs_.find(hash_code) == objmon_objs_.end()) {
      objmon_objs_.insert(
          std::pair<size_t, std::list<obj_info_t> >(hash_code, std::list<obj_info_t>()));
    }

    objmon_objs_[hash_code].push_back(obj_info);

    unlock_objmon();
    LOG_INFO("[objmon] register instance %p, %s\n", obj_info.ptr_, obj_info.type_name_);
  }

  void log_objmon_unregister(size_t hash_code, void* ptr) {
    lock_objmon();

    if (objmon_objs_.find(hash_code) == objmon_objs_.end()) {
      objmon_objs_.insert(
          std::pair<size_t, std::list<obj_info_t> >(hash_code, std::list<obj_info_t>()));
    }

    std::list<obj_info_t>& obj_ptrs = objmon_objs_[hash_code];

    auto it = std::find_if(obj_ptrs.begin(), obj_ptrs.end(),
                     [ptr](const obj_info_t& obj_info) { return obj_info.ptr_ == ptr; });

    if (it != obj_ptrs.end()) {
      LOG_INFO("[objmon] delete instance %p, %s\n", it->ptr_, it->type_name_);
      obj_ptrs.erase(it);
    } else {
      LOG_WARNING("[objmon] delete request, but object not found. %p\n", ptr);
    }

    unlock_objmon();
  }

  void log_objmon_dump(int verb_level) {
    if (!is_message_enabled(verb_level)) return;

    lock_objmon();

    std::stringstream ss;
    ss << "*** OBJECT LIST ***" << std::endl;
    for (auto it = objmon_objs_.begin(); it != objmon_objs_.end(); it++) {
      if (it->second.size() == 0) continue;

      ss << " ** OBJECT TYPE: " << it->first << ", " << it->second.begin()->type_name_
         << std::endl;

      for (const obj_info_t& obj_info : it->second) {
        ss << "  ptr: " << str::stringformat("%.8X", obj_info.ptr_) << std::endl;
      }
    }

    unlock_objmon();

    put_to_stream(verb_level, std::string(), ss.str());
    std::cout << ss.str();
  }
#endif  /*LOG_USE_OBJMON*/

  unsigned int get_version() const {
    return 0x02010101;
  }

  /**
   * \brief    Logger object constructor
   */
  logger()
      : ref_counter_(0)
      , first_write_(true)
#if LOG_SHARED
      , shared_master_(false)
      , shared_obj_ptr_(NULL)
#endif  // LOG_SHARED

#if LOG_MULTITHREADED
      , mt_terminating_(false)
#endif  // LOG_MULTITHREADED
  {
#if LOG_SHARED
    if (shared_obj::try_find_shared_object(0) == NULL) {
      shared_obj_ptr_ =
          shared_obj::create_shared_object(0, dynamic_cast<logger_interface*>(this));
      shared_master_ = shared_obj_ptr_ ? true : false;
    }
#endif  // LOG_SHARED

    for (int i = kLogPluginTypeMin; i <= kLogPluginTypeMax; i++) {
      plugins_.insert(std::pair<int, std::list<plugin_data> >(i, std::list<plugin_data>()));
    }

#if LOG_MULTITHREADED
    LOG_MT_MUTEX_INIT(&mt_buffer_lock_, NULL);

#if LOG_USE_OBJMON
    LOG_MT_MUTEX_INIT(&mutex_objmon_, NULL);
#endif  // LOG_USE_OBJMON

    reload_config();

#ifdef LOG_PLATFORM_WINDOWS
    write_event_ = CreateEvent(NULL, FALSE, TRUE, NULL);
    buffer_devastated_ = CreateEvent(NULL, FALSE, TRUE, NULL);

    DWORD thread_id;
    log_thread_handle_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&log_thread_fn,
                                     this, 0, &thread_id);    
#else   // LOG_PLATFORM_WINDOWS
    pthread_cond_init(&write_event_, NULL);
    pthread_cond_init(&buffer_devastated_, NULL);
    pthread_create(&log_thread_handle_, NULL, (void* (*)(void*)) & log_thread_fn, this);
#endif  // LOG_PLATFORM_WINDOWS

#endif  // LOG_MULTITHREADED

  }

  /**
   * \brief  Logger object destructor
   */
  virtual ~logger() {
#if LOG_SHARED
    if (shared_master_ && shared_obj::free_shared_object(0)) {
      shared_obj_ptr_ = NULL;
      shared_master_ = false;
#if LOG_CHECKED
      assert(shared_obj::try_find_shared_object(0) == NULL);
#endif  // LOG_CHECKED
    }
#endif  // LOG_SHARED

#if LOG_MULTITHREADED
    // Send termination signal
    LOG_MT_MUTEX_LOCK(&mt_buffer_lock_);
    mt_terminating_ = true;
    LOG_MT_MUTEX_UNLOCK(&mt_buffer_lock_);

    fire_event(&write_event_);

#ifdef LOG_PLATFORM_WINDOWS
    WaitForSingleObject(log_thread_handle_, 10000);
    CloseHandle(write_event_);
    CloseHandle(buffer_devastated_);
#else   // LOG_PLATFORM_WINDOWS
    pthread_join(log_thread_handle_, NULL);
    pthread_cond_destroy(&write_event_);
    pthread_cond_destroy(&buffer_devastated_);
#endif  // LOG_PLATFORM_WINDOWS
#endif /*LOG_MULTITHREADED*/

    // Notify outputs about flush and close
    for (std::list<plugin_data>::const_iterator it = plugins_.at(kLogPluginTypeOutput).cbegin();
      it != plugins_.at(kLogPluginTypeOutput).cend(); it++) {
      logger_output_plugin_interface* output_plugin = dynamic_cast<logger_output_plugin_interface*>(it->plugin_);
      if (output_plugin) {
        output_plugin->flush();
        output_plugin->close();
      }
    }

    // detach all plugins
    for (int i = kLogPluginTypeMin; i <= kLogPluginTypeMax; i++) {
      while (plugins_.at(i).size()) {
        detach_plugin(plugins_.at(i).begin()->plugin_, true);
      }
    }

#if LOG_MULTITHREADED
    LOG_MT_MUTEX_DESTROY(&mt_buffer_lock_);

#  if LOG_USE_OBJMON
    LOG_MT_MUTEX_DESTROY(&mutex_objmon_);
#  endif  // LOG_USE_OBJMON
#endif  // LOG_MULTITHREADED
  }

  /**
   * \brief    Load plugins from load section in specified config list
   * \param    Config values list
   * \return   Count of new modules load
   */
  int load_plugins_from_config(const cfg::KeyValueTypeList& config) {
    int modules_loaded = 0;

    // process [logger] LoadPlugins=plugin1 plugin2:name plugin3  values
    std::string load_plugins = cfg::get_logcfg_string(config, "logger", std::string(), "LoadPlugins");
    std::vector<std::string> plugins_for_load;
    str::split(load_plugins, plugins_for_load);

    for (size_t i = 0; i < plugins_for_load.size(); i++) {
      std::vector<std::string> key_name;

      if (!plugins_for_load[i].size())
        continue;

      str::split(plugins_for_load[i], key_name, ':');
      std::string key = key_name[0];
      std::string name;

      if (key_name.size() > 1)
        name = key_name[1];

      if (is_plugin_attached(key.c_str(), name.c_str()))
        continue; // loaded already

      if (create_and_attach_plugin_from_factory(key, name))
        ++modules_loaded;
    }

    // process [load] section
    for (cfg::KeyValueTypeList::const_iterator it = config.cbegin(); it != config.cend(); it++) {
      const std::string load_prefix = "load:";

      if (!str::starts_with(it->first, load_prefix))
        continue;

      if (atoi(it->second.c_str()) == 0)
        continue; // do not load

      std::string key_name = it->first.substr(load_prefix.size());

      std::string key;
      std::string name;

      std::vector<std::string> key_name_split;
      str::split(key_name, key_name_split, ':');
      if (key_name_split.size() == 2) {
        key = key_name_split[0];
        name = key_name_split[1];
      }
      else if (key_name_split.size() == 1) {
        key = key_name_split[0];
      }
      else continue; // error

      if (is_plugin_attached(key.c_str(), name.c_str()))
        continue; // loaded already

      if (create_and_attach_plugin_from_factory(key, name))
        ++modules_loaded;
    }

    return modules_loaded;
  }

  void decode_logger_config() {
    std::string type_name = "logger";
    log_config_.log_sys_info_ = cfg::get_logcfg_int(config_, type_name, std::string(), "LogSysInfo", 1) ? true : false;
    log_config_.filter_verbose_level_ = cfg::get_logcfg_int(config_, type_name, std::string(), "FilterVerboseLevel", logger_verbose_all);
    log_config_.header_format_ = cfg::get_logcfg_string(config_, type_name, std::string(), "HdrFormat", default_hdr_format);
    log_config_.process_macro_in_log_text_ = cfg::get_logcfg_int(config_, type_name, std::string(), "ProcessMacroInLogText", 0) ? true : false;
    log_config_.load_plugins_from_config_ = cfg::get_logcfg_int(config_, type_name, std::string(), "LoadPluginsFromConfig", 1) ? true : false;
    log_config_.load_plugins_ = cfg::get_logcfg_string(config_, type_name, std::string(), "LoadPlugins", std::string());
  }

  void reload_config_from_plugins() {
    for (std::list<plugin_data>::const_iterator it = plugins_.at(kLogPluginTypeConfig).cbegin();
      it != plugins_.at(kLogPluginTypeConfig).cend(); it++) {
      logger_config_plugin_interface* config_plugin = dynamic_cast<logger_config_plugin_interface*>(it->plugin_);
      if (config_plugin)
        config_plugin->reload(config_);
    }
  }

  void update_config() {
    do {
      // override all config values from user code config because it has highest priority
      for (cfg::KeyValueTypeList::iterator it = user_code_config_.begin(); it != user_code_config_.end(); it++) {
        it->second = process_config_macros_value(it->second);
        cfg::set_value(config_, it->first, it->second);
      }

      // process macroses
      for (cfg::KeyValueTypeList::iterator it = config_.begin(); it != config_.end(); it++) {
        it->second = process_config_macros_value(it->second);
      }

      // update global logger config firstly
      decode_logger_config();

      int modules_loaded;

      // process load section
      if (log_config_.load_plugins_from_config_) { // load plugins from all configs include ini files, registry, etc
        modules_loaded = load_plugins_from_config(config_);
      }
      else { // load plugins only from user-code config
        modules_loaded = load_plugins_from_config(user_code_config_);
      }

      if (!modules_loaded)
        break;
      else {
        reload_config_from_plugins();
      }
    } while (true);

    // Notify all plugins that config has been reloaded
    for (std::map<int, std::list<plugin_data> >::iterator it = plugins_.begin();
      it != plugins_.end(); it++) {
      for (std::list<plugin_data>::iterator plugin_it = it->second.begin(); plugin_it != it->second.end(); plugin_it++) {
        if (plugin_it->plugin_)
          plugin_it->plugin_->config_updated(config_);
      }
    }
  }

  void reload_config() {
#if LOG_INI_CONFIGURATION
//    log_ini_configurator::configure(configurator.get_ini_file_find_paths().c_str());
#endif  // LOG_INI_CONFIGURATION

#if LOG_CONFIGURE_FROM_REGISTRY
    log_registry_configurator::configure(configurator.get_reg_config_path());
#endif  // LOG_CONFIGURE_FROM_REGISTRY

    reload_config_from_plugins();
    update_config();
  }

  void dump_state(int verb_level) {
    put_to_stream(verb_level, std::string(), "*** Logger actual config ***");

    for (cfg::KeyValueTypeList::iterator it = config_.begin(); it != config_.end(); it++) {
      std::string s = it->first + std::string(" = ") + it->second;
      put_to_stream(verb_level, std::string(), s);
    }

    put_to_stream(verb_level, std::string(), "*** Logger loaded plugins ***");

    for (std::map<int, std::list<plugin_data> >::iterator it = plugins_.begin();
      it != plugins_.end(); it++) {
      for (std::list<plugin_data>::iterator plugin_it = it->second.begin(); plugin_it != it->second.end(); plugin_it++) {
        std::string s;
        
        if (plugin_it->plugin_) {
          s = str::stringformat("Plugin Name: %s, Instance name: %s, Plugin Type: %d, Addr: %.8X, Factory: %.8X, Deleter: %.8X",
            plugin_it->plugin_->type(), plugin_it->plugin_->name(),
            plugin_it->plugin_->plugin_type(), plugin_it->plugin_, plugin_it->factory_, plugin_it->plugin_delete_fn_);
        }
        else {
          s = str::stringformat("N/A, Addr: %.8X, Factory: %.8X, Deleter: %.8X", plugin_it->plugin_, plugin_it->factory_, plugin_it->plugin_delete_fn_);
        }

        put_to_stream(verb_level, std::string(), s);
      }
    }

    put_to_stream(verb_level, std::string(), "*** Logger plugins factories ***");

    for (std::list<logger_plugin_factory_interface*>::const_iterator it = plugins_factories_.cbegin(); it != plugins_factories_.cend(); it++) {
      std::string s = str::stringformat("Plugin Name: %s, Plugin Type: %d", (*it)->type(), (*it)->plugin_type());
      put_to_stream(verb_level, std::string(), s);
    }

    put_to_stream(verb_level, std::string(), "*** Logger status ends ***");
  }

/*  void log_binary(int verb_level, void* addr, const char* function_name,
                  const char* source_file, int line_number, const char* data, int len) {
    if (!is_message_enabled(verb_level)) return;

    std::string module_name = try_get_module_name_fast(addr);

    std::stringstream sstream;
    std::string header =
        log_process_macros(log_config_.header_format_, verb_level, 
                           source_file, function_name, line_number, module_name.c_str());
    log_binary(sstream, data, len);
    put_to_stream(verb_level, header, sstream.str());
  }*/


  /**
  * \brief   Log binary information method
  * \param   verb_level     Verbose level
  * \param   addr           Caller address
  * \param   function_name  Caller function name
  * \param   source_file    Caller source file name
  * \param   line_number    Caller source file line number
  * \param   format         Format string
  * \param   ...            Format arguments
  */
  void LOG_CDECL log(int verb_level, void* addr, const char* function_name,
                     const char* source_file, int line_number, const char* format, ...) {
    if (!is_message_enabled(verb_level)) return;

    va_list arguments;
    va_start(arguments, format);
    log_args(verb_level, addr, function_name, source_file, line_number, format, arguments);
    va_end(arguments);
  }

  /**
   * \brief    Log format with va_list arguments
   * \param    verb_level    Verbose level
   * \param    addr           Caller address
   * \param    function_name  Caller function name
   * \param    src_file       Caller source file
   * \param    line_num       Caller source file line number
   * \param    format         Format string
   * \param    arguments      va_list arguments
   */
  void log_args(int verb_level, void* addr, const char* function_name,
                const char* src_file, int line_num, const char* format,
                va_list arguments) {
    if (!is_message_enabled(verb_level)) return;

    std::string module_name = try_get_module_name_fast(addr);
    std::string header =
        log_process_macros(log_config_.header_format_, verb_level, src_file,
                           function_name, line_num, module_name.c_str());
    std::string result;

#if LOG_PROCESS_MACRO_IN_LOG_TEXT
    std::string text = log_process_macroses_nocache(
        format, verb_level, src_file, function_name, line_num, module_name.c_str());
    str::format_arguments_list(result, text.c_str(), arguments);
#else   // LOG_PROCESS_MACRO_IN_LOG_TEXT
    str::format_arguments_list(result, format, arguments);
#endif  // LOG_PROCESS_MACRO_IN_LOG_TEXT

    put_to_stream(verb_level, header, result);
  }

  void log_cmd(int cmd_id, int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number, const void* vparam, int iparam) {

    std::map<int, logger_plugin_interface*>::const_iterator cmd_it = cmd_refs_.find(cmd_id);
    if (cmd_it == cmd_refs_.end())
      return;

    logger_command_plugin_interface* cmd_plugin = dynamic_cast<logger_command_plugin_interface*>(cmd_it->second);
    if (!cmd_plugin)
      return;

    std::string module_name = try_get_module_name_fast(addr);
    std::string header =
      log_process_macros(log_config_.header_format_, verb_level,
        source_file, function_name, line_number, module_name.c_str());
    
    std::string result;
    bool ret = cmd_plugin->cmd(result, cmd_id, verb_level, addr, vparam, iparam);
    if (ret) {
      put_to_stream(verb_level, header, result);
    }
  }

  void log_cmd_args(int cmd_id, int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number, const void* vparam, int iparam, va_list args) {

    std::map<int, logger_plugin_interface*>::const_iterator cmd_it = cmd_refs_.find(cmd_id);
    if (cmd_it == cmd_refs_.end())
      return;

    logger_args_command_plugin_interface* args_cmd_plugin = dynamic_cast<logger_args_command_plugin_interface*>(cmd_it->second);
    if (!args_cmd_plugin)
      return;

    std::string module_name = try_get_module_name_fast(addr);
    std::string header =
      log_process_macros(log_config_.header_format_, verb_level,
        source_file, function_name, line_number, module_name.c_str());

    std::string result;
    bool ret = args_cmd_plugin->cmd_args(result, cmd_id, verb_level, addr, vparam, iparam, args);
    if (ret) {
      put_to_stream(verb_level, header, result);
    }

  }

  void LOG_CDECL log_cmd_vargs(int cmd_id, int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number, const void* vparam, int iparam, ...) {}



#if !LOG_USE_MODULEDEFINITION
  __inline std::string try_get_module_name_fast(void* ptr) {
    (void)ptr;
    return std::string();
  }
#endif  //! LOG_USE_MODULEDEFINITION

#if LOG_USE_MODULEDEFINITION

#if LOG_USE_MODULES_CACHE

  typedef struct _cache_module_t {
    intptr_t base_addr;
    intptr_t end_addr;
    std::string name;
  } cache_module_t;

  std::vector<cache_module_t> cached_modules_;

  bool is_module_found_in_cache(const char* name, intptr_t base_addr, intptr_t end_addr) {
    for (std::vector<cache_module_t>::iterator i = cached_modules_.begin();
         i != cached_modules_.end(); i++) {
      if (i->base_addr == base_addr && i->end_addr == end_addr && i->name == name)
        return true;
    }

    return false;
  }
#endif  // LOG_USE_MODULES_CACHE

  std::string try_get_module_name_fast(void* ptr) {
#if LOG_USE_MODULES_CACHE
    intptr_t addr = (intptr_t)ptr;

    if (!addr) return std::string();

    for (size_t i = 0; i < cached_modules_.size(); i++) {
      if (cached_modules_[i].base_addr <= addr && cached_modules_[i].end_addr >= addr)
        return cached_modules_[i].name;
    }

    std::list<module_entry_t> modules;
    if (module_definition::query_module_list(modules)) {
      for (std::list<module_entry_t>::iterator i = modules.begin(); i != modules.end();
           i++) {
        if (!is_module_found_in_cache(i->module_name.c_str(), i->base_address,
                                      i->base_address + i->size)) {
          cache_module_t mod;

          mod.base_addr = i->base_address;
          mod.end_addr = i->size + i->base_address;
          mod.name = i->module_name;
          cached_modules_.push_back(mod);
        }
      }
    }
#endif  // LOG_USE_MODULES_CACHE

    return module_definition::module_name_by_addr(ptr);
  }
  /*
  void log_modules(int verb_level, void* addr, const char* function_name,
                   const char* source_file, int line_number) {
    if (!is_message_enabled(verb_level)) return;

    std::string module_name = try_get_module_name_fast(addr);

    std::stringstream sstream;

    std::string header =
        log_process_macros(log_config_.header_format_, verb_level, 
                           source_file, function_name, line_number, module_name.c_str());

    std::list<module_entry_t> modules;
    if (!module_definition::query_module_list(modules)) return;

    sstream << "Base\tSize\tModule name\tImage name\tVersion info" << std::endl;
    sstream << "------------------------------------------------" << std::endl;

    for (std::list<module_entry_t>::iterator i = modules.begin(); i != modules.end();
         i++) {
      const module_entry_t& mod_entry = *i;

#ifdef LOG_PLATFORM_64BIT
      sstream << stringformat("0x%.8X%.8X", static_cast<uint32_t>(mod_entry.baseAddress >> 32),
        mod_entry.baseAddress) << "\t";
#endif  // LOG_PLATFORM_64BIT

#ifdef LOG_PLATFORM_32BIT
      sstream << str::stringformat("0x%.8X", mod_entry.base_address) << "\t";
#endif  // LOG_PLATFORM_32BIT

      sstream << mod_entry.size << "\t" << mod_entry.module_name.c_str() << "\t";
      sstream << mod_entry.image_name.c_str() << "\t";

      if (mod_entry.lang_id)
        sstream << "LangId:" << str::stringformat("%.4X", mod_entry.lang_id) << "\t";

      if (mod_entry.file_version.size())
        sstream << "FileVersion:"
                << "\"" << mod_entry.file_version << "\""
                << "\t";

      if (mod_entry.product_version.size())
        sstream << "ProductVersion:"
                << "\"" << mod_entry.product_version << "\""
                << "\t";

      if (mod_entry.company_name.size())
        sstream << "CompanyName:"
                << "\"" << mod_entry.company_name << "\""
                << "\t";

      if (mod_entry.file_description.size())
        sstream << "FileDescription:"
                << "\"" << mod_entry.file_description << "\""
                << "\t";

      sstream << std::endl;
    }

    put_to_stream(verb_level, header, sstream.str());
  }*/
#endif  // LOG_USE_MODULEDEFINITION

#if LOG_AUTO_DEBUGGING

/*
  void log_stack_trace(int verb_level, void* addr, const char* function_name,
                       const char* src_file, int line_number) {
    if (!is_message_enabled(verb_level)) return;

    std::string stack;
    get_current_stack_trace_string(&stack);

    std::string module_name = try_get_module_name_fast(addr);

    std::stringstream sstream;
    std::string header =
        log_process_macros(log_config_.header_format_, verb_level, 
                           src_file, function_name, line_number, module_name.c_str());

    sstream << "Stack trace:" << std::endl << stack << std::endl;
    put_to_stream(verb_level, header, sstream.str());
  }*/
#endif  // LOG_AUTO_DEBUGGING

  void log_exception(int verb_level, void* addr, const char* function_name,
                     const char* src_file, int line_num, const char* user_msg) {
    if (!is_message_enabled(verb_level)) return;

    std::string module_name = try_get_module_name_fast(addr);

    std::stringstream sstream;
    std::string header =
        log_process_macros(log_config_.header_format_, verb_level, src_file,
                           function_name, line_num, module_name.c_str());

    sstream << "*** Exception occured at " << src_file << " (line " << line_num << ")"
            << std::endl;
    if (user_msg)
      sstream << user_msg << std::endl;

    put_to_stream(verb_level, header, sstream.str());
  }

  void log_exception(int verb_level, void* addr, const char* function_name,
                     const char* src_file, int line_num, std::exception* e) {
    if (!is_message_enabled(verb_level)) return;

    std::string message;
    if (e != NULL) {
#if LOG_RTTI_ENABLED
      message += std::string("Exception type: ") + std::string(typeid(*e).name()) + "\n";
#endif  // LOG_RTTI_ENABLED

      if (e->what()) message += std::string(e->what()) + "\n";
    }
    log_exception(verb_level, addr, function_name, src_file, line_num, message.c_str());
  }

 private:

#if LOG_USE_MACRO_HEADER_CACHE
  struct log_macro_cache_t {
    std::string processed_cached_hdr_full;
    std::string processed_cached_hdr_partial;
    struct tm processed_cached_tm;
    std::string processed_cached_src_file;
    std::string processed_cached_function_name;
    std::string processed_cached_module_name;
    std::string processed_cached_hdr_format;
    int processed_cached_global_verb_level;
    int processed_cached_verb_level;
    int processed_cached_line_num;
    int processed_cached_millitm;
    unsigned long processed_cached_pid;
    unsigned long processed_cached_tid;

#if LOG_MULTITHREADED
    LOG_MT_MUTEX mt_cache_lock;
#endif  // LOG_MULTITHREADED

    log_macro_cache_t() {
#if LOG_MULTITHREADED
      LOG_MT_MUTEX_INIT(&mt_cache_lock, NULL);
#endif  // LOG_MULTITHREADED
    }

    virtual ~log_macro_cache_t() {
#if LOG_MULTITHREADED
      LOG_MT_MUTEX_DESTROY(&mt_cache_lock);
#endif  // LOG_MULTITHREADED
    }

    inline void lock() {
#if LOG_MULTITHREADED
      LOG_MT_MUTEX_LOCK(&mt_cache_lock);
#endif  // LOG_MULTITHREADED
    }

    inline void unlock() {
#if LOG_MULTITHREADED
      LOG_MT_MUTEX_UNLOCK(&mt_cache_lock);
#endif  // LOG_MULTITHREADED
    }
  };

  log_macro_cache_t cache;
#endif  // LOG_USE_MACRO_HEADER_CACHE

  std::string log_process_macroses_nocache(std::string format, 
                                           int verbose, 
                                           const char* src_file,
                                           const char* function_name,
                                           int line_num,
                                           const char* module_name) {
#if LOG_USE_MACRO_HEADER_CACHE
    std::string result = log_process_macros_setlen(format, module_name);
    return log_process_macros_often(result, verbose, src_file, function_name, line_num);
#else   // LOG_USE_MACRO_HEADER_CACHE
    return log_process_macros(result, verbose, src_file, function_name, line_num,
                              module_name);
#endif  // LOG_USE_MACRO_HEADER_CACHE
  }

  std::string log_process_macros(std::string format, int verb_level, 
                                 const char* src_file, const char* function_name,
                                 int line_num, const char* module_name) {
    std::string result;

#if LOG_USE_MACRO_HEADER_CACHE
    bool recreate = false;

    cache.lock();

    if (log_config_.header_format_ != cache.processed_cached_hdr_format ||
        log_config_.filter_verbose_level_ != cache.processed_cached_global_verb_level) {
      recreate = true;

      cache.processed_cached_hdr_format = log_config_.header_format_;
      cache.processed_cached_global_verb_level = log_config_.filter_verbose_level_;
    }

    unsigned long pid = utils::get_process_id(), tid = utils::get_thread_id();

    int millisec;
    struct tm newtime = utils::get_time(millisec);

    if (!recreate && cache.processed_cached_hdr_partial.size() &&
        pid == cache.processed_cached_pid &&
        newtime.tm_year == cache.processed_cached_tm.tm_year &&
        newtime.tm_mon == cache.processed_cached_tm.tm_mon &&
        newtime.tm_mday == cache.processed_cached_tm.tm_mday &&
        newtime.tm_hour == cache.processed_cached_tm.tm_hour &&
        newtime.tm_min == cache.processed_cached_tm.tm_min &&
        !strcmp(module_name, cache.processed_cached_module_name.c_str())) {
      if (tid == cache.processed_cached_tid &&
          newtime.tm_sec == cache.processed_cached_tm.tm_sec &&
          millisec == cache.processed_cached_millitm &&
          line_num == cache.processed_cached_line_num &&
          !strcmp(cache.processed_cached_function_name.c_str(), function_name) &&
          verb_level == cache.processed_cached_verb_level) {
        result = cache.processed_cached_hdr_full;
      } else {
        result = log_process_macros_often(cache.processed_cached_hdr_partial, verb_level,
                                          src_file, function_name, line_num);
      }

      cache.processed_cached_millitm = millisec;
      cache.processed_cached_hdr_full = result;
      cache.processed_cached_src_file = src_file;
      cache.processed_cached_function_name = function_name;
      cache.processed_cached_line_num = line_num;
      cache.processed_cached_tm = newtime;
      cache.processed_cached_verb_level = verb_level;
      cache.processed_cached_pid = pid;
      cache.processed_cached_tid = tid;

      cache.unlock();
      return result;
    }
#endif  // LOG_USE_MACRO_HEADER_CACHE

    result = log_process_macros_setlen(format, module_name);

#if LOG_USE_MACRO_HEADER_CACHE
    cache.processed_cached_hdr_partial = result;
#endif  // LOG_USE_MACRO_HEADER_CACHE

    result = log_process_macros_often(result, verb_level, src_file, function_name, line_num);

#if LOG_USE_MACRO_HEADER_CACHE
    cache.processed_cached_hdr_full = result;
    cache.processed_cached_module_name = module_name;
    cache.processed_cached_millitm = millisec;
    cache.processed_cached_hdr_full = result;
    cache.processed_cached_src_file = src_file;
    cache.processed_cached_function_name = function_name;
    cache.processed_cached_line_num = line_num;
    cache.processed_cached_tm = newtime;
    cache.processed_cached_verb_level = verb_level;
    cache.processed_cached_pid = pid;
    cache.processed_cached_tid = tid;

    cache.unlock();
#endif  // LOG_USE_MACRO_HEADER_CACHE

    return result;
  }

  std::string log_process_macros_setlen(std::string format, const char* module_name) {
    const char* format_str = format.c_str();

    bool macro_yyyy = str::contains(format_str, "$(yyyy)");
    bool macro_yy = str::contains(format_str, "$(yy)");
    bool macro_MM = str::contains(format_str, "$(MM)");
    bool macro_M = str::contains(format_str, "$(M)");
    bool macro_dd = str::contains(format_str, "$(dd)");
    bool macro_d = str::contains(format_str, "$(d)");
    bool macro_hh = str::contains(format_str, "$(hh)");
    bool macro_h = str::contains(format_str, "$(h)");
    bool macro_mm = str::contains(format_str, "$(mm)");
    bool macro_m = str::contains(format_str, "$(m)");

    bool macro_module = str::contains(format_str, "$(module)");
    bool macro_MODULE = str::contains(format_str, "$(MODULE)");

    bool macro_PID = str::contains(format_str, "$(PID)");

    if (macro_yyyy || macro_yy || macro_MM || macro_M || macro_dd || macro_d ||
        macro_hh || macro_h || macro_mm || macro_m || macro_PID) {
      int millisec;
      struct tm newtime = utils::get_time(millisec);

      if (macro_yyyy)
        format = str::replace(format, "$(yyyy)", str::stringformat("%.4d", newtime.tm_year + 1900));
      if (macro_yy)
        format = str::replace(format, "$(yy)", str::stringformat("%.2d", newtime.tm_year - 100));
      if (macro_M)
        format = str::replace(format, "$(M)", str::stringformat("%d", newtime.tm_mon + 1));
      if (macro_MM)
        format = str::replace(format, "$(MM)", str::stringformat("%.2d", newtime.tm_mon + 1));
      if (macro_d) format = str::replace(format, "$(d)", str::stringformat("%d", newtime.tm_mday));
      if (macro_dd)
        format = str::replace(format, "$(dd)", str::stringformat("%.2d", newtime.tm_mday));
      if (macro_h) format = str::replace(format, "$(h)", str::stringformat("%d", newtime.tm_hour));
      if (macro_hh)
        format = str::replace(format, "$(hh)", str::stringformat("%.2d", newtime.tm_hour));
      if (macro_m) format = str::replace(format, "$(m)", str::stringformat("%d", newtime.tm_min));
      if (macro_mm)
        format = str::replace(format, "$(mm)", str::stringformat("%.2d", newtime.tm_min));

      if (macro_PID)
        format = str::replace(format, "$(PID)", str::stringformat("%d", utils::get_process_id()));
    }

    if ((macro_module || macro_MODULE) && strlen(module_name)) {
      if (macro_MODULE) format = str::replace(format, "$(MODULE)", module_name);

      if (macro_module) {
        std::string resultName = module_name;
        size_t position = resultName.find_last_of("\\/");
        if (position == resultName.npos)
          position = 0;
        else
          position++;

        format = str::replace(format, "$(module)", resultName.substr(position));
      }
    }
    return format;
  }

  /**
   * \brief    Process log header macroses which used often
   * \param    format         Format string
   * \param    verb_level     Verbose level
   * \param    src_file       Source file name
   * \param    function_name  Function name
   * \param    line_num       Source file line number
   */
  std::string log_process_macros_often(std::string format, int verb_level, 
                                       const char* src_file, const char* function_name, int line_num) {
    const char* format_str = format.c_str();

    bool macro_ss = str::contains(format_str, "$(ss)");
    bool macro_s = str::contains(format_str, "$(s)");
    bool macro_ttt = str::contains(format_str, "$(ttt)");
    bool macro_t = str::contains(format_str, "$(t)");

    bool macro_srcfile = str::contains(format_str, "$(srcfile)");
    bool macro_line = str::contains(format_str, "$(line)");
    bool macro_function = str::contains(format_str, "$(function)");

    bool macro_V = str::contains(format_str, "$(V)");
    bool macro_v = str::contains(format_str, "$(v)");

    bool macro_tid = str::contains(format_str, "$(TID)");

    if (macro_ss || macro_s || macro_ttt || macro_t) {
      int millisec;
      struct tm newtime = utils::get_time(millisec);

      if (macro_s) format = str::replace(format, "$(s)", str::stringformat("%d", newtime.tm_sec));
      if (macro_ss)
        format = str::replace(format, "$(ss)", str::stringformat("%.2d", newtime.tm_sec));
      if (macro_t) format = str::replace(format, "$(t)", str::stringformat("%d", millisec));
      if (macro_ttt) format = str::replace(format, "$(ttt)", str::stringformat("%.3d", millisec));
    }

    if (macro_V) format = str::replace(format, "$(V)", get_verbose_string(verb_level));
    if (macro_v) format = str::replace(format, "$(v)", str::stringformat("%d", verb_level));

    if (macro_srcfile && src_file)
      format = str::replace(format, "$(srcfile)", src_file);

    if (macro_line && line_num >= 0)
      format = str::replace(format, "$(line)", str::stringformat("%d", line_num));

    if (macro_function && function_name)
      format = str::replace(format, "$(function)", function_name);

    if (macro_tid) {
      unsigned long tid = utils::get_thread_id();
      format = str::replace(format, "$(TID)", str::stringformat("%d", tid));
    }

    return format;
  }

  /**
   * \brief    Check is message enabled with specified verbose level
   * \param    verb_level    Verbose level value
   * \return   true - message is enabled for output, false - disabled
   */
  __inline bool is_message_enabled(int verb_level) const {
    return (log_config_.filter_verbose_level_ & verb_level) ? true : false;
  }
  /*
  void log_binary(std::ostream& stream, const char* data, int len) {
    const int output_byte_width = 16;
    int current_element = 0;

    while (current_element < len) {
      int line_start_element = current_element;
      stream << str::stringformat("%.4X: ", line_start_element);

      for (int i = 0; i < output_byte_width; i++) {
        if (current_element < len)
          stream << str::stringformat("%.2X ",
                                 static_cast<unsigned char>(data[current_element++]));
        else
          stream << "   ";
      }

      stream << "|";
      for (int j = line_start_element; j < line_start_element + output_byte_width; j++) {
        if (j >= len) break;
        stream << (static_cast<unsigned char>(data[j]) >= '!' ? data[j] : '.');
      }

      stream << std::endl;
    }
  }
  */
  /**
   * \brief    Get verbose level value string description
   * \param    verb_level      Verbose level value
   * \return   Pointer to constant zero-ended string with description
   */
  static const char* get_verbose_string(int verb_level) {
    switch (verb_level) {
      case logger_verbose_mute:
        return "MUTE";
      case logger_verbose_fatal:
        return "FATAL";
      case logger_verbose_error:
        return "ERROR";
      case logger_verbose_info:
        return "INFO";
      case logger_verbose_warning:
        return "WARNING";
      default:
      case logger_verbose_debug:
        return "DEBUG";
    }
  }
}; // class logger


//////////////////////////////////////////////////////////////
#if LOG_UNHANDLED_EXCEPTIONS

#if defined(LOG_PLATFORM_WINDOWS)
class unhandled_exceptions_processor {
 private:
  /**
   * \brief    Minidump Windows callback implementation
   * \param    param    Unused
   * \param    input    Input request
   * \param    output   Output information
   * \return   TRUE if information is significant, FALSE if not
   */
  static BOOL CALLBACK minidump_callback(PVOID param,
                                         const PMINIDUMP_CALLBACK_INPUT input,
                                         PMINIDUMP_CALLBACK_OUTPUT output) {
    (void)param;

    if (input == 0 || output == 0) return FALSE;

    switch (input->CallbackType) {
      case IncludeModuleCallback:
      case IncludeThreadCallback:
      case ModuleCallback:
      case ThreadCallback:
      case ThreadExCallback:
      case MemoryCallback:
        return TRUE;

      default:
      case CancelCallback:
        return FALSE;
    }
  }

  /**
   * \brief    Create minidump file for Windows
   * \param    ex_ptrs              Information about crash
   * \param    minidump_file_path   Path to minidump file
   */
  static void create_minidump(EXCEPTION_POINTERS* ex_ptrs,
                              const std::string& minidump_file_path) {
    HANDLE file_handle =
        CreateFileA(minidump_file_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file_handle == NULL || file_handle == INVALID_HANDLE_VALUE) {
      LOG_ERROR("Cannot create minidump file: %s, error: %u", minidump_file_path.c_str(),
                GetLastError());
      return;
    }

    MINIDUMP_EXCEPTION_INFORMATION mdei;

    mdei.ThreadId = GetCurrentThreadId();
    mdei.ExceptionPointers = ex_ptrs;
    mdei.ClientPointers = FALSE;

    MINIDUMP_CALLBACK_INFORMATION mci;

    mci.CallbackRoutine =
        (MINIDUMP_CALLBACK_ROUTINE)unhandled_exceptions_processor::minidump_callback;
    mci.CallbackParam = 0;

    MINIDUMP_TYPE mdt =
        (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithHandleData | MiniDumpWithThreadInfo |
                        MiniDumpWithUnloadedModules);

    BOOL rv = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file_handle,
                                mdt, (ex_ptrs != 0) ? &mdei : 0, 0, &mci);

    if (!rv) {
      LOG_ERROR("MiniDumpWriteDump failed. Error: %u", GetLastError());
    } else {
      LOG_INFO("Minidump created: %s", minidump_file_path.c_str());
    }

    CloseHandle(file_handle);
    return;
  }

 public:
  static LPTOP_LEVEL_EXCEPTION_FILTER prev_exception_filter_;

  /**
   * \brief  Crash handler exception filter for Windows
   * \param  ex_ptrs  Exception pointers structure
   * \return EXCEPTION_CONTINUE_SEARCH for continue exception processing or result code
   */
  static long __stdcall crash_handler_exception_filter(EXCEPTION_POINTERS* ex_ptrs) {
    std::string message = str::stringformat(
        "*** Unhandled Exception!\n"
        "   Exception code: 0x%8.8X (%s)\n"
        "   Exception flags: %d\n"
        "   Exception address: 0x%8.8X\n",
        ex_ptrs->ExceptionRecord->ExceptionCode,
        runtime_debugging::get_system_expection_code_text(
            ex_ptrs->ExceptionRecord->ExceptionCode),
        ex_ptrs->ExceptionRecord->ExceptionFlags,
        ex_ptrs->ExceptionRecord->ExceptionAddress);

    LOG_FATAL(message.c_str());

    LOG_MODULES_FATAL();

#if LOG_AUTO_DEBUGGING
    std::string config_sym_path = _logger->get_config_param("runtime_debugging::SymPath");

    LOG_FATAL("Stack trace:\n%s",
              runtime_debugging::get_stack_trace_string(ex_ptrs->ContextRecord, 0, config_sym_path).c_str());
#endif  // LOG_AUTO_DEBUGGING

    int millisec;
    struct tm newtime = utils::get_time(millisec);
    
    std::string path = _logger->get_config_param("crash_dump::DumpPath");
    if (!path.size())
      path = ".";

    std::string file_name = _logger->get_config_param("crash_dump::DumpName");
    if (!file_name.size())
      file_name = "crashdump";

    std::string create_dir = _logger->get_config_param("crash_dump::CreateDumpDirectory");
    if (create_dir.size() && atoi(create_dir.c_str())) {
      utils::create_directory_path(path);
    }

    std::string dump_name = str::stringformat(
        "%s\\%s__%.4d_%.2d_%.2d__%.2d_%.2d_%.2d.dmp", 
        path.c_str(),
        file_name.c_str(), 
        newtime.tm_year + 1900,
        newtime.tm_mon + 1, 
        newtime.tm_mday, 
        newtime.tm_hour, 
        newtime.tm_min,
        newtime.tm_sec);

    create_minidump(ex_ptrs, dump_name);

    if (prev_exception_filter_) return prev_exception_filter_(ex_ptrs);
    
    if (_logger.get())
      _logger->flush();

#if LOG_RELEASE_ON_APP_CRASH
    _logger.release();
#endif  // LOG_RELEASE_ON_APP_CRASH

#if LOG_SHOW_MESSAGE_ON_FATAL_CRASH
    FatalAppExitA(-1, message.c_str());
    return -1;
#else   // LOG_SHOW_MESSAGE_ON_FATAL_CRASH
    return EXCEPTION_CONTINUE_SEARCH;
#endif  // LOG_SHOW_MESSAGE_ON_FATAL_CRASH
  }
};
#else   // defined(LOG_PLATFORM_WINDOWS)

/**
 * \brief  Crash handler for UNIX-based platforms
 * \param  sig     Signal code
 * \param  info    Signal information structure
 * \param  secret  Pointer to context structure
 */
static void crash_handler(int sig, siginfo_t* info, void* secret) {
  void* trace[1024];
  ucontext_t* uc = (ucontext_t*)secret;

#ifdef LOG_PLATFORM_64BIT
  void* pc = (void*)uc->uc_mcontext.gregs[REG_RIP];
#endif  // LOG_PLATFORM_64BIT

#ifdef LOG_PLATFORM_32BIT
  void* pc = (void*)uc->uc_mcontext.gregs[REG_EIP];
#endif  // LOG_PLATFORM_32BIT

  std::string message = str::stringformat("*** Got signal %d, faulty address %p, from %p", sig,
                                     info->si_addr, pc);
  LOG_FATAL(message.c_str());
  LOG_MODULES_FATAL;

  int trace_size = backtrace(trace, 1024);
  trace[1] = pc;

  std::string result =
      "\n" + runtime_debugging::get_stack_trace_string(trace, trace_size, 0);

  LOG_FATAL("*** STACKTRACE ***");
  LOG_FATAL(result.c_str());
  LOG_FATAL("*** END STACKTRACE ***");

  if (_logger.get())
    _logger->flush();

#if LOG_RELEASE_ON_APP_CRASH
  _logger.release();
#endif  // LOG_RELEASE_ON_APP_CRASH

#if LOG_SHOW_MESSAGE_ON_FATAL_CRASH
  printf("%s\n", message.c_str());
#endif  // LOG_SHOW_MESSAGE_ON_FATAL_CRASH

  exit(-1);
}

#endif  // defined(LOG_PLATFORM_WINDOWS)

static void init_unhandled_exceptions_handler() {
#ifdef LOG_PLATFORM_WINDOWS
  unhandled_exceptions_processor::prev_exception_filter_ = SetUnhandledExceptionFilter(
      unhandled_exceptions_processor::crash_handler_exception_filter);
#else   // LOG_PLATFORM_WINDOWS
  struct sigaction sa;

  sa.sa_sigaction = crash_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  sigaction(SIGSEGV, &sa, NULL);
#endif  // LOG_PLATFORM_WINDOWS
}

#endif  // LOG_UNHANDLED_EXCEPTIONS

}//namespace detail


};  // namespace logging

///////////////   C tail  ///////////////////////

#else  // defined(__cplusplus) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))

#if defined(__cplusplus)
extern "C" {
#endif  // defined(__cplusplus)

#define LOG_INFO(...)                                                              \
  __c_logger_log(logger_verbose_info, LOG_GET_CALLER_ADDR, __FUNCTION__, __FILE__, \
                 __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)                                                              \
  __c_logger_log(logger_verbose_debug, LOG_GET_CALLER_ADDR, __FUNCTION__, __FILE__, \
                 __LINE__, __VA_ARGS__)
#define LOG_WARNING(...)                                                              \
  __c_logger_log(logger_verbose_warning, LOG_GET_CALLER_ADDR, __FUNCTION__, __FILE__, \
                 __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)                                                              \
  __c_logger_log(logger_verbose_error, LOG_GET_CALLER_ADDR, __FUNCTION__, __FILE__, \
                 __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)                                                              \
  __c_logger_log(logger_verbose_fatal, LOG_GET_CALLER_ADDR, __FUNCTION__, __FILE__, \
                 __LINE__, __VA_ARGS__)

#define LOG_SET_VERBOSE_LEVEL(l)   \
  __c_logger_set_verbose_level(l)

#define LOG_EXCEPTION_TEXT_INFO(msg)                                                              \
  __c_logger_log(logger_verbose_info, LOG_GET_CALLER_ADDR, __FUNCTION__, __FILE__, \
                 __LINE__, "** Exception: %s", msg)
#define LOG_EXCEPTION_TEXT_DEBUG(msg)                                                              \
  __c_logger_log(logger_verbose_debug, LOG_GET_CALLER_ADDR, __FUNCTION__, __FILE__, \
                 __LINE__, "** Exception: %s", msg)
#define LOG_EXCEPTION_TEXT_WARNING(msg)                                                              \
  __c_logger_log(logger_verbose_warning, LOG_GET_CALLER_ADDR, __FUNCTION__, __FILE__, \
                 __LINE__, "** Exception: %s", msg)
#define LOG_EXCEPTION_TEXT_ERROR(msg)                                                              \
  __c_logger_log(logger_verbose_error, LOG_GET_CALLER_ADDR, __FUNCTION__, __FILE__, \
                 __LINE__, "** Exception: %s", msg)
#define LOG_EXCEPTION_TEXT_FATAL(msg)                                                              \
  __c_logger_log(logger_verbose_fatal, LOG_GET_CALLER_ADDR, __FUNCTION__, __FILE__, \
                 __LINE__, "** Exception: %s", msg)



#define LOG_BINARY_INFO(p, stack_frame)                                         \
  __c_logger_log_binary(logger_verbose_info, LOG_GET_CALLER_ADDR, __FUNCTION__, \
                        __FILE__, __LINE__, p, stack_frame)
#define LOG_BINARY_DEBUG(p, stack_frame)                                         \
  __c_logger_log_binary(logger_verbose_debug, LOG_GET_CALLER_ADDR, __FUNCTION__, \
                        __FILE__, __LINE__, p, stack_frame)
#define LOG_BINARY_WARNING(p, stack_frame)                                         \
  __c_logger_log_binary(logger_verbose_warning, LOG_GET_CALLER_ADDR, __FUNCTION__, \
                        __FILE__, __LINE__, p, stack_frame)
#define LOG_BINARY_ERROR(p, stack_frame)                                         \
  __c_logger_log_binary(logger_verbose_error, LOG_GET_CALLER_ADDR, __FUNCTION__, \
                        __FILE__, __LINE__, p, stack_frame)
#define LOG_BINARY_FATAL(p, stack_frame)                                         \
  __c_logger_log_binary(logger_verbose_fatal, LOG_GET_CALLER_ADDR, __FUNCTION__, \
                        __FILE__, __LINE__, p, stack_frame)

#define LOG_SET_CURRENT_THREAD_NAME(name) __c_logger_set_current_thread_name(name)

#if LOG_USE_DLL
extern void(LOG_CDECL* __c_logger_log_modules)(int verbose_level, void* caller_addr,
                                               const char* function, const char* file,
                                               int line);
#endif  // LOG_USE_DLL

#if LOG_USE_OBJMON
#define LOG_OBJMON_REGISTER_INSTANCE() \
  __c_logger_objmon_register(typeid(*this).hash_code(), typeid(*this).name(), this)
#define LOG_OBJMON_UNREGISTER_INSTANCE() \
  __c_logger_objmon_unregister(typeid(*this).hash_code(), this)
#define LOG_OBJMON_DUMP_INFO() __c_logger_objmon_dump(logger_verbose_info)
#define LOG_OBJMON_DUMP_DEBUG() __c_logger_objmon_dump(logger_verbose_debug)
#define LOG_OBJMON_DUMP_WARNING() __c_logger_objmon_dump(logger_verbose_warning)
#define LOG_OBJMON_DUMP_ERROR() __c_logger_objmon_dump(logger_verbose_error)
#endif  // LOG_USE_OBJMON

#if LOG_USE_MODULEDEFINITION

#define LOG_MODULES_DEBUG()                                                         \
  __c_logger_log_modules(logger_verbose_debug, LOG_GET_CALLER_ADDR, __FUNCTION__, \
                         __FILE__, __LINE__)
#define LOG_MODULES_INFO                                                         \
  __c_logger_log_modules(logger_verbose_info, LOG_GET_CALLER_ADDR, __FUNCTION__, \
                         __FILE__, __LINE__)
#define LOG_MODULES_WARNING                                                         \
  __c_logger_log_modules(logger_verbose_warning, LOG_GET_CALLER_ADDR, __FUNCTION__, \
                         __FILE__, __LINE__)
#define LOG_MODULES_ERROR                                                         \
  __c_logger_log_modules(logger_verbose_error, LOG_GET_CALLER_ADDR, __FUNCTION__, \
                         __FILE__, __LINE__)
#define LOG_MODULES_FATAL                                                         \
  __c_logger_log_modules(logger_verbose_fatal, LOG_GET_CALLER_ADDR, __FUNCTION__, \
                         __FILE__, __LINE__)

#if !LOG_USE_DLL
void __c_logger_log_modules(int verbose_level, void* caller_addr, const char* function,
                            const char* file, int line);
#endif  //! LOG_USE_DLL

#else  // LOG_USE_MODULEDEFINITION

#define LOG_MODULES_DEBUG()
#define LOG_MODULES_INFO()
#define LOG_MODULES_WARNING()
#define LOG_MODULES_ERROR()
#define LOG_MODULES_FATAL()

#endif  // LOG_USE_MODULEDEFINITION

#if LOG_USE_DLL
extern void(LOG_CDECL* __c_logger_log_stack_trace)(int verbose_level, void* caller_addr,
                                                   const char* function, const char* file,
                                                   int line);
#endif  // LOG_USE_DLL

#if LOG_AUTO_DEBUGGING

#define LOG_STACKTRACE_DEBUG()                                                     \
  __c_logger_log_stack_trace(logger_verbose_debug, logging_get_caller_address(), \
                             __FUNCTION__, __FILE__, __LINE__)
#define LOG_STACKTRACE_INFO                                                     \
  __c_logger_log_stack_trace(logger_verbose_info, logging_get_caller_address(), \
                             __FUNCTION__, __FILE__, __LINE__)
#define LOG_STACKTRACE_WARNING                                                     \
  __c_logger_log_stack_trace(logger_verbose_warning, logging_get_caller_address(), \
                             __FUNCTION__, __FILE__, __LINE__)
#define LOG_STACKTRACE_ERROR                                                     \
  __c_logger_log_stack_trace(logger_verbose_error, logging_get_caller_address(), \
                             __FUNCTION__, __FILE__, __LINE__)
#define LOG_STACKTRACE_FATAL                                                     \
  __c_logger_log_stack_trace(logger_verbose_fatal, logging_get_caller_address(), \
                             __FUNCTION__, __FILE__, __LINE__)

#if !LOG_USE_DLL
void __c_logger_log_stack_trace(int verbose_level, void* caller_addr,
                                const char* function, const char* file, int line);
#endif  //! LOG_USE_DLL

#else  // LOG_AUTO_DEBUGGING

#define LOG_STACKTRACE_DEBUG
#define LOG_STACKTRACE_INFO
#define LOG_STACKTRACE_WARNING
#define LOG_STACKTRACE_ERROR
#define LOG_STACKTRACE_FATAL

#endif  // LOG_AUTO_DEBUGGING

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

void __c_logger_objmon_register(size_t hash_code, const char* type_name, void* ptr);
void __c_logger_objmon_unregister(size_t hash_code, void* ptr);
void __c_logger_objmon_dump(int verbose_level);

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
