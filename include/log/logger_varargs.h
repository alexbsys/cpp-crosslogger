
#ifndef LOGGER_VARARGS_HEADER
#define LOGGER_VARARGS_HEADER

#include "logger_pdefs.h"

#ifdef ANSI
#include <stdarg.h>
int average(int first, ...);
#else  // ANSI
#include <vadefs.h>
#include <varargs.h>
int average(va_list);
#endif  // ANSI

#endif /*LOGGER_VARARGS_HEADER*/
