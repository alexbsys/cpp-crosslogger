
#ifndef LOGGER_USERDEFS_HEADER
#define LOGGER_USERDEFS_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_pdefs.h"
#include "logger_verbose.h"

#if !LOG_ENABLED
#define LOG_INFO(...)
#define LOG_WARNING(...)
#define LOG_DEBUG(...)
#define LOG_ERROR(...)
#define LOG_FATAL(...)

#define LOG_BINARY_INFO(p,l)
#define LOG_BINARY_WARNING(p,l)
#define LOG_BINARY_DEBUG(p,l)
#define LOG_BINARY_ERROR(p,l)
#define LOG_BINARY_FATAL(p,l)

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

#define DEFINE_LOGGER(on_create_fn)

#define LOG_SET_VERBOSE_LEVEL(l)
#define LOG_SET_REG_CONFIG_PATH(p)
#define LOG_SET_INI_CONFIG_PATHS(p)

#define LOGGER_VERBOSE_FATAL 0
#define LOGGER_VERBOSE_ERROR 1
#define LOGGER_VERBOSE_INFO 2
#define LOGGER_VERBOSE_WARNING 3
#define LOGGER_VERBOSE_DEBUG 4
#define LOGGER_VERBOSE_OPTIMAL 5
#define LOGGER_VERBOSE_MUTE 6
#define LOGGER_VERBOSE_FATAL_ERROR 7
#define LOGGER_VERBOSE_ALL 8
#define LOGGER_VERBOSE_NORMAL 9

#define LOG_OBJMON_REGISTER_INSTANCE()
#define LOG_OBJMON_UNREGISTER_INSTANCE()

#define LOG_OBJMON_DUMP_DEBUG()
#define LOG_OBJMON_DUMP_INFO()
#define LOG_OBJMON_DUMP_ERROR()
#define LOG_OBJMON_DUMP_FATAL()

#define LOG_RELOAD_CONFIG()
#define LOG_DUMP_STATE(v)

#define LOG_REGISTER_PLUGIN_FACTORY(plugin_factory)
#define LOG_UNREGISTER_PLUGIN_FACTORY(plugin_factory)

#define LOG_ATTACH_PLUGIN(plugin)
#define LOG_DETACH_PLUGIN(plugin)

#define LOG_GET_VERSION()
#define LOG_SET_CONFIG_PARAM(name,value)
#define LOG_GET_CONFIG_PARAM(name, buf, size)
#define LOG_FLUSH()
#define LOG_SHUTDOWN()

#define LOG_TEXT(v,...)
#define LOG_CMD(...)
#define LOG_GET_LOGGER()
#define LOG_STREAM(v)

#else  /*LOG_ENABLED*/

#if LOG_USE_MODULEDEFINITION
#define LOG_GET_CALLER_ADDR logging_get_caller_address()
#else //LOG_USE_MODULEDEFINITION
#define LOG_GET_CALLER_ADDR 0L
#endif  // LOG_USE_MODULEDEFINITION

#if defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))
// Logger inproc implementation

#define LOGOBJ_CMD(logobj,cmdid,v,p,l) \
  (logobj)->log_cmd((cmdid), (v), LOG_GET_CALLER_ADDR, __FUNCTION__, \
                        __FILE__, __LINE__, (p), (l) )

#define LOGOBJ_TEXT(logobj, v, ...) \
  (logobj)->log((v), LOG_GET_CALLER_ADDR, __FUNCTION__, \
    __FILE__, __LINE__, __VA_ARGS__)

#define LOGOBJ_RELOAD_CONFIG(logobj) \
  (logobj)->reload_config()

#define LOGOBJ_DUMP_STATE(logobj,v) \
  (logobj)->dump_state((v))

#define LOGOBJ_REGISTER_PLUGIN_FACTORY(logobj,plugin_factory) \
  (logobj)->register_plugin_factory((plugin_factory))

#define LOGOBJ_UNREGISTER_PLUGIN_FACTORY(logobj,plugin_factory) \
  (logobj)->unregister_plugin_factory((plugin_factory))

#define LOGOBJ_ATTACH_PLUGIN(logobj,plugin) \
  (logobj)->attach_plugin((plugin))

#define LOGOBJ_DETACH_PLUGIN(logobj,plugin) \
  (logobj)->detach_plugin((plugin))

