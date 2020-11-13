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
#include <log/logger_register_builtin_plugin.h>

#if !LOG_USE_DLL || defined(LOG_THIS_IS_DLL)

void on_logger_created_fn(logging::logger_interface* log);

DEFINE_LOGGER(&on_logger_created_fn);

///////   C tail  ///////

#ifndef LOG_PLATFORM_WINDOWS
extern "C"
void __cxa_pure_virtual() {}
#endif //LOG_PLATFORM_WINDOWS

void on_logger_created_fn(logging::logger_interface* log) {
  if (log) {
    log->register_plugin_factory(new logging::logger_register_builtin_plugin_factory());
  }
}


#if LOG_ENABLED



extern "C"
void* __c_logger_get_logger() {
  if (!logging::_logger->get())
    return NULL;

  return logging::_logger->get();
}

extern "C"
void* c_logger_ref(void* logobj) {
  void* log_interface_vptr = logobj ? logobj : __c_logger_get_logger();
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(log_interface_vptr);

  if (!logi)
    return NULL;

  logi->ref();
  return log_interface_vptr;
}

extern "C"
void* c_logger_deref(void* logobj) {
  void* log_interface_vptr = logobj ? logobj : __c_logger_get_logger();
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(log_interface_vptr);

  if (!logi)
    return NULL;

  if (logi->deref() == 0)
    return NULL;

  return log_interface_vptr;
}

extern "C"
unsigned int c_logger_get_version(void* logobj) {
  void* log_interface_vptr = logobj ? logobj : __c_logger_get_logger();
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(log_interface_vptr);

  if (!logi)
    return NULL;

  return logi->get_version();
}

extern "C"
void __c_logger_log_args(void* logobj, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, va_list args) {
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return;

  logi->log_args(verbose_level, caller_addr, function, file, line, format, args);
  c_logger_deref(reinterpret_cast<void*>(logi));
}

extern "C"
void __c_logger_log(void* logobj, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, ...) {
	va_list arguments;
	va_start(arguments, format);

  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return;

	logi->log_args(verbose_level, caller_addr, function, file, line, format, arguments);
  c_logger_deref(reinterpret_cast<void*>(logi));

	va_end(arguments);
}

extern "C"
void __c_logger_log_cmd(void* logobj, int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam) {
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return;

  logi->log_cmd(cmd_id, verbose_level, caller_addr, function, file, line, vparam, iparam);
  c_logger_deref(reinterpret_cast<void*>(logi));
}

extern "C"
void __c_logger_log_cmd_args(void* logobj, int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam, va_list args) {
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return;

  logi->log_cmd_args(cmd_id, verbose_level, caller_addr, function, file, line, vparam, iparam, args);
  c_logger_deref(reinterpret_cast<void*>(logi));
}

extern "C"
void __c_logger_set_current_thread_name(const char* thread_name) {
  logging::detail::utils::set_current_thread_name(thread_name);
}

extern "C"
void __c_logger_set_config_param(void* logobj, const char* key, const char* value) {
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return;

  logi->set_config_param(key, value);
  c_logger_deref(reinterpret_cast<void*>(logi));
}

extern "C"
int __c_logger_get_config_param(void* logobj, const char* key, char* value, int buffer_size) {
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return 0;

  int ret = logi->get_config_param(key, value, buffer_size);
  
  c_logger_deref(reinterpret_cast<void*>(logi));
  return ret;
}

extern "C"
void  __c_logger_reload_config(void* logobj) {
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return;

  logi->reload_config();
  c_logger_deref(reinterpret_cast<void*>(logi));
}

extern "C"
void __c_logger_dump_state(void* logobj, int verbose_level) {
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return;

  logi->dump_state(verbose_level);
  c_logger_deref(reinterpret_cast<void*>(logi));
}

extern "C"
int __c_logger_register_plugin_factory(void* logobj, void* factory_interface) {
  int ret;
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return 0;

  ret = logi->register_plugin_factory(reinterpret_cast<logging::logger_plugin_factory_interface*>(factory_interface)) ? 1 : 0;
  c_logger_deref(reinterpret_cast<void*>(logi));

  return ret;
}

extern "C"
int __c_logger_unregister_plugin_factory(void* logobj, void* factory_interface) {
  int ret;
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return 0;

  ret = logi->unregister_plugin_factory(reinterpret_cast<logging::logger_plugin_factory_interface*>(factory_interface)) ? 1 : 0;
  c_logger_deref(reinterpret_cast<void*>(logi));

  return ret;
}

extern "C"
int __c_logger_attach_plugin(void* logobj, void* plugin_interface) {
  int ret;
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return 0;

  ret = logi->attach_plugin(reinterpret_cast<logging::logger_plugin_interface*>(plugin_interface)) ? 1 : 0;
  c_logger_deref(reinterpret_cast<void*>(logi));

  return ret;
}

extern "C"
int __c_logger_detach_plugin(void* logobj, void* plugin_interface) {
  int ret;
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return 0;

  ret = logi->detach_plugin(reinterpret_cast<logging::logger_plugin_interface*>(plugin_interface)) ? 1 : 0;
  c_logger_deref(reinterpret_cast<void*>(logi));

  return ret;
}

extern "C"
void __c_logger_flush(void* logobj) {
  logging::logger_interface* logi = reinterpret_cast<logging::logger_interface*>(c_logger_ref(logobj));
  if (!logi)
    return;

  logi->flush();
  c_logger_deref(reinterpret_cast<void*>(logi));
}

#endif //LOG_ENABLED

#endif // !LOG_USE_DLL || defined(LOG_THIS_IS_DLL)
