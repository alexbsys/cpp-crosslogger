
#ifndef LOGGER_VERBOSE_HEADER
#define LOGGER_VERBOSE_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_pdefs.h"

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

#if defined(_DEBUG) || defined(LOG_PLATFORM_ANDROID)
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






#endif /*LOGGER_VERBOSE_HEADER*/