#define LOGOBJ_GET_VERSION(logobj) \
  (logobj)->get_version()

#define LOGOBJ_SET_CONFIG_PARAM(logobj,name,value) \
  (logobj)->set_config_param((name),(value))

#define LOGOBJ_GET_CONFIG_PARAM(logobj,name,buf,size) \
  (logobj)->get_config_param(name, (buf),(size))

#define LOGOBJ_FLUSH(logobj) \
  (logobj)->flush(1)

#define LOGOBJ_SHUTDOWN() (logging::_logger->release())


#define LOGOBJ_GET_DEFAULT_LOGGER() (logging::_logger->get())

// hi-level calls
#define LOG_STREAM(v) \
  LOGOBJ_GET_DEFAULT_LOGGER()->stream((v), LOG_GET_CALLER_ADDR, \
      __FUNCTION__, __FILE__, __LINE__)

#define LOG_GET_LOGGER() \
  LOGOBJ_GET_DEFAULT_LOGGER()


#else //defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))
// Logger outproc implementation

#define LOGOBJ_CMD(logobj,cmdid,v,p,l) \
  __c_logger_log_cmd((logobj), (cmdid), (v), LOG_GET_CALLER_ADDR, __FUNCTION__, \
                        __FILE__, __LINE__, (p), (l))

#define LOGOBJ_TEXT(logobj, v, ...) \
  __c_logger_log((logobj), (v), LOG_GET_CALLER_ADDR, __FUNCTION__, \
    __FILE__, __LINE__, __VA_ARGS__)

#define LOGOBJ_RELOAD_CONFIG(logobj) \
  __c_logger_reload_config((logobj))

#define LOGOBJ_DUMP_STATE(logobj,v) \
  __c_logger_dump_state((logobj), (v))

#define LOGOBJ_REGISTER_PLUGIN_FACTORY(logobj,plugin_factory) \
  __c_logger_register_plugin_factory((logobj),(plugin_factory))

#define LOGOBJ_UNREGISTER_PLUGIN_FACTORY(logobj,plugin_factory) \
  __c_logger_unregister_plugin_factory((logobj),(plugin_factory))

#define LOGOBJ_ATTACH_PLUGIN(logobj,plugin) \
  __c_logger_attach_plugin((logobj),(plugin))

#define LOGOBJ_DETACH_PLUGIN(logobj,plugin) \
  __c_logger_detach_plugin((logobj),(plugin))

#define LOGOBJ_GET_VERSION(logobj) \
  c_logger_get_version((logobj))

#define LOGOBJ_IS_MASTER(logobj) \
  c_logger_is_master((logobj))

#define LOGOBJ_SET_CONFIG_PARAM(logobj,name,value) \
  __c_logger_set_config_param((logobj),(name),(value))

#define LOGOBJ_GET_CONFIG_PARAM(logobj,name,buf,size) \
  __c_logger_get_config_param((logobj),(name),(buf),(size))

#define LOGOBJ_FLUSH(logobj) \
  __c_logger_flush((logobj))

#define LOGOBJ_SHUTDOWN() \
  __c_logger_shutdown()

#define LOG_GET_LOGGER() \
  __c_logger_get_logger()

#if defined(LOG_CPP)
#define LOGOBJ_STREAM(logobj,v)  reinterpret_cast<logging::logger_interface*>((logobj))->stream((v), LOG_GET_CALLER_ADDR, \
      __FUNCTION__, __FILE__, __LINE__)

#define LOG_STREAM(v) \
  (LOG_GET_LOGGER() ? LOGOBJ_STREAM(LOG_GET_LOGGER(), (v)) : logging::log_stream(NULL,(v)))

#endif /*LOG_CPP*/


#if LOG_CPP_X11
#  define LOGOBJ_GET_DEFAULT_LOGGER() (nullptr)
#else /*LOG_CPP_X11*/
#  define LOGOBJ_GET_DEFAULT_LOGGER() (0L)
#endif /*LOG_CPP_X11*/


#endif


//// both for C and C++
// hi-level calls
#define LOG_CMD(cmdid,v,p,l) \
  LOGOBJ_CMD(LOGOBJ_GET_DEFAULT_LOGGER(), (cmdid), (v), (p), (l))

#define LOG_TEXT(v, ...) \
  LOGOBJ_TEXT(LOGOBJ_GET_DEFAULT_LOGGER(), (v), __VA_ARGS__)


