
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, 
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.

// Add this file to project if you use logger in separated DLL
// DO NOT ADD BOTH LOGGER.CPP AND LOGGERDLL.C

#include <log/logger.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#	ifdef ANSI 
#		include <stdarg.h>
int average( int first, ... );
#	else //ANSI
#		include <vadefs.h>
#		include <varargs.h>
int average( va_list );
#	endif //ANSI

#ifdef LOG_PLATFORM_WINDOWS
#	include <windows.h>
#else //LOG_PLATFORM_WINDOWS
#	include <dlfcn.h>
#	include <limits.h>

#	ifdef LOG_HAVE_UNISTD_H
#		include <unistd.h>
#	endif //LOG_HAVE_UNISTD_H
#endif //LOG_PLATFORM_WINDOWS

#if LOG_USE_DLL && !LOG_THIS_IS_DLL

#ifdef LOG_PLATFORM_MAC
#  include <mach-o/dyld.h>
#endif //LOG_PLATFORM_MAC

void* __logger_dll_handle = NULL;
int __logger_try_load = 0;

int LOG_CDECL logger_load_dll();

void LOG_CDECL __c_logger_log_args_dummy(void* logobj, int verbose_level, void* caller_addr, const char* function, const char* file,
  int line, const char* format, va_list args) {
	if (logger_load_dll())
		__c_logger_log_args(logobj, verbose_level, caller_addr, function, file, line, format, args);
}

void LOG_CDECL __c_logger_log_dummy(void* logobj, int verbose_level, void* caller_addr, const char* function, const char* file,
  int line, const char* format, ...) {
	va_list arguments;
	va_start(arguments, format);

	if (logger_load_dll())
		__c_logger_log_args(logobj, verbose_level, caller_addr, function, file, line, format, arguments);

	va_end(arguments);
}

void LOG_CDECL __c_logger_log_cmd_dummy(void* logobj, int cmd_id, int verbose_level, void* caller_addr, 
  const char* function, const char* file, int line, const void* vparam, int iparam) { 
	if (logger_load_dll() && __c_logger_log_cmd != __c_logger_log_cmd_dummy)
		__c_logger_log_cmd(logobj, cmd_id, verbose_level, caller_addr, function, file, line, vparam, iparam);
}

void LOG_CDECL __c_logger_log_cmd_args_dummy(void* logobj, int cmd_id, int verbose_level, void* caller_addr, 
  const char* function, const char* file, int line, const void* vparam, int iparam, va_list args) {
  if (logger_load_dll() && __c_logger_log_cmd_args != __c_logger_log_cmd_args_dummy)
    __c_logger_log_cmd_args(logobj, cmd_id, verbose_level, caller_addr, function, file, line, vparam, iparam, args);
}

void LOG_CDECL __c_logger_set_current_thread_name_dummy(void* logobj, const char* thread_name) {
  if (logger_load_dll() && __c_logger_set_current_thread_name != __c_logger_set_current_thread_name_dummy)
    __c_logger_set_current_thread_name(logobj, thread_name);
}

void LOG_CDECL __c_logger_set_config_param_dummy(void* logobj, const char* key, const char* value) {
  if (logger_load_dll() && __c_logger_set_config_param != __c_logger_set_config_param_dummy)
    __c_logger_set_config_param(logobj, key, value);
}

int LOG_CDECL __c_logger_get_config_param_dummy(void* logobj, const char* key, char* value, int buffer_size) {
  if (logger_load_dll() && __c_logger_get_config_param != __c_logger_get_config_param_dummy)
    return __c_logger_get_config_param(logobj, key, value, buffer_size);

  return 0;
}

void LOG_CDECL __c_logger_reload_config_dummy(void* logobj) {
  if (logger_load_dll() && __c_logger_reload_config != __c_logger_reload_config_dummy)
    __c_logger_reload_config(logobj);
}

void LOG_CDECL __c_logger_dump_state_dummy(void* logobj, int verbose_level) {
  if (logger_load_dll() && __c_logger_dump_state != __c_logger_dump_state_dummy)
    __c_logger_dump_state(logobj, verbose_level);
}

int LOG_CDECL __c_logger_register_plugin_factory_dummy(void* logobj, void* factory_interface) {
  if (logger_load_dll() && __c_logger_register_plugin_factory != __c_logger_register_plugin_factory_dummy)
    return __c_logger_register_plugin_factory(logobj, factory_interface);

  return 0;
}

