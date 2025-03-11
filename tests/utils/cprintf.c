#include "cprintf.h"

#include <ctype.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#include <stdio.h>
#endif

static cprintf_t global_cprintf
#ifndef CPRINTF_DLL
    = {NULL}
#endif
;

CPRINTF_EXPORT bool colored_printf_use(cprintf_t *handle, const cprintf_fd_t type)
{
    if (handle == NULL)
    {
        handle = &global_cprintf;
    }

    handle->fd = type;

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(handle->fd, &info) == 0)
        return false;

    handle->default_attributes = info.wAttributes;
    handle->default_inverted_attributes = (info.wAttributes & 0x8088) | ((info.wAttributes & 0x77) ^ 0x77);
#endif

    return true;
}

#ifdef _WIN32
#define cprintf_handle_fmt_specifier(fmt, in, out_attributes) __cprintf_handle_fmt_specifier(fmt, in, out_attributes)
#else
#define cprintf_handle_fmt_specifier(fmt, in, out_attributes) __cprintf_handle_fmt_specifier(fmt, in)
#endif

static const char *__cprintf_handle_fmt_specifier(const char *fmt, const cprintf_t *in
#ifdef _WIN32
                                            ,
                                            WORD *out_attributes // extra argument (on windows)
#endif
)
{
#ifdef _WIN32
    WORD attributes;
#else
    unsigned char code;
#endif

    const char lower = (char)tolower(*fmt);
    unsigned char offset = 1;

    if (lower == 'f' || lower == 'b')
    {
        offset++;

#ifndef _WIN32
        code = (30 + (10 * (lower == 'b'))) + (60 * (lower != *fmt));
#endif

        switch ((char)tolower(fmt[1]))
        {
        case 'r':
#ifdef _WIN32
            attributes = 0x04;
#else
            code++;
#endif
            break;
        case 'g':
#ifdef _WIN32
            attributes = 0x02;
#else
            code += 2;
#endif
            break;
        case 'y':
#ifdef _WIN32
            attributes = 0x06;
#else
            code += 3;
#endif
            break;
        case 'b':
#ifdef _WIN32
            attributes = 0x01;
#else
            code += 4;
#endif
            break;
        case 'm':
#ifdef _WIN32
            attributes = 0x05;
#else
            code += 5;
#endif
            break;
        case 'c':
#ifdef _WIN32
            attributes = 0x03;
#else
            code += 6;
#endif
            break;
        case 'w':
#ifdef _WIN32
            attributes = 0x07;
#else
            code += 7;
#endif
            break;
        case 'k':
#ifdef _WIN32
            attributes = 0;
#endif
            break;
        default:
            return NULL;
        }

#ifdef _WIN32
        if (lower == 'b')
        {
            attributes = attributes << 4;
        }
#endif
    }
    else
    {
        switch (lower)
        {
        case 'i':
#ifdef _WIN32
            attributes = in->default_inverted_attributes;
#else
            code = 7;
#endif
            break;
        case 'u':
#ifdef _WIN32
            attributes = COMMON_LVB_UNDERSCORE;
#else
            code = 4;
#endif
            break;
        default:
            return NULL;
        }
    }

#ifdef _WIN32
    if (lower != *fmt)
    {
        attributes |= (8 << (4 * (lower == 'b')));
    }

    *out_attributes |= attributes;
#else
    fprintf(in->fd, "\x1b[%dm", code);
#endif

    return fmt + offset;
}

#ifdef _WIN32
#define cprintf_print_colors(in, vl, attributes) __cprintf_print_colors(in, vl, attributes)
#else
#define cprintf_print_colors(in, vl, attributes) __cprintf_print_colors(in, vl)
#endif

static void __cprintf_print_colors(const cprintf_t *in, va_list *vl
#ifdef _WIN32
                                   ,
                                   WORD attributes
#endif
)
{
#ifdef _WIN32
    if (attributes == COMMON_LVB_UNDERSCORE)
    {
        attributes = COMMON_LVB_UNDERSCORE | in->default_attributes; // poor windows...
    }

    const char *s = va_arg(*vl, char *);

    SetConsoleTextAttribute(in->fd, attributes);
    WriteConsoleA(in->fd, s, (DWORD)strlen(s), NULL, NULL);
    SetConsoleTextAttribute(in->fd, in->default_attributes);
#else
    fprintf(in->fd, "%s\x1b[0m", va_arg(*vl, char *));
#endif
}

static const char *cprintf_handle_fmt(const cprintf_t *in, const char *fmt, va_list *vl)
{
    if (*fmt == '%')
    {
#ifdef _WIN32
        WriteConsoleA(in->fd, fmt, 1, NULL, NULL);
#else
        fputc('%', in->fd);
#endif

        return fmt + 1; // escaped, ignore
    }

#ifdef _WIN32
    WORD attributes = 0;
#endif

    if (*fmt == '{')
    {
        fmt++; // don't parse the { stupid

        // too lazy to use while (1) thanks

        // cprintf_handle_fmt_specifier and cprintf_print_colors are a macro
        // the last argument will be taken out on unix platforms
    cprintf_handle_loop:
        if ((fmt = cprintf_handle_fmt_specifier(fmt, in, &attributes)) != NULL)
        {
            if (*fmt == ',')
            {
                fmt++;
                goto cprintf_handle_loop;
            }
            else if (*fmt == '}')
            { // end of concatenation
                cprintf_print_colors(in, vl, attributes);

                return fmt + 1;
            }
        }
    }
    else if ((fmt = cprintf_handle_fmt_specifier(fmt, in, &attributes)) != NULL)
    {
        cprintf_print_colors(in, vl, attributes);

        return fmt;
    }

    return NULL;
}

CPRINTF_EXPORT bool colored_printf(cprintf_t *in, const char *start, ...)
{
    if (in == NULL)
    {
        in = &global_cprintf;
    }

#ifndef CPRINTF_DLL
    if (in->fd == NULL && colored_printf_use(in, CPRINTF_STDOUT) == 0)
    {
        return false;
    }
#endif

    va_list vl;
    va_start(vl, start);

    const char *former = start;

    while ((start = strchr(start, '%')) != NULL)
    {
#ifdef _WIN32
        WriteConsoleA(in->fd, former, (DWORD)(start - former), NULL, NULL);
#else
        fprintf(in->fd, "%.*s", (int)(start - former), former);
#endif

        if ((start = cprintf_handle_fmt(in, start + 1, &vl)) == NULL)
        {
          va_end(vl);
          return false;
        }
        else
        {
            former = start;
        }
    }

    // just in case they forgot any
#ifdef _WIN32
    WriteConsoleA(in->fd, former, (DWORD)strlen(former), NULL, NULL);
#else
    fputs(former, in->fd);
#endif

    va_end(vl);
    return true;
}

#ifdef CPRINTF_DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
    return dwReason != DLL_PROCESS_ATTACH || (BOOL)cprintf_use(&global_cprintf, CPRINTF_STDOUT);
}
#endif
