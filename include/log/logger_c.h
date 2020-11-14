
// logger declaration part
// This file can be included from C or CPP part

#ifndef LOGGER_C_HEADER
#define LOGGER_C_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_pdefs.h"

#ifdef LOG_CPP
extern "C" {
#endif /*LOG_CPP*/

#if LOG_USE_DLL && !LOG_THIS_IS_DLL
extern void(LOG_CDECL* __c_logger_log)(void* logobj, int verbose_level, void* caller_addr,
                                       const char* function, const char* file, int line,
                                       const char* format, ...);
extern void(LOG_CDECL* __c_logger_log_args)(void* logobj, int verbose_level, void* caller_addr,
                                            const char* function, const char* file,
                                            int line, const char* format, va_list args);

extern void(LOG_CDECL* __c_logger_log_cmd)(void* logobj, int cmd_id, int verbose_level, void* caller_addr,
  const char* function, const char* file,
  int line, const void* vparam, int iparam);

extern void(LOG_CDECL* __c_logger_log_cmd_args)(void* logobj, int cmd_id, int verbose_level, void* caller_addr,
  const char* function, const char* file,
  int line, const void* vparam, int iparam, va_list args);

extern void(LOG_CDECL* __c_logger_set_current_thread_name)(void* logobj, const char* thread_name);

extern void(LOG_CDECL* __c_logger_set_config_param)(void* logobj, const char* key, const char* value);
extern int(LOG_CDECL* __c_logger_get_config_param)(void* logobj, const char* key, char* value, int buffer_size);


extern void(LOG_CDECL* __c_logger_reload_config)(void* logobj);
extern void(LOG_CDECL* __c_logger_dump_state)(void* logobj, int verbose_level);
extern int(LOG_CDECL* __c_logger_register_plugin_factory)(void* logobj, void* factory_interface);
extern int(LOG_CDECL* __c_logger_unregister_plugin_factory)(void* logobj, void* factory_interface);
extern int(LOG_CDECL* __c_logger_attach_plugin)(void* logobj, void* plugin_interface);
extern int(LOG_CDECL* __c_logger_detach_plugin)(void* logobj, void* plugin_interface);
extern void(LOG_CDECL* __c_logger_flush)(void* logobj);

extern unsigned int(LOG_CDECL* c_logger_get_version)(void* logobj);

extern int(LOG_CDECL* c_logger_is_master)(void* logobj);

#else  /* LOG_USE_DLL && !LOG_THIS_IS_DLL */
void __c_logger_log(void* logobj, int verbose_level, void* caller_addr, const char* function,
                    const char* file, int line, const char* format, ...);
void __c_logger_log_args(void* logobj, int verbose_level, void* caller_addr, const char* function,
                         const char* file, int line, const char* format, va_list args);

void __c_logger_log_cmd(void* logobj, int cmd_id, int verbose_level, void* caller_addr, const char* function,
  const char* file, int line, const void* vparam, int iparam);

void __c_logger_log_cmd_args(void* logobj, int cmd_id, int verbose_level, void* caller_addr, const char* function,
  const char* file, int line, const void* vparam, int iparam, va_list args);

void __c_logger_set_config_param(void* logobj, const char* key, const char* value);
int __c_logger_get_config_param(void* logobj, const char* key, char* value, int buffer_size);

void __c_logger_set_current_thread_name(void* logobj, const char* thread_name);

void  __c_logger_reload_config(void* logobj);
void __c_logger_dump_state(void* logobj, int verbose_level);
int __c_logger_register_plugin_factory(void* logobj, void* factory_interface);
int __c_logger_unregister_plugin_factory(void* logobj, void* factory_interface);
int __c_logger_attach_plugin(void* logobj, void* plugin_interface);
int __c_logger_detach_plugin(void* logobj, void* plugin_interface);
void __c_logger_flush(void* logobj);

unsigned int c_logger_get_version(void* logobj);


#endif  /*LOG_USE_DLL && !LOG_THIS_IS_DLL*/

#ifdef LOG_CPP
}; /*extern "C"*/
#endif /*LOG_CPP*/


#endif /*LOGGER_C_HEADER*/
