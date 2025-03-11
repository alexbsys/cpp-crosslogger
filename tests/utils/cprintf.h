#ifndef CPRINTF_H
#define CPRINTF_H

#include <stdarg.h>
#include <stdbool.h>

#ifdef CPRINTF_DLL
#ifdef CPRINTF_BUILDING
#define CPRINTF_EXPORT __declspec(dllexport)
#else
#define CPRINTF_EXPORT __declspec(dllimport)
#endif
#else
#define CPRINTF_EXPORT
#endif

#ifdef _WIN32
#include <windows.h>
#define CPRINTF_STDOUT GetStdHandle(STD_OUTPUT_HANDLE)
#define CPRINTF_STDERR GetStdHandle(STD_ERROR_HANDLE)
typedef HANDLE cprintf_fd_t;
#else
#include <stdio.h>
#define CPRINTF_STDOUT stdout
#define CPRINTF_STDERR stderr
typedef FILE *cprintf_fd_t;
#endif

typedef struct
{
    cprintf_fd_t fd;
#ifdef _WIN32
    WORD default_attributes;
    WORD default_inverted_attributes;
#endif
} cprintf_t;

#ifdef __cplusplus
extern "C"
{
#endif

    CPRINTF_EXPORT bool colored_printf_use(cprintf_t *out, const cprintf_fd_t type);
    CPRINTF_EXPORT bool colored_printf(cprintf_t *in, const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif
