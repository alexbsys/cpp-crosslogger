
#ifndef LOGGER_GET_CALLER_ADDR_HEADER
#define LOGGER_GET_CALLER_ADDR_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"


////////////////  logging_get_caller_address  implementation start //////////////////
/// When module definition is enabled, logger will need to know current caller code address (EIP/RIP register).
/// logging_get_caller_address returns caller address and it is implemented differently depend on compiler type
/// This part cannot be placed in namespace because it is actual for both C and C++ code
#if LOG_USE_MODULEDEFINITION

#ifdef LOG_COMPILER_MSVC   // Implementation for Microsoft Visual C++ compiler

// MSVC has builtin function '_ReturnAddress' for obtain return address of function
#ifdef LOG_CPP
extern "C"
#endif  //LOG_CPP
void* _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)
#pragma optimize("", off)

#endif  // LOG_COMPILER_MSVC
#if defined(LOG_COMPILER_GCC) || defined(LOG_COMPILER_MINGW)   // Implementation for GCC compiler
#pragma GCC push_options
#pragma GCC optimize("O0")
#endif  // defined(LOG_COMPILER_GCC) || defined(LOG_COMPILER_MINGW)

/**
* \brief    get_caller_address function. Returns caller address
* \return   caller address
*/
static void* logging_get_caller_address() {
#ifdef LOG_COMPILER_MSVC
  return _ReturnAddress();
#endif  // LOG_COMPILER_MSVC

#if defined(LOG_COMPILER_GCC) || defined(LOG_COMPILER_MINGW) || defined(LOG_COMPILER_ICC)
  return __builtin_return_address(0);
#endif  // defined(LOG_COMPILER_GCC) || defined(LOG_COMPILER_MINGW) ||
  // defined(LOG_COMPILER_ICC)
}

#ifdef LOG_COMPILER_MSVC
#pragma optimize("", on)
#endif  // LOG_COMPILER_MSVC

#if defined(LOG_COMPILER_GCC) || defined(LOG_COMPILER_MINGW)
#pragma GCC pop_options
#endif  // defined(LOG_COMPILER_GCC) || defined(LOG_COMPILER_MINGW)

#endif  // LOG_USE_MODULEDEFINITION
////////////////  logging_get_caller_address  implementation end //////////////////

#endif /*LOGGER_GET_CALLER_ADDR_HEADER*/
