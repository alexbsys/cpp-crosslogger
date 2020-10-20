
#ifndef LOGGER_USERDEFS_HEADER
#define LOGGER_USERDEFS_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_pdefs.h"


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

#else  /*LOG_ENABLED*/

#if defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))

#if LOG_USE_MODULEDEFINITION
#define LOG_GET_CALLER_ADDR logging_get_caller_address()
#else
#define LOG_GET_CALLER_ADDR 0L
#endif  // LOG_USE_MODULEDEFINITION


#define LOGOBJ_CMD(logobj,cmdid,v,p,l) \
  (logobj)->log_cmd((cmdid), (v), LOG_GET_CALLER_ADDR, __FUNCTION__, \
                        __FILE__, __LINE__, (p), (l) )


#define LOG_CMD(cmdid,v,p,l) \
  LOGOBJ_CMD(logging::_logger->get(), (cmdid), (v), (p), (l))
//->log_cmd((cmdid), (v), LOG_GET_CALLER_ADDR, __FUNCTION__, \
 //                       __FILE__, __LINE__, (p), (l) )

#define LOGOBJ_TEXT(logobj, v, ...) \
  (logobj)->log((v), LOG_GET_CALLER_ADDR, __FUNCTION__, \
    __FILE__, __LINE__, __VA_ARGS__)


#define LOG_TEXT(v, ...) \
  LOGOBJ_TEXT(logging::_logger->get(), (v), __VA_ARGS__)

//logging::_logger->get()->log((v), LOG_GET_CALLER_ADDR, __FUNCTION__, \
//    __FILE__, __LINE__, __VA_ARGS__)

#define LOG_STREAM(v) \
  logging::_logger->get()->stream((v), LOG_GET_CALLER_ADDR, \
    __FUNCTION__, __FILE__, __LINE__)

#define LOG_INFO(...)                                                                    \
  LOG_TEXT(logging::logger_verbose_info, __VA_ARGS__)  
//logging::_logger->log(logging::logger_verbose_info, LOG_GET_CALLER_ADDR, __FUNCTION__, \
//                        __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)                                                      \
  LOG_TEXT(logging::logger_verbose_debug, __VA_ARGS__)  
//logging::_logger->log(logging::logger_verbose_debug, LOG_GET_CALLER_ADDR, \
//                        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...)                                                      \
  LOG_TEXT(logging::logger_verbose_warning, __VA_ARGS__)  
//logging::_logger->log(logging::logger_verbose_warning, LOG_GET_CALLER_ADDR, \
//                        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)                                                      \
  LOG_TEXT(logging::logger_verbose_error, __VA_ARGS__)  
//  logging::_logger->log(logging::logger_verbose_error, LOG_GET_CALLER_ADDR, \
//                        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)                                                      \
  LOG_TEXT(logging::logger_verbose_fatal, __VA_ARGS__)  

//  logging::_logger->log(logging::logger_verbose_fatal, LOG_GET_CALLER_ADDR, \
//                        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

#define LOGS_DEBUG() \
  LOG_STREAM(logging::logger_verbose_debug)
//logging::_logger->stream(logging::logger_verbose_debug, LOG_GET_CALLER_ADDR, \
  //                      __FUNCTION__, __FILE__, __LINE__)

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

#define LOG_STD_EXCEPTION(v, e)  \
  LOG_CMD(0x1008, (v), (e), 0)

#define LOG_EXCEPTION_DEBUG(e)                                                        \
  LOG_STD_EXCEPTION(logging::logger_verbose_debug, e)

//  logging::_logger->log_exception(logging::logger_verbose_debug, LOG_GET_CALLER_ADDR, \
//                                  __FUNCTION__, __FILE__, __LINE__, e)
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


#define LOG_EXCEPTION_TEXT(v, msg)  \
     LOG_CMD(0x1009, (v), (msg), 0)


#define LOG_EXCEPTION_TEXT_DEBUG(msg)                                                        \
    LOG_EXCEPTION_TEXT(logging::logger_verbose_debug, (msg))

//  logging::_logger->log_exception(logging::logger_verbose_debug, LOG_GET_CALLER_ADDR, \
//                                  __FUNCTION__, __FILE__, __LINE__, reinterpret_cast<const char*>(msg))
#define LOG_EXCEPTION_TEXT_WARNING(msg)                                                        \
    LOG_EXCEPTION_TEXT(logging::logger_verbose_warning, (msg))

//logging::_logger->log_exception(logging::logger_verbose_warning, LOG_GET_CALLER_ADDR, \
//                                  __FUNCTION__, __FILE__, __LINE__, reinterpret_cast<const char*>(msg))
#define LOG_EXCEPTION_TEXT_INFO(msg)                                                        \
    LOG_EXCEPTION_TEXT(logging::logger_verbose_info, (msg))
