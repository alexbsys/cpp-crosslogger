

#ifndef LOGGER_STRING_UTILS_HEADER
#define LOGGER_STRING_UTILS_HEADER

#include <log/logger_config.h>
#include <log/logger_varargs.h>
#include <string>
#include <vector>
#include <string.h>

#if LOG_USE_OWN_VSNPRINTF
#include "logger_xprintf.h"
#endif /*LOG_USE_OWN_VSNPRINTF*/

//////////////   String functions    //////////////
namespace logging {
namespace detail {
namespace str {
  static void format_arguments_list(std::string& result_str, const char* format,
    va_list arguments) {
    static const int kStartBufferSize = 512;
    static const int kBufferSizeIncrementBytes = 512;

    // static buffer is faster than heap allocated
    char static_buf[kStartBufferSize];
    int buffer_size = kStartBufferSize;

    char* buffer = static_buf;
    int need_delete = 0;

    do {
      int result;
#if LOG_USE_OWN_VSNPRINTF
      result = xvsnprintf(buffer, buffer_size - 1, format, arguments);
#else LOG_USE_OWN_VSNPRINTF
#  ifdef LOG_COMPILER_MSVC
      result = _vsnprintf_s(buffer, buffer_size, buffer_size - 1, format, arguments);
#  else   // LOG_COMPILER_MSVC
      result = vsnprintf(buffer, buffer_size - 1, format, arguments);
#  endif  // LOG_COMPILER_MSVC
#endif // LOG_USE_OWN_VSNPRINTF

      if (result >= 0 && result < buffer_size) break;

      if (result >= buffer_size) {
        buffer_size = result + 1;
      } else {
        buffer_size += kBufferSizeIncrementBytes;
      }

      if (!need_delete) {
        need_delete = 1;
        buffer = NULL;
      }

      buffer = reinterpret_cast<char*>(realloc(buffer, buffer_size));
      if (!buffer) break;
    } while (true);

    result_str = buffer ? buffer : std::string();
    if (need_delete) free(buffer);
  }

  /** 
   * \brief    stringformat function. vsprintf analog for std::string
   */
  static std::string stringformat(const char* format, ...) {
    using namespace detail::str;

    va_list arguments;
    va_start(arguments, format);
    std::string result;
    format_arguments_list(result, format, arguments);
    va_end(arguments);
    return result;
  }

  static bool contains(const char* str, const char* find_str) {
    return strstr(str, find_str) != NULL;
  }

  static bool compare(const char* str1, const char* str2) {
    return strcmp(str1, str2) == 0;
  }

  static bool starts_with(const std::string& str, const std::string& what) {
    return str.substr(0, what.length()) == what;
  }
  
  static std::string& replace(std::string& str, const char* find_str,
    const std::string& replaceStr) {
    size_t i = 0;
    const char* ptr = NULL;
    size_t find_str_len = strlen(find_str);

    while (((ptr = strstr(str.c_str(), find_str)) != NULL) && ptr >= str.c_str()) {
      str = static_cast<std::string>(str).replace(ptr - str.c_str(), find_str_len,
        replaceStr);
      i += replaceStr.length();
    }
    return str;
  }

  static void split(std::string str, std::vector<std::string>& out_strings, char ch = ' ') {
    while (true) {
      size_t position = str.find_first_of(ch);
      std::string current = str.substr(0, position);

      if (current.size()) out_strings.push_back(current);

      if (position == std::string::npos) break;

      str = str.substr(position);
      position = str.find_first_not_of(ch);
      if (position == std::string::npos) break;

      str = str.substr(position);
    }
  }

}//namespace str
}//namespace detail
}//namespace logging

#endif /*LOGGER_STRING_UTILS_HEADER*/