int LOG_CDECL __c_logger_unregister_plugin_factory_dummy(void* logobj, void* factory_interface) {
  if (logger_load_dll() && __c_logger_unregister_plugin_factory != __c_logger_unregister_plugin_factory_dummy)
    return __c_logger_unregister_plugin_factory(logobj, factory_interface);

  return 0;
}

int LOG_CDECL __c_logger_attach_plugin_dummy(void* logobj, void* plugin_interface) {
  if (logger_load_dll() && __c_logger_attach_plugin != __c_logger_attach_plugin_dummy)
    return __c_logger_attach_plugin(logobj, plugin_interface);

  return 0;
}

int LOG_CDECL __c_logger_detach_plugin_dummy(void* logobj, void* plugin_interface) {
  if (logger_load_dll() && __c_logger_detach_plugin != __c_logger_detach_plugin_dummy)
    return __c_logger_detach_plugin(logobj, plugin_interface);

  return 0;
}

void LOG_CDECL __c_logger_flush_dummy(void* logobj) {
  if (logger_load_dll() && __c_logger_flush != __c_logger_flush_dummy)
    __c_logger_flush(logobj);
}

unsigned int LOG_CDECL c_logger_get_version_dummy(void* logobj) {
  if (logger_load_dll() && c_logger_get_version != c_logger_get_version_dummy)
    return c_logger_get_version(logobj);

  return 0;
}

int LOG_CDECL c_logger_is_master_dummy(void* logobj) {
  if (logger_load_dll() && c_logger_is_master != c_logger_is_master_dummy)
    return c_logger_is_master(logobj);

  return 0;
}

void* LOG_CDECL __c_logger_get_logger_dummy() {
  if (logger_load_dll() && __c_logger_get_logger != __c_logger_get_logger_dummy)
    return __c_logger_get_logger();

  return NULL;
}


void (LOG_CDECL *__c_logger_log)(void* logobj, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, ...) = &__c_logger_log_dummy;
void (LOG_CDECL *__c_logger_log_args)(void* logobj, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, va_list args) = &__c_logger_log_args_dummy;
void (LOG_CDECL *__c_logger_log_cmd)(void* logobj, int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam) = &__c_logger_log_cmd_dummy;
void (LOG_CDECL *__c_logger_log_cmd_args)(void* logobj, int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam, va_list args) = &__c_logger_log_cmd_args_dummy;
void (LOG_CDECL *__c_logger_set_current_thread_name)(void* logobj, const char* thread_name) = &__c_logger_set_current_thread_name_dummy;
void (LOG_CDECL *__c_logger_set_config_param)(void* logobj, const char* key, const char* value) = &__c_logger_set_config_param_dummy;
int (LOG_CDECL *__c_logger_get_config_param)(void* logobj, const char* key, char* value, int buffer_size) = &__c_logger_get_config_param_dummy;
void(LOG_CDECL* __c_logger_reload_config)(void* logobj);
void(LOG_CDECL* __c_logger_dump_state)(void* logobj, int verbose_level);
int(LOG_CDECL* __c_logger_register_plugin_factory)(void* logobj, void* factory_interface);
int(LOG_CDECL* __c_logger_unregister_plugin_factory)(void* logobj, void* factory_interface);
int(LOG_CDECL* __c_logger_attach_plugin)(void* logobj, void* plugin_interface);
int(LOG_CDECL* __c_logger_detach_plugin)(void* logobj, void* plugin_interface);
void(LOG_CDECL* __c_logger_flush)(void* logobj);
unsigned int(LOG_CDECL* c_logger_get_version)(void* logobj);
int(LOG_CDECL* c_logger_is_master)(void* logobj);
void* (LOG_CDECL* __c_logger_get_logger)();

