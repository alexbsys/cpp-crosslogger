
/* Formatted string output 
   xprintf("%d", 1234);             "1234"
   xprintf("%6d,%3d%%", -200, 5);   "  -200,  5%"
   xprintf("%-6u", 100);            "100   "
   xprintf("%ld", 12345678L);       "12345678"
   xprintf("%04x", 0xA3);           "00a3"
   xprintf("%08LX", 0x123ABC);      "00123ABC"
   xprintf("%016b", 0x550F);        "0101010100001111"
   xprintf("%s", "String");         "String"
   xprintf("%-4s", "abc");          "abc "
   xprintf("%4s", "abc");           " abc"
   xprintf("%c", 'a');              "a"
   xprintf("%f", 10.0);             "10.00000000"
   xprintf("%.2f", 10.0);           "10.00"
   xprintf("%zu", sizeof(int));     "4" (size_t support)
   xprintf("%.*s", 3, "Hello");     "Hel" (precision for strings)

   When int64 supported:
   xprintf("%lld", 1234567812345678LL); "1234567812345678"
*/

#ifndef LOGGER_XPRINTF_HEADER
#define LOGGER_XPRINTF_HEADER

#include <log/logger_config.h>
#include <log/logger_pdefs.h>
#include <log/logger_varargs.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>

/* Support for %lld */
#define LOG_OWN_VSNPRINTF_INT64_SUPPORT   1
#define LOG_OWN_VSNPRINTF_FLOAT_SUPPORT   1