#define LOG_RELOAD_CONFIG() \
  LOGOBJ_RELOAD_CONFIG(LOGOBJ_GET_DEFAULT_LOGGER())

#define LOG_DUMP_STATE(v) \
  LOGOBJ_DUMP_STATE(LOGOBJ_GET_DEFAULT_LOGGER(),(v))

#define LOG_REGISTER_PLUGIN_FACTORY(plugin_factory) \
  LOGOBJ_REGISTER_PLUGIN_FACTORY(LOGOBJ_GET_DEFAULT_LOGGER(),(plugin_factory))

#define LOG_UNREGISTER_PLUGIN_FACTORY(plugin_factory) \
  LOGOBJ_UNREGISTER_PLUGIN_FACTORY(LOGOBJ_GET_DEFAULT_LOGGER(),(plugin_factory))

#define LOG_ATTACH_PLUGIN(plugin) \
  LOGOBJ_ATTACH_PLUGIN(LOGOBJ_GET_DEFAULT_LOGGER(),(plugin))

#define LOG_DETACH_PLUGIN(plugin) \
  LOGOBJ_DETACH_PLUGIN(LOGOBJ_GET_DEFAULT_LOGGER(),(plugin))

#define LOG_GET_VERSION() \
  LOGOBJ_GET_VERSION(LOGOBJ_GET_DEFAULT_LOGGER())

#define LOG_SET_CONFIG_PARAM(name,value) \
  LOGOBJ_SET_CONFIG_PARAM(LOGOBJ_GET_DEFAULT_LOGGER(), (name), (value))

#define LOG_GET_CONFIG_PARAM(name, buf, size) \
  LOGOBJ_GET_CONFIG_PARAM(LOGOBJ_GET_DEFAULT_LOGGER(), (buf), (size))

#define LOG_FLUSH() \
  LOGOBJ_FLUSH(LOGOBJ_GET_DEFAULT_LOGGER())

#define LOG_SHUTDOWN() \
  LOGOBJ_SHUTDOWN()

#define LOG_INFO(...)                                                                    \
  LOG_TEXT(LOGGER_VERBOSE_INFO, __VA_ARGS__)  
#define LOG_DEBUG(...)                                                      \
  LOG_TEXT(LOGGER_VERBOSE_DEBUG, __VA_ARGS__)  
#define LOG_WARNING(...)                                                      \
  LOG_TEXT(LOGGER_VERBOSE_WARNING, __VA_ARGS__)  
#define LOG_ERROR(...)                                                      \
  LOG_TEXT(LOGGER_VERBOSE_ERROR, __VA_ARGS__)  
#define LOG_FATAL(...)                                                      \
  LOG_TEXT(LOGGER_VERBOSE_FATAL, __VA_ARGS__)  


#define LOG_BINARY_INFO(p, l)                                           \
     LOG_CMD(0x1001, LOGGER_VERBOSE_INFO, (p), (l))
#define LOG_BINARY_DEBUG(p, l) \
     LOG_CMD(0x1001, LOGGER_VERBOSE_DEBUG, (p), (l))
#define LOG_BINARY_WARNING(p, l)                                           \
     LOG_CMD(0x1001, LOGGER_VERBOSE_WARNING, (p), (l))
#define LOG_BINARY_ERROR(p, l)                                           \
     LOG_CMD(0x1001, LOGGER_VERBOSE_ERROR, (p), (l))
#define LOG_BINARY_FATAL(p, l)                                           \
     LOG_CMD(0x1001, LOGGER_VERBOSE_FATAL, (p), (l))


#define LOG_EXCEPTION_TEXT(v, msg)  \
     LOG_CMD(0x1009, (v), (msg), 0)
#define LOG_EXCEPTION_TEXT_DEBUG(msg)                                                        \
    LOG_EXCEPTION_TEXT(LOGGER_VERBOSE_DEBUG, (msg))
#define LOG_EXCEPTION_TEXT_WARNING(msg)                                                        \
    LOG_EXCEPTION_TEXT(LOGGER_VERBOSE_WARNING, (msg))
#define LOG_EXCEPTION_TEXT_INFO(msg)                                                        \
    LOG_EXCEPTION_TEXT(LOGGER_VERBOSE_INFO, (msg))