void logger_restore_dummies()
{
  __c_logger_get_logger = &__c_logger_get_logger_dummy;
	__c_logger_log = &__c_logger_log_dummy;
	__c_logger_log_args = &__c_logger_log_args_dummy;

  __c_logger_log_cmd = &__c_logger_log_cmd_dummy;
  __c_logger_log_cmd_args = &__c_logger_log_cmd_args_dummy;

  __c_logger_set_current_thread_name = &__c_logger_set_current_thread_name_dummy;
  __c_logger_set_config_param = &__c_logger_set_config_param_dummy;
  __c_logger_get_config_param = &__c_logger_get_config_param_dummy;
  __c_logger_reload_config = &__c_logger_reload_config_dummy;
  __c_logger_dump_state = &__c_logger_dump_state_dummy;
  __c_logger_register_plugin_factory = &__c_logger_register_plugin_factory_dummy;
  __c_logger_unregister_plugin_factory = &__c_logger_unregister_plugin_factory_dummy;
  __c_logger_attach_plugin = &__c_logger_attach_plugin_dummy;
  __c_logger_detach_plugin = &__c_logger_detach_plugin_dummy;
  __c_logger_flush = &__c_logger_flush_dummy;
  c_logger_get_version = &c_logger_get_version_dummy;
  c_logger_is_master = &c_logger_is_master_dummy;

	if (__logger_dll_handle) {
#ifdef LOG_PLATFORM_WINDOWS
		FreeLibrary(__logger_dll_handle);
#else //LOG_PLATFORM_WINDOWS
		dlclose(__logger_dll_handle);
#endif //LOG_PLATFORM_WINDOWS
	}
}

#ifdef LOG_PLATFORM_WINDOWS
#	define LOG_GET_PROC_ADDRESS	GetProcAddress

#  ifndef LOG_DLL_NAME
#	   define LOG_DLL_NAME 		"logger_dll.dll"
#  endif //LOG_DLL_NAME
#else //LOG_PLATFORM_WINDOWS
#	define LOG_GET_PROC_ADDRESS dlsym

# ifndef LOG_DLL_NAME
#  if defined(LOG_PLATFORM_ANDROID)
#    if defined(LOG_PLATFORM_64BIT)
#	   define LOG_DLL_NAME 		"liblogger_dll_arm64-v8a.so"
#    elif defined(LOG_PLATFORM_32BIT)
#	   define LOG_DLL_NAME 		"liblogger_dll_armeabi-v7a.so"
#    endif /*LOG_PLATFORM_XXX*/
#  else //LOG_PLATFORM_ANDROID
#    ifdef LOG_PLATFORM_MAC
#    define LOG_DLL_NAME        "liblogger_dll.dylib"
#  else //LOG_PLATFORM_MAC
#    define LOG_DLL_NAME 		"liblogger_dll.so"
#  endif //LOG_PLATFORM_MAC
#  endif //LOG_PLATFORM_ANDROID
# endif //LOG_DLL_NAME

#endif //LOG_PLATFORM_WINDOWS


int LOG_CDECL logger_is_dll_loaded() {
  return !!__logger_dll_handle;
}

int LOG_CDECL logger_load_dll_functions(void* logger_dll_handle) {
  __c_logger_log_args = (void (LOG_CDECL *)(void* logobj, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, va_list args))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_log_args");

  __c_logger_log = (void (LOG_CDECL *)(void* logobj, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, ...))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_log");

  __c_logger_log_cmd = (void (LOG_CDECL *)(void* logobj, int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_log_cmd");

  __c_logger_log_cmd_args = (void (LOG_CDECL *)(void* logobj, int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam, va_list args))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_log_cmd_args");

  __c_logger_set_current_thread_name = (void(LOG_CDECL*)(void* logobj, const char*))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_set_current_thread_name");

  __c_logger_set_config_param = (void(LOG_CDECL*)(void* logobj, const char*, const char*))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_set_config_param");

  __c_logger_get_config_param = (int(LOG_CDECL*)(void* logobj, const char*, char*, int))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_get_config_param");

  __c_logger_reload_config = (void(LOG_CDECL*)(void* logobj))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_reload_config");

  __c_logger_dump_state = (void(LOG_CDECL*)(void* logobj, int verbose_level))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_dump_state");

  __c_logger_register_plugin_factory = (int(LOG_CDECL*)(void* logobj, void* factory_interface))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_register_plugin_factory");

  __c_logger_unregister_plugin_factory = (int(LOG_CDECL*)(void* logobj, void* factory_interface))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_unregister_plugin_factory");

  __c_logger_attach_plugin = (int(LOG_CDECL*)(void* logobj, void* plugin_interface))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_attach_plugin");

  __c_logger_detach_plugin = (int(LOG_CDECL*)(void* logobj, void* plugin_interface))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_detach_plugin");

  __c_logger_flush = (void(LOG_CDECL*)(void* logobj))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_flush");

  c_logger_get_version = (unsigned int(LOG_CDECL*)(void* logobj))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "c_logger_get_version");

  c_logger_is_master = (int(LOG_CDECL*)(void* logobj))
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "c_logger_is_master");

  __c_logger_get_logger = (void*(LOG_CDECL*)())
    LOG_GET_PROC_ADDRESS(logger_dll_handle, "__c_logger_get_logger");

  return 1;
}

