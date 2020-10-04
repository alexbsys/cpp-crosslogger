
// Formatted string output 
/*  xprintf("%d", 1234);			"1234"
xprintf("%6d,%3d%%", -200, 5);	"  -200,  5%"
xprintf("%-6u", 100);			"100   "
xprintf("%ld", 12345678L);		"12345678"
xprintf("%04x", 0xA3);			"00a3"
xprintf("%08LX", 0x123ABC);		"00123ABC"
xprintf("%016b", 0x550F);		"0101010100001111"
xprintf("%s", "String");		"String"
xprintf("%-4s", "abc");			"abc "
xprintf("%4s", "abc");			" abc"
xprintf("%c", 'a');				"a"
xprintf("%f", 10.0);            <xprintf lacks floating point support>

when int64 supported
xprintf("%lld", 1234567812345678LL); "1234567812345678"
*/


#ifndef LOGGER_XPRINTF_HEADER
#define LOGGER_XPRINTF_HEADER

#include "logger_config.h"
#include "logger_pdefs.h"
#include "logger_varargs.h"

/** Support for %lld  */
#define LOG_OWN_VSNPRINTF_INT64_SUPPORT   1

namespace logging {
namespace detail {
namespace str {

// Put a character to buffer
static int xputc(char** outptr, char c) {
  int length = 0;

  if (LOG_OWN_VSNPRINTF_CR_CRLF && c == '\n')
	length += xputc(outptr, '\r');  /* CR -> CRLF */

  if (outptr) {
    *(*outptr) = (unsigned char)c;
    (*outptr)++;
    ++length;
  }

  return length;
}

// Put a null-terminated string helper
static int xputs_helper(char** outptr,
	const char* str, int max_chars, int current_length) {
  const int max_chars_produced_by_xputc = LOG_OWN_VSNPRINTF_CR_CRLF ? 2 : 1;
  int length = 0;

  while (*str) {
    if (current_length + max_chars_produced_by_xputc + length >= max_chars)
      return -1;

    length += xputc(outptr, *str++);
  }

  return length;
}

/**
 * \brief    Printf to buffer used varargs
 * \param    outptr     Pointer to pointer to output buffer
 * \param    out_chars_count    Output buffer size in bytes
 * \param    fmt        Pointer to the format string
 * \param    arp        varargs list
 * \return   length in bytes
 */
static int LOG_CDECL xbuff_printf_args(char** outptr,  int out_chars_count,
	const char* fmt, /* Pointer to the format string */
	va_list arp) {
  const unsigned int kZeroPadded = 1;
  const unsigned int kLeftJustified = 2;
  const unsigned int kSignFlag = 8;
  const unsigned int kLongFlag = 4;
  const unsigned int kValue64BitFlag = 32;
  
  unsigned int r, i, j, w, f;
  unsigned long v;

#if LOG_OWN_VSNPRINTF_INT64_SUPPORT
  unsigned long long v64;
#endif /*LOG_OWN_VSNPRINTF_INT64_SUPPORT*/

  char s[32], c, d, *p;
  int length = 0;

  // how many chars can be produced by xputc function
  // When \n->\r\n replace is enabled, xputc can produce 2 chars
  const int max_chars_produced_by_xputc = LOG_OWN_VSNPRINTF_CR_CRLF ? 2: 1;

	for (;;) {
		c = *fmt++;					/* Get a char */
		if (!c)
			break;				/* End of format? */

		if (c != '%') { /* Pass through it if not a % sequense */
          if (length + max_chars_produced_by_xputc >= out_chars_count)
            return -1;
				
          length += xputc(outptr, c);
          continue;
		}

		f = 0;
		c = *fmt++;	  /* Get first char of the sequense */
		if (c == '0' || c == '.') {				/* Flag: '0' padded */
			f = kZeroPadded;
			c = *fmt++;
		}
		else {
			if (c == '-') {			/* Flag: left justified */
				f = kLeftJustified;
				c = *fmt++;
			}
		}

		for (w = 0; c >= '0' && c <= '9'; c = *fmt++)	/* Minimum width */
			w = w * 10 + c - '0';

		if (c == 'l' || c == 'L') {	/* Prefix: Size is long int */
			f |= kLongFlag; c = *fmt++;
		}

    if ((f & kLongFlag) && (c == 'l' || c == 'L')) {  /* Prefix: size is long long */
      f |= kValue64BitFlag; c = *fmt++;
    }

		if (!c)
			break;    /* End of format? */

		d = c;
		if (d >= 'a')
			d -= 0x20;

		switch (d) {				/* Type is... */
		case 'S':  { /* String */
			int s_len;

			p = va_arg(arp, char*);
			for (j = 0; p[j]; j++);
			while (!(f & 2) && j++ < w) {
              if (length + max_chars_produced_by_xputc >= out_chars_count)
                return -1;
				
              length += xputc(outptr, ' ');
			}

			s_len = xputs_helper(outptr, p, out_chars_count, length);
			if (s_len < 0)
              return -1;

			while (j++ < w) {
              if (length + max_chars_produced_by_xputc >= out_chars_count)
                return -1;

              length += xputc(outptr, ' ');
			}
			continue;
        }
		case 'C':  /* Character */
            if (length + max_chars_produced_by_xputc >= out_chars_count)
              return -1;
			length += xputc(outptr, (char)va_arg(arp, int));
			continue;
		case 'B':  /* Binary */
			r = 2;
			break;
		case 'O':					/* Octal */
			r = 8;
			break;
		case 'D':					/* Signed decimal */
		case 'U':					/* Unsigned decimal */
			r = 10;
			break;
		case 'X':					/* Hexdecimal */
			r = 16;
			break;
		default:					/* Unknown type (passthrough) */
            if (length + max_chars_produced_by_xputc >= out_chars_count)
              return -1;
			length += xputc(outptr, c);
			continue;
		}

		/* Get an argument and put it in numeral */
#if LOG_OWN_VSNPRINTF_INT64_SUPPORT
    if ((f & kLongFlag) && (f & kValue64BitFlag)) {
      v64 = va_arg(arp, long long);
    }
    else 
#endif /*LOG_OWN_VSNPRINTF_INT64_SUPPORT*/      
    if (f & kLongFlag) {
      v = (long)va_arg(arp, long);
    }
    else if (d == 'D') {
      v = (long)va_arg(arp, int);
    }
    else {
      v = (long)va_arg(arp, unsigned int);
    }

		if (!(f & kValue64BitFlag) && d == 'D' && (v & 0x80000000)) {
			v = 0 - v;
			f |= kSignFlag;
    }
#if LOG_OWN_VSNPRINTF_INT64_SUPPORT
    else if ((f & kValue64BitFlag) && d == 'D' && (v64 & 0x8000000000000000ULL)) {
      v64 = 0LL - v64;
      f |= kSignFlag;
    }
#endif /*LOG_OWN_VSNPRINTF_INT64_SUPPORT*/

		i = 0;
    int continue_process = 0;

    do {
#if LOG_OWN_VSNPRINTF_INT64_SUPPORT
      if (f & kValue64BitFlag) {
        d = (char)(v64 % (unsigned long long)r);
        v64 /= r;
        continue_process = !!v64;
      }
      else
#endif /*LOG_OWN_VSNPRINTF_INT64_SUPPORT*/
      {
        d = (char)(v % r);
        v /= r;
        continue_process = !!v;
      }

      if (d > 9)
        d += (c == 'x') ? 0x27 : 0x07;

      s[i++] = d + '0';
    } while (continue_process && i < sizeof(s));

		if (f & kSignFlag)
			s[i++] = '-';

		j = i;
		d = (f & kZeroPadded) ? '0' : ' ';

		while (!(f & kLeftJustified) && j++ < w) {
      if (length + max_chars_produced_by_xputc >= out_chars_count)
        return -1;
			length += xputc(outptr, d);
		}

		do {
      if (length + max_chars_produced_by_xputc >= out_chars_count)
        return -1;
			length += xputc(outptr, s[--i]);
		} while (i);

		while (j++ < w) {
      if (length + max_chars_produced_by_xputc >= out_chars_count)
        return -1;
			length += xputc(outptr, ' ');
		}
	}

	return length;
}

static int LOG_CDECL xsnprintf(char* buff, int chars_count, const char* fmt, ...) {
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

static int LOG_CDECL xvsnprintf(char* buff, int chars_count, const char* fmt, va_list args) {
  using namespace detail::str;

  char* buff_ptr = buff;
  int length;

  length = xbuff_printf_args(&buff_ptr, chars_count, fmt, args);

  if (length >= 0 && length < chars_count)
    *buff_ptr = 0;

  return length;
}

}//namespace str
}//namespace detail
}//namespace logging

#endif /*LOGGER_XPRINTF_HEADER*/