#define LOG_EXCEPTION_TEXT_ERROR(msg)                                                        \
    LOG_EXCEPTION_TEXT(LOGGER_VERBOSE_ERROR, (msg))
#define LOG_EXCEPTION_TEXT_FATAL(msg)                                                        \
    LOG_EXCEPTION_TEXT(LOGGER_VERBOSE_FATAL, (msg))

#if LOG_USE_MODULEDEFINITION
#  define LOG_MODULES_DEBUG()                                                           \
     LOG_CMD(0x1002, LOGGER_VERBOSE_DEBUG, NULL, 0)
#  define LOG_MODULES_INFO()                                                           \
     LOG_CMD(0x1002, LOGGER_VERBOSE_INFO, NULL, 0)
#  define LOG_MODULES_WARNING()                                                           \
     LOG_CMD(0x1002, LOGGER_VERBOSE_WARNING, NULL, 0)
#  define LOG_MODULES_ERROR()                                                           \
     LOG_CMD(0x1002, LOGGER_VERBOSE_ERROR, NULL, 0)
#  define LOG_MODULES_FATAL()                                                           \
     LOG_CMD(0x1002, LOGGER_VERBOSE_FATAL, NULL, 0)

#else  // LOG_USE_MODULEDEFINITION
#  define LOG_MODULES_DEBUG()
#  define LOG_MODULES_INFO()
#  define LOG_MODULES_WARNING()
#  define LOG_MODULES_ERROR()
#  define LOG_MODULES_FATAL()
#endif  // LOG_USE_MODULEDEFINITION


#if LOG_AUTO_DEBUGGING
#  define LOG_STACKTRACE_DEBUG()                                                            \
     LOG_CMD(0x1003, LOGGER_VERBOSE_DEBUG, NULL, 0)
#  define LOG_STACKTRACE_INFO()                                                            \
     LOG_CMD(0x1003, LOGGER_VERBOSE_INFO, NULL, 0)
#  define LOG_STACKTRACE_WARNING()                                                   \
     LOG_CMD(0x1003, LOGGER_VERBOSE_WARNING, NULL, 0)
#  define LOG_STACKTRACE_ERROR()                                                            \
     LOG_CMD(0x1003, LOGGER_VERBOSE_ERROR, NULL, 0)
#  define LOG_STACKTRACE_FATAL()                                                            \
     LOG_CMD(0x1003, LOGGER_VERBOSE_FATAL, NULL, 0)

#else  // LOG_AUTO_DEBUGGING
#  define LOG_STACKTRACE_DEBUG()
#  define LOG_STACKTRACE_INFO()
#  define LOG_STACKTRACE_WARNING()
#  define LOG_STACKTRACE_ERROR()
#  define LOG_STACKTRACE_FATAL()
#endif  // LOG_AUTO_DEBUGGING


#if defined(LOG_CPP)
#define LOGS_DEBUG()    LOG_STREAM(LOGGER_VERBOSE_DEBUG)
#define LOGS_INFO()    LOG_STREAM(LOGGER_VERBOSE_INFO)
#define LOGS_WARNING()    LOG_STREAM(LOGGER_VERBOSE_WARNING)
#define LOGS_ERROR()    LOG_STREAM(LOGGER_VERBOSE_ERROR)
#define LOGS_FATAL()    LOG_STREAM(LOGGER_VERBOSE_FATAL)
#endif /*LOG_CPP*/


////// both for C and C++ ends

#if defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))
/*
#define LOGS_DEBUG() \
  LOG_STREAM(LOGGER_VERBOSE_DEBUG)

#define LOGS_INFO() \
  LOG_STREAM(LOGGER_VERBOSE_INFO)

#define LOGS_ERROR() \
  LOG_STREAM(LOGGER_VERBOSE_ERROR)

#define LOGS_WARNING() \
  LOG_STREAM(LOGGER_VERBOSE_WARNING)

#define LOGS_FATAL() \
  LOG_STREAM(LOGGER_VERBOSE_FATAL)
  */
#define LOG_STD_EXCEPTION(v, e)  \
  LOG_CMD(0x1008, (v), (e), 0)

#define LOG_EXCEPTION_DEBUG(e)                                                        \
  LOG_STD_EXCEPTION(logging::logger_verbose_debug, e)

#define LOG_SET_CURRENT_THREAD_NAME(name) logging::detail::utils::set_current_thread_name(name)