int LOG_CDECL logger_load_dll() {
#if !defined(LOG_PLATFORM_WINDOWS) && !defined(LOG_PLATFORM_ANDROID)
    char libpath[512];
    int pos;
#endif /*!defined(LOG_PLATFORM_WINDOWS) && !defined(LOG_PLATFORM_ANDROID)*/

	if (__logger_dll_handle)
		return 1;

	if (__logger_try_load)
		return 0;
	
	__logger_try_load = 1;

#ifdef LOG_PLATFORM_WINDOWS
	__logger_dll_handle = LoadLibraryA(LOG_DLL_NAME);
#else //LOG_PLATFORM_WINDOWS


#ifdef LOG_PLATFORM_ANDROID
  __logger_dll_handle = dlopen(LOG_DLL_NAME, RTLD_LAZY);
#else //LOG_PLATFORM_ANDROID

#	ifdef LOG_HAVE_UNISTD_H

#ifndef LOG_PLATFORM_MAC
    readlink(LOG_SELF_PROC_LINK, libpath, sizeof(libpath));
#else //LOG_PLATFORM_MAC
    {
      uint32_t size = sizeof(libpath);
      _NSGetExecutablePath(libpath, &size);
    }
#endif //LOG_PLATFORM_MAC

	pos = strlen(libpath);
	while(pos && libpath[pos] != '/')
	{
	    libpath[pos] = 0;
	    pos--;
	}
#	else //LOG_HAVE_UNISTD_H
	strcpy(libpath,"./");
#	endif //LOG_HAVE_UNISTD_H

	strcat(libpath, LOG_DLL_NAME);
	__logger_dll_handle = dlopen(libpath, RTLD_LAZY);

  if (!__logger_dll_handle) {
    __logger_dll_handle = dlopen(LOG_DLL_NAME, RTLD_LAZY);
  }

#endif /*not LOG_PLATFORM_ANDROID*/

#endif /*LOG_PLATFORM_WINDOWS*/

  if (!__logger_dll_handle) {
    return 0;
  }

  logger_load_dll_functions(__logger_dll_handle);

  if (!__c_logger_log_cmd)
    __c_logger_log_cmd = &__c_logger_log_cmd_dummy;

  if (!__c_logger_log_cmd_args)
    __c_logger_log_cmd_args = &__c_logger_log_cmd_args_dummy;

  if (!__c_logger_set_current_thread_name)
    __c_logger_set_current_thread_name = &__c_logger_set_current_thread_name_dummy;

  if (!__c_logger_set_config_param)
    __c_logger_set_config_param = &__c_logger_set_config_param_dummy;

  if (!__c_logger_get_config_param)
    __c_logger_get_config_param = &__c_logger_get_config_param_dummy;

  if (!__c_logger_reload_config)
    __c_logger_reload_config = &__c_logger_reload_config_dummy;

  if (!__c_logger_dump_state)
    __c_logger_dump_state = &__c_logger_dump_state_dummy;

  if (!__c_logger_register_plugin_factory)
    __c_logger_register_plugin_factory = &__c_logger_register_plugin_factory_dummy;

  if (!__c_logger_unregister_plugin_factory)
    __c_logger_unregister_plugin_factory = &__c_logger_unregister_plugin_factory_dummy;

  if (!__c_logger_attach_plugin)
    __c_logger_attach_plugin = &__c_logger_attach_plugin_dummy;

  if (!__c_logger_detach_plugin)
    __c_logger_detach_plugin = &__c_logger_detach_plugin_dummy;
  
  if (!__c_logger_flush)
    __c_logger_flush = &__c_logger_flush_dummy;

  if (!c_logger_get_version)
    c_logger_get_version = &c_logger_get_version_dummy;

  if (!c_logger_is_master)
    c_logger_is_master = &c_logger_is_master_dummy;

  if (!__c_logger_get_logger)
    __c_logger_get_logger = &__c_logger_get_logger_dummy;

  if (!__c_logger_log_args || !__c_logger_log || !__c_logger_log_cmd || !__c_logger_log_cmd_args) {
    logger_restore_dummies();
    return 0;
  }

  return 1;
}

#endif //LOG_USE_DLL && !LOG_THIS_IS_DLL