namespace logging {
namespace detail {
namespace str {

/* Put a character to buffer */
static int xputc(char** outptr, char ch) {
  int length = 0;

#if LOG_OWN_VSNPRINTF_CR_CRLF
  if (ch == '\n')
    length += xputc(outptr, '\r');  /* CR -> CRLF */
#endif /* LOG_OWN_VSNPRINTF_CR_CRLF */

  if (outptr) {
    *(*outptr) = (unsigned char)ch;
    (*outptr)++;
    ++length;
  }

  return length;
}

/* Put a null-terminated string helper with optional max_len limit */
static int xputs_helper_n(char** outptr,
    const char* str, int max_chars, int current_length, int max_str_len) {
  const int max_chars_produced_by_xputc = LOG_OWN_VSNPRINTF_CR_CRLF ? 2 : 1;
  int length = 0;
  int chars_written = 0;

  while (*str) {
    /* Check output buffer limit */
    if (current_length + max_chars_produced_by_xputc + length >= max_chars)
      return -1;
    
    /* Check string length limit (for %.*s) */
    if (max_str_len >= 0 && chars_written >= max_str_len)
      break;

    length += xputc(outptr, *str++);
    chars_written++;
  }

  return length;
}

/* Put a null-terminated string helper (no length limit) */
static int xputs_helper(char** outptr,
    const char* str, int max_chars, int current_length) {
  return xputs_helper_n(outptr, str, max_chars, current_length, -1);
}

/* Put a null-terminated wide string helper with optional max_len limit */
static int xputws_helper_n(char** outptr,
    const wchar_t* str, int max_chars, int current_length, int max_str_len) {
  const int max_chars_produced_by_xputc = LOG_OWN_VSNPRINTF_CR_CRLF ? 2 : 1;
  int length = 0;
  int chars_written = 0;

  while (*str) {
    /* Check output buffer limit */
    if (current_length + max_chars_produced_by_xputc + length >= max_chars)
      return -1;
    
    /* Check string length limit (for %.*s) */
    if (max_str_len >= 0 && chars_written >= max_str_len)
      break;

    wchar_t wch = *str++;
    char ch = (char)wch;
    length += xputc(outptr, ch);
    chars_written++;
  }

  return length;
}

/* Put a null-terminated wide string helper (no length limit) */
static int xputws_helper(char** outptr,
    const wchar_t* str, int max_chars, int current_length) {
  return xputws_helper_n(outptr, str, max_chars, current_length, -1);
}


/**
 * \brief    Printf to buffer using varargs
 * \param    outptr          Pointer to pointer to output buffer
 * \param    out_chars_count Output buffer size in bytes
 * \param    fmt             Pointer to the format string
 * \param    arp             varargs list
 * \return   length in bytes, or -1 on buffer overflow
 * 
 * Supported format specifiers:
 *   %d, %i  - signed decimal int
 *   %u      - unsigned decimal int
 *   %x, %X  - hexadecimal (lowercase/uppercase)
 *   %o      - octal
 *   %b      - binary
 *   %c      - character
 *   %s      - string
 *   %ws, %Ws - wide string
 *   %p      - pointer
 *   %f      - float/double
 *   %%      - literal %
 * 
 * Modifiers:
 *   l       - long (32-bit)
 *   ll, L   - long long (64-bit)
 *   z       - size_t (platform-dependent)
 * 
 * Flags:
 *   -       - left justify
 *   0       - zero padding
 *   [width] - minimum field width
 *   .[prec] - precision (for floats: decimal places, for strings: max chars)
 *   .*      - precision from argument
 */
static int LOG_CDECL xbuff_printf_args(char** outptr, int out_chars_count,
    const char* fmt,
    va_list arp) {
  
  /* Format flags */
  const unsigned int kZeroPadded = 1;
  const unsigned int kLeftJustified = 2;
  const unsigned int kLongFlag = 4;
  const unsigned int kSignFlag = 8;
  const unsigned int kFloatFlag = 16;
  const unsigned int kPrecisionPadded = 32;
  const unsigned int kValue64BitFlag = 64;
  const unsigned int kSizeTFlag = 128;      /* size_t modifier (z) */
  const unsigned int kPrecisionFromArg = 256; /* precision from argument (*) */
  
  unsigned int flags = 0;
  unsigned int radix = 0;
  unsigned int buf_idx = 0;
  unsigned int pad_len = 0;
  unsigned int width = 0;
  unsigned long value = 0;

#if LOG_OWN_VSNPRINTF_INT64_SUPPORT
  unsigned long long value64 = 0;
#endif /* LOG_OWN_VSNPRINTF_INT64_SUPPORT */

  char num_buf[32];
  char ch, digit;
  char* str_ptr;
  int length = 0;

#if LOG_OWN_VSNPRINTF_FLOAT_SUPPORT
  int precision = 8;          /* default precision for floats */
  unsigned long frac_value = 0;  /* fractional part value */
#endif /* LOG_OWN_VSNPRINTF_FLOAT_SUPPORT */

  int str_precision = -1;  /* precision for strings (-1 = unlimited) */

  /* How many chars can be produced by xputc function
     When \n->\r\n replace is enabled, xputc can produce 2 chars */
  const int max_chars_produced_by_xputc = LOG_OWN_VSNPRINTF_CR_CRLF ? 2 : 1;

  for (;;) {
    ch = *fmt++;
    if (!ch)
      break;

    if (ch != '%') {
      /* Pass through regular characters */
      if (length + max_chars_produced_by_xputc >= out_chars_count)
        return -1;
      length += xputc(outptr, ch);
      continue;
    }

    /* Reset state for new format specifier */
    flags = 0;
    width = 0;
    str_precision = -1;
#if LOG_OWN_VSNPRINTF_FLOAT_SUPPORT
    precision = 8;
#endif

    ch = *fmt++;
    if (!ch)
      break;

    /* Parse flags */
    if (ch == '0') {
      flags = kZeroPadded;
      ch = *fmt++;
    } else if (ch == '-') {
      flags = kLeftJustified;
      ch = *fmt++;
    }

    if (!ch)
      break;

    /* Parse width */
    while (ch >= '0' && ch <= '9') {
      width = width * 10 + (unsigned int)(ch - '0');
      ch = *fmt++;
    }

    if (!ch)
      break;

    /* Parse precision */
    if (ch == '.') {
      ch = *fmt++;
      if (!ch)
        break;
      
      if (ch == '*') {
        /* Precision from argument */
        flags |= kPrecisionFromArg;
        int arg_prec = va_arg(arp, int);
        if (arg_prec >= 0) {
#if LOG_OWN_VSNPRINTF_FLOAT_SUPPORT
          precision = arg_prec;
#endif
          str_precision = arg_prec;
        }
        ch = *fmt++;
      } else {
        /* Precision from format string */
        flags |= kPrecisionPadded;
        int prec_val = 0;
        while (ch >= '0' && ch <= '9') {
          prec_val = prec_val * 10 + (ch - '0');
          ch = *fmt++;
        }
#if LOG_OWN_VSNPRINTF_FLOAT_SUPPORT
        precision = prec_val;
#endif
        str_precision = prec_val;
      }
    }

    if (!ch)
      break;

    /* Parse size modifiers */
    if (ch == 'z') {
      /* size_t modifier */
      flags |= kSizeTFlag;
#if INTPTR_MAX == INT64_MAX
      flags |= kValue64BitFlag;
#endif
      ch = *fmt++;
    } else if (ch == 'l' || ch == 'L') {
      flags |= kLongFlag;
      ch = *fmt++;
      
      /* Check for 'll' (long long) */
      if ((flags & kLongFlag) && (ch == 'l' || ch == 'L')) {
        flags |= kValue64BitFlag;
        ch = *fmt++;
      }
    }

    if (!ch)
      break;

    /* Convert to uppercase for switch */
    digit = ch;
    if (digit >= 'a' && digit <= 'z')
      digit -= 0x20;

    switch (digit) {
    case 'S': {
      /* String */
      int s_len;
      unsigned int str_len;

      str_ptr = va_arg(arp, char*);
      if (!str_ptr)
        str_ptr = (char*)"(null)";

      /* Calculate string length (respecting precision) */
      str_len = 0;
      while (str_ptr[str_len]) {
        if (str_precision >= 0 && (int)str_len >= str_precision)
          break;
        str_len++;
      }

      /* Right padding (spaces before string) */
      pad_len = str_len;
      while (!(flags & kLeftJustified) && pad_len < width) {
        if (length + max_chars_produced_by_xputc >= out_chars_count)
          return -1;
        length += xputc(outptr, ' ');
        pad_len++;
      }

      /* Output string with precision limit */
      s_len = xputs_helper_n(outptr, str_ptr, out_chars_count, length, str_precision);
      if (s_len < 0)
        return -1;
      length += s_len;

      /* Left padding (spaces after string) */
      while (pad_len < width) {
        if (length + max_chars_produced_by_xputc >= out_chars_count)
          return -1;
        length += xputc(outptr, ' ');
        pad_len++;
      }
      continue;
    }

    case 'W': {
      /* Wide string (%ws or %Ws) */
      ch = *fmt++;
      if (!ch)
        break;

      if (ch != 's' && ch != 'S') {
        /* Not a wide string, output the W and continue */
        if (length + max_chars_produced_by_xputc >= out_chars_count)
          return -1;
        length += xputc(outptr, ch);
        continue;
      }

      int ws_len;
      unsigned int wstr_len;
      wchar_t* wstr_ptr = va_arg(arp, wchar_t*);
      
      if (!wstr_ptr)
        wstr_ptr = (wchar_t*)L"(null)";

      /* Calculate string length (respecting precision) */
      wstr_len = 0;
      while (wstr_ptr[wstr_len]) {
        if (str_precision >= 0 && (int)wstr_len >= str_precision)
          break;
        wstr_len++;
      }

      /* Right padding */
      pad_len = wstr_len;
      while (!(flags & kLeftJustified) && pad_len < width) {
        if (length + max_chars_produced_by_xputc >= out_chars_count)
          return -1;
        length += xputc(outptr, ' ');
        pad_len++;
      }

      ws_len = xputws_helper_n(outptr, wstr_ptr, out_chars_count, length, str_precision);
      if (ws_len < 0)
        return -1;
      length += ws_len;

      /* Left padding */
      while (pad_len < width) {
        if (length + max_chars_produced_by_xputc >= out_chars_count)
          return -1;
        length += xputc(outptr, ' ');
        pad_len++;
      }
      continue;
    }

    case 'C':
      /* Character */
      if (length + max_chars_produced_by_xputc >= out_chars_count)
        return -1;
      length += xputc(outptr, (char)va_arg(arp, int));
      continue;

    case 'B':
      /* Binary */
      radix = 2;
      break;

    case 'O':
      /* Octal */
      radix = 8;
      break;

    case 'D':
    case 'I':
      /* Signed decimal */
      radix = 10;
      break;

    case 'U':
      /* Unsigned decimal */
      radix = 10;
      break;

    case 'X':
      /* Hexadecimal */
      radix = 16;
      break;

    case 'P':
      /* Pointer */
      flags |= kLongFlag;
#if INTPTR_MAX == INT64_MAX
      flags |= kValue64BitFlag;
#endif
      radix = 16;
      break;

#if LOG_OWN_VSNPRINTF_FLOAT_SUPPORT
    case 'F':
      flags |= kFloatFlag;
#if INTPTR_MAX == INT64_MAX
      flags |= kValue64BitFlag;
#endif
      radix = 10;
      break;
#endif /* LOG_OWN_VSNPRINTF_FLOAT_SUPPORT */

    default:
      /* Unknown type - output as literal */
      if (length + max_chars_produced_by_xputc >= out_chars_count)
        return -1;
      length += xputc(outptr, ch);
      continue;
    }

    /* Get argument value based on type and modifiers */
#if LOG_OWN_VSNPRINTF_INT64_SUPPORT
    if ((flags & kLongFlag) && (flags & kValue64BitFlag)) {
      value64 = va_arg(arp, long long);
    } else if (flags & kSizeTFlag) {
      /* size_t handling */
#if INTPTR_MAX == INT64_MAX
      value64 = (unsigned long long)va_arg(arp, size_t);
#else
      value = (unsigned long)va_arg(arp, size_t);
#endif
    } else
#endif /* LOG_OWN_VSNPRINTF_INT64_SUPPORT */
    if (flags & kLongFlag) {
      value = (unsigned long)va_arg(arp, long);
    } else if (digit == 'D' || digit == 'I') {
      value = (unsigned long)(long)va_arg(arp, int);
    }
#if LOG_OWN_VSNPRINTF_FLOAT_SUPPORT
    else if (digit == 'F') {
      double fv = va_arg(arp, double);
      if (fv < 0.0) {
        flags |= kSignFlag;
        fv = -fv;
      }

      value = (unsigned long)fv;
#if LOG_OWN_VSNPRINTF_INT64_SUPPORT
      value64 = (unsigned long long)fv;
#endif

      double frac = fv - (double)value;
      frac_value = 0;
      for (int z = 0; z < precision; z++) {
        frac *= 10.0;
        frac_value = (unsigned long)frac;
        if (!(flags & kPrecisionPadded)) {
          double remaining = frac - (double)frac_value;
          double scale = pow(10.0, precision - z - 1);
          if ((unsigned long)(remaining * scale) == 0)
            break;
        }
      }
    }
#endif /* LOG_OWN_VSNPRINTF_FLOAT_SUPPORT */
    else {
      value = (unsigned long)va_arg(arp, unsigned int);
    }

    /* Handle signed values */
    if (!(flags & kValue64BitFlag) && (digit == 'D' || digit == 'I') && (value & 0x80000000)) {
      value = 0 - value;
      flags |= kSignFlag;
    }
#if LOG_OWN_VSNPRINTF_INT64_SUPPORT
    else if ((flags & kValue64BitFlag) && (digit == 'D' || digit == 'I') && (value64 & 0x8000000000000000ULL)) {
      value64 = 0ULL - value64;
      flags |= kSignFlag;
    }
#endif /* LOG_OWN_VSNPRINTF_INT64_SUPPORT */

    /* Convert number to string (in reverse order) */
    buf_idx = 0;
    int continue_process = 0;

#if LOG_OWN_VSNPRINTF_FLOAT_SUPPORT
    /* Handle fractional part first */
    if ((flags & kFloatFlag) && frac_value) {
      do {
        digit = (char)(frac_value % radix);
        frac_value /= radix;
        continue_process = (frac_value != 0);

        if (digit > 9)
          digit += (ch == 'x') ? 0x27 : 0x07;

        if (buf_idx < sizeof(num_buf))
          num_buf[buf_idx++] = digit + '0';
      } while (continue_process && buf_idx < sizeof(num_buf));

      if (buf_idx < sizeof(num_buf))
        num_buf[buf_idx++] = '.';
    }
#endif /* LOG_OWN_VSNPRINTF_FLOAT_SUPPORT */

    /* Convert integer part */
    do {
#if LOG_OWN_VSNPRINTF_INT64_SUPPORT
      if (flags & kValue64BitFlag) {
        digit = (char)(value64 % (unsigned long long)radix);
        value64 /= radix;
        continue_process = (value64 != 0);
      } else
#endif /* LOG_OWN_VSNPRINTF_INT64_SUPPORT */
      {
        digit = (char)(value % radix);
        value /= radix;
        continue_process = (value != 0);
      }

      if (digit > 9)
        digit += (ch == 'x') ? 0x27 : 0x07;

      if (buf_idx < sizeof(num_buf))
        num_buf[buf_idx++] = digit + '0';
    } while (continue_process && buf_idx < sizeof(num_buf));

    /* Add sign if needed */
    if ((flags & kSignFlag) && buf_idx < sizeof(num_buf))
      num_buf[buf_idx++] = '-';

    /* Output with padding */
    pad_len = buf_idx;
    digit = (flags & kZeroPadded) ? '0' : ' ';

    /* Right padding */
    while (!(flags & kLeftJustified) && pad_len < width) {
      if (length + max_chars_produced_by_xputc >= out_chars_count)
        return -1;
      length += xputc(outptr, digit);
      pad_len++;
    }

    /* Output number (reversed) */
    while (buf_idx > 0) {
      if (length + max_chars_produced_by_xputc >= out_chars_count)
        return -1;
      length += xputc(outptr, num_buf[--buf_idx]);
    }

    /* Left padding */
    while (pad_len < width) {
      if (length + max_chars_produced_by_xputc >= out_chars_count)
        return -1;
      length += xputc(outptr, ' ');
      pad_len++;
    }
  }

  return length;
}

LOG_INTERNAL_USED static int LOG_CDECL xsnprintf(char* buff, int chars_count, const char* fmt, ...) {
  char* buff_ptr = buff;
  int length;

  va_list arp;
  va_start(arp, fmt);
  length = xbuff_printf_args(&buff_ptr, chars_count, fmt, arp);
  va_end(arp);

  if (length >= 0 && length < chars_count)
    *buff_ptr = 0;

  return length;
}

LOG_INTERNAL_USED static int LOG_CDECL xvsnprintf(char* buff, int chars_count, const char* fmt, va_list args) {
  using namespace detail::str;

  va_list args_copy;
  va_copy(args_copy, args);

  char* buff_ptr = buff;
  int length;

  length = xbuff_printf_args(&buff_ptr, chars_count, fmt, args_copy);

  va_end(args_copy);

  if (length >= 0 && length < chars_count)
    *buff_ptr = 0;

  return length;
}

} /* namespace str */
} /* namespace detail */
} /* namespace logging */

#endif /* LOGGER_XPRINTF_HEADER */
