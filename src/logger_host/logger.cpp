// logger.cpp
// Smart logger library for C/C++
// alexb 2009-2020

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, 
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.

// You need to add this file only if you need to use C tail of library
// DO NOT ADD BOTH LOGGER.CPP AND LOGGERDLL.C

#include <log/logger.h>

#if !LOG_USE_DLL || defined(LOG_THIS_IS_DLL)

DEFINE_LOGGER();

///////   C tail  ///////

#ifndef LOG_PLATFORM_WINDOWS
extern "C"
void __cxa_pure_virtual() {}
#endif //LOG_PLATFORM_WINDOWS


#if LOG_ENABLED

extern "C"
void* __c_logger_get_logger() {
  if (!logging::_logger->get())
    return NULL;

  return logging::_logger->get();
}

extern "C"
void __c_logger_log_args(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, va_list args) {
	logging::_logger->get()->log_args(verbose_level, caller_addr, function, file, line, format, args);
}

extern "C"
void __c_logger_log(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, ...) {
	va_list arguments;
	va_start(arguments, format);

	logging::_logger->get()->log_args(verbose_level, caller_addr, function, file, line, format, arguments);

	va_end(arguments);
}

extern "C"
void __c_logger_log_cmd(int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam) {
  logging::_logger->get()->log_cmd(cmd_id, verbose_level, caller_addr, function, file, line, vparam, iparam);
}

extern "C"
void __c_logger_log_cmd_args(int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam, va_list args) {
  logging::_logger->get()->log_cmd_args(cmd_id, verbose_level, caller_addr, function, file, line, vparam, iparam, args);
}
/*
extern "C"
void __c_logger_objmon_register(size_t hash_code, const char* type_name, void* ptr) {
#if LOG_USE_OBJMON
  logging::_logger->get()->log_objmon_register(hash_code, type_name, ptr);
#endif //LOG_USE_OBJMON
}

extern "C"
void __c_logger_objmon_unregister(size_t hash_code, void* ptr) {
#if LOG_USE_OBJMON
  logging::_logger->get()->log_objmon_unregister(hash_code, ptr);
#endif //LOG_USE_OBJMON
}

extern "C"
void __c_logger_objmon_dump(int verbose_level) {
#if LOG_USE_OBJMON
  logging::_logger->get()->log_objmon_dump(verbose_level);
#endif //LOG_USE_OBJMON
}
*/
extern "C"
void __c_logger_set_current_thread_name(const char* thread_name) {
  logging::detail::utils::set_current_thread_name(thread_name);
}

extern "C"
void __c_logger_set_config_param(const char* key, const char* value) {
  logging::_logger->get()->set_config_param(key, value);
}

extern "C"
int __c_logger_get_config_param(const char* key, char* value, int buffer_size) {
  const char* val = logging::_logger->get()->get_config_param(key);
  if (!val)
    return 0;

  int len = strlen(val);

  if (len >= buffer_size)
    return len;

  strncpy(value, val, buffer_size);
  return len;
}

#endif //LOG_ENABLED

#endif // !LOG_USE_DLL || defined(LOG_THIS_IS_DLL)