//logging::_logger->log_exception(logging::logger_verbose_info, LOG_GET_CALLER_ADDR, \
//                                  __FUNCTION__, __FILE__, __LINE__, reinterpret_cast<const char*>(msg))
#define LOG_EXCEPTION_TEXT_ERROR(msg)                                                        \
    LOG_EXCEPTION_TEXT(logging::logger_verbose_error, (msg))
//logging::_logger->log_exception(logging::logger_verbose_error, LOG_GET_CALLER_ADDR, \
//                                  __FUNCTION__, __FILE__, __LINE__, reinterpret_cast<const char*>(msg))
#define LOG_EXCEPTION_TEXT_FATAL(msg)                                                        \
    LOG_EXCEPTION_TEXT(logging::logger_verbose_fatal, (msg))
//logging::_logger->log_exception(logging::logger_verbose_fatal, LOG_GET_CALLER_ADDR, \
//                                  __FUNCTION__, __FILE__, __LINE__, reinterpret_cast<const char*>(msg))

#define LOG_SET_CURRENT_THREAD_NAME(name) logging::utils::set_current_thread_name(name)

#if LOG_USE_OBJMON
#  define LOG_OBJMON_REGISTER_INSTANCE() { \
  struct { unsigned int hash; const char* type_name; void* ptr; } param = { typeid(*this).hash_code(), typeid(*this).name(), this }; \
  LOG_CMD(0x1010, logging::logger_verbose_info, &param, 0); \
}

//    logging::_logger->log_objmon_register(typeid(*this).hash_code(), typeid(*this).name(), \
//                                        this)
#  define LOG_OBJMON_UNREGISTER_INSTANCE() { \
  struct { unsigned int hash; void* ptr; } param = { typeid(*this).hash_code(), this }; \
  LOG_CMD(0x1011, logging::logger_verbose_info, &param, 0); \
}

//    logging::_logger->log_objmon_unregister(typeid(*this).hash_code(), this)

#  define LOG_OBJMON_DUMP_INFO() \
  LOG_CMD(0x1012, logging::logger_verbose_info, 0L, 0)

//    logging::_logger->log_objmon_dump(logging::logger_verbose_info)
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
  void* logging::unhandled_exceptions_processor::prev_exception_filter_ = NULL;
#else  // LOG_UNHANDLED_EXCEPTIONS && defined(LOG_PLATFORM_WINDOWS)
#  define DEFINE_LOG_UNHANDLED_EXCEPTIONS_MEMBERS
#endif  // LOG_UNHANDLED_EXCEPTIONS && defined(LOG_PLATFORM_WINDOWS)

// Logger instance definition macro
#if LOG_SHARED
#  define DEFINE_LOGGER()                                                            \
    logging::logger_singleton_interface<logging::logger_interface>* logging::_logger( \
    new logging::singleton<logging::logger_interface, logging::detail::logger>( \
      &logging::logger_interface::ref, &logging::logger_interface::deref,          \
      &logging::logger_interface::ref_counter,                                     \
      (logging::logger_interface*)logging::detail::shared_obj::try_find_shared_object(0), \
      false));

//DEFINE_LOG_UNHANDLED_EXCEPTIONS_MEMBERS
#else  // LOG_SHARED
#  define DEFINE_LOGGER()                                                              \
    logging::singleton<logging::logger_interface, logging::detail::logger> logging::_logger; \
    DEFINE_LOG_UNHANDLED_EXCEPTIONS_MEMBERS
#endif  // LOG_SHARED

#else /*defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))*/

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


#if LOG_USE_OBJMON
#define LOG_OBJMON_REGISTER_INSTANCE() { \
   struct { unsigned int hash; const char* type_name; void* ptr; } param = { typeid(*this).hash_code(), typeid(*this).name(), this }; \
  __c_logger_log_cmd(0x1010, logger_verbose_info, LOG_GET_CALLER_ADDR, __FUNCTION__, __FILE__, __LINE__, &param, 0) \
}

//  __c_logger_objmon_register(typeid(*this).hash_code(), typeid(*this).name(), this)

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

#else  // LOG_USE_MODULEDEFINITION

#define LOG_MODULES_DEBUG()
#define LOG_MODULES_INFO()
#define LOG_MODULES_WARNING()
#define LOG_MODULES_ERROR()
#define LOG_MODULES_FATAL()

#endif  // LOG_USE_MODULEDEFINITION




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

#else  // LOG_AUTO_DEBUGGING

#define LOG_STACKTRACE_DEBUG
#define LOG_STACKTRACE_INFO
#define LOG_STACKTRACE_WARNING
#define LOG_STACKTRACE_ERROR
#define LOG_STACKTRACE_FATAL

#endif  // LOG_AUTO_DEBUGGING


#endif /*defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))*/


#endif /*LOG_ENABLED*/


#endif /*LOGGER_USERDEFS_HEADER*/