// Logger instance definition macro
#if LOG_SHARED
#  define DEFINE_LOGGER(on_create_fn)                                                 \
    logging::logger_singleton_interface<logging::logger_interface>* logging::_logger( \
    new logging::singleton<logging::logger_interface, logging::detail::logger>( \
      (on_create_fn), \
      &logging::logger_interface::ref, \
      &logging::logger_interface::deref,          \
      &logging::logger_interface::ref_counter,                                     \
      (logging::logger_interface*)logging::detail::shared_obj::try_find_shared_object(0), \
      false))
#else  // LOG_SHARED
#  define DEFINE_LOGGER(on_create_fn)                                                              \
    logging::logger_singleton_interface<logging::logger_interface>* logging::_logger( \
    new logging::singleton<logging::logger_interface, logging::detail::logger>( \
      (on_create_fn), \
      &logging::logger_interface::ref, \
      &logging::logger_interface::deref,          \
      &logging::logger_interface::ref_counter,                                     \
      NULL, \
      false))

#endif  // LOG_SHARED


#define DEFINE_LOGGER_DEFAULT() \
  void __OnLoggerCreatedHandler(logging::logger_interface* log) { \
    if(log) log->register_plugin_factory(new logging::logger_register_builtin_plugin_factory()); \
  } \
  DEFINE_LOGGER(&__OnLoggerCreatedHandler)


#else /*defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))*/

#define DEFINE_LOGGER(...)
#define DEFINE_LOGGER_DEFAULT()

#define LOG_SET_CURRENT_THREAD_NAME(name) __c_logger_set_current_thread_name(LOGOBJ_GET_DEFAULT_LOGGER(), name)

#endif /*defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))*/

#ifdef LOG_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4250)
#endif /*LOG_COMPILER_MSVC*/


#define LOG_OBJMON_REGISTER_INSTANCE() { \
struct { unsigned int hash; const char* type_name; void* ptr; } \
  param = { (unsigned int)typeid(*this).hash_code(), typeid(*this).name(), this }; \
  LOG_CMD(0x1010, LOGGER_VERBOSE_INFO, &param, 0); \
}


#define LOG_OBJMON_UNREGISTER_INSTANCE() { \
struct { unsigned int hash; void* ptr; } param = { (unsigned int)typeid(*this).hash_code(), this }; \
  LOG_CMD(0x1011, LOGGER_VERBOSE_INFO, &param, 0); \
}

#define LOG_OBJMON_SET_USER_OBJ_INFO_CALLBACK(pcallback) { \
int(*pcb)(void* obj_ptr, const char* type_name, const struct tm* created, int created_ms, const char* creation_trace, char* out_ext_message); \
  pcb = pcallback; \
  LOG_CMD(0x1013, LOGGER_VERBOSE_INFO, (const void*)pcb, 0); \
}

#define LOG_OBJMON_GET_REGISTERED_OBJECTS_COUNT(pintvalue) { \
int obj_count = 0; \
  LOG_CMD(0x1014, LOGGER_VERBOSE_INFO, &obj_count, 0); \
  if (pintvalue) *pintvalue = obj_count; \
}

#ifdef LOG_COMPILER_MSVC
#pragma warning(pop)
#endif /*LOG_COMPILER_MSVC*/


#define LOG_OBJMON_DUMP_INFO()   LOG_CMD(0x1012, LOGGER_VERBOSE_INFO, NULL, 0)
#define LOG_OBJMON_DUMP_DEBUG() LOG_CMD(0x1012, LOGGER_VERBOSE_DEBUG, NULL, 0)
#define LOG_OBJMON_DUMP_WARNING() LOG_CMD(0x1012, LOGGER_VERBOSE_WARNING, NULL, 0)
#define LOG_OBJMON_DUMP_ERROR() LOG_CMD(0x1012, LOGGER_VERBOSE_ERROR, NULL, 0)
#define LOG_OBJMON_DUMP_FATAL() LOG_CMD(0x1012, LOGGER_VERBOSE_FATAL, NULL, 0)

#define LOG_OBJMON_DUMP_CONSOLE() LOG_CMD(0x1015, LOGGER_VERBOSE_FATAL, NULL, 0)

#endif /*LOG_ENABLED*/

#endif /*LOGGER_USERDEFS_HEADER*/
