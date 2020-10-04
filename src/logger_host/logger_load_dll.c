
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
/*
void LOG_CDECL __c_logger_log_modules_dummy(int verbose_level, void* caller_addr, const char* function, const char* file, int line) 
{ 
	if (logger_load_dll())
		__c_logger_log_modules(verbose_level, caller_addr, function, file, line);
}

void LOG_CDECL __c_logger_log_stack_trace_dummy(int verbose_level, void* caller_addr, const char* function, const char* file, int line) 
{ 
	if (logger_load_dll()) 
		__c_logger_log_stack_trace(verbose_level, caller_addr, function, file, line);
}
*/
void LOG_CDECL __c_logger_log_args_dummy(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, va_list args) 
{
	if (logger_load_dll())
		__c_logger_log_args(verbose_level, caller_addr, function, file, line, format, args);
}

void LOG_CDECL __c_logger_log_dummy(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, ...) 
{
	va_list arguments;
	va_start(arguments, format);

	if (logger_load_dll())
		__c_logger_log_args(verbose_level, caller_addr, function, file, line, format, arguments);

	va_end(arguments);
}

void LOG_CDECL __c_logger_log_cmd_dummy(int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam) 
{ 
	if (logger_load_dll() && __c_logger_log_cmd != __c_logger_log_cmd_dummy)
		__c_logger_log_cmd(cmd_id, verbose_level, caller_addr, function, file, line, vparam, iparam);
}

void LOG_CDECL __c_logger_log_cmd_args_dummy(int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam, va_list args)
{
  if (logger_load_dll() && __c_logger_log_cmd_args != __c_logger_log_cmd_args_dummy)
    __c_logger_log_cmd_args(cmd_id, verbose_level, caller_addr, function, file, line, vparam, iparam, args);
}

void LOG_CDECL __c_logger_objmon_register_dummy(size_t hash_code, const char* type_name, void* ptr) {
  if (logger_load_dll() && __c_logger_objmon_register != __c_logger_objmon_register_dummy)
    __c_logger_objmon_register(hash_code, type_name, ptr);
}

void LOG_CDECL __c_logger_objmon_unregister_dummy(size_t hash_code, void* ptr) {
  if (logger_load_dll() && __c_logger_objmon_unregister != __c_logger_objmon_unregister_dummy)
    __c_logger_objmon_unregister(hash_code, ptr);
}

void LOG_CDECL __c_logger_objmon_dump_dummy(int verbose_level) {
  if (logger_load_dll() && __c_logger_objmon_dump != __c_logger_objmon_dump_dummy)
    __c_logger_objmon_dump(verbose_level);
}

void LOG_CDECL __c_logger_set_current_thread_name_dummy(const char* thread_name) {
  if (logger_load_dll() && __c_logger_set_current_thread_name != __c_logger_set_current_thread_name_dummy)
    __c_logger_set_current_thread_name(thread_name);
}

void LOG_CDECL __c_logger_set_config_param_dummy(const char* key, const char* value) {
  if (logger_load_dll() && __c_logger_set_config_param != __c_logger_set_config_param_dummy)
    __c_logger_set_config_param(key, value);
}

int LOG_CDECL __c_logger_get_config_param_dummy(const char* key, char* value, int buffer_size) {
  if (logger_load_dll() && __c_logger_get_config_param != __c_logger_get_config_param_dummy)
    return __c_logger_get_config_param(key, value, buffer_size);

  return 0;
}


//void (LOG_CDECL *__c_logger_log_modules)(int verbose_level, void* caller_addr, const char* function, const char* file, int line) = &__c_logger_log_modules_dummy;
//void (LOG_CDECL * __c_logger_log_stack_trace)(int verbose_level, void* caller_addr, const char* function, const char* file, int line) = &__c_logger_log_stack_trace_dummy;
void (LOG_CDECL *__c_logger_log)(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, ...) = &__c_logger_log_dummy;
void (LOG_CDECL *__c_logger_log_args)(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, va_list args) = &__c_logger_log_args_dummy;
//void (LOG_CDECL *__c_logger_log_binary)(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* data, int len) = &__c_logger_log_binary_dummy;
void (LOG_CDECL *__c_logger_log_cmd)(int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam) = &__c_logger_log_cmd_dummy;
void (LOG_CDECL *__c_logger_log_cmd_args)(int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam, va_list args) = &__c_logger_log_cmd_args_dummy;

void (LOG_CDECL *__c_logger_objmon_register)(size_t hash_code, const char* type_name, void* ptr) = &__c_logger_objmon_register_dummy;
void (LOG_CDECL *__c_logger_objmon_unregister)(size_t hash_code, void* ptr) = &__c_logger_objmon_unregister_dummy;
void (LOG_CDECL *__c_logger_objmon_dump)(int verbose_level) = &__c_logger_objmon_dump_dummy;

void (LOG_CDECL *__c_logger_set_current_thread_name)(const char* thread_name) = &__c_logger_set_current_thread_name_dummy;

void (LOG_CDECL *__c_logger_set_config_param)(const char* key, const char* value) = &__c_logger_set_config_param_dummy;
int (LOG_CDECL *__c_logger_get_config_param)(const char* key, char* value, int buffer_size) = &__c_logger_get_config_param_dummy;


void logger_restore_dummies()
{
//	__c_logger_log_modules = &__c_logger_log_modules_dummy;
//	__c_logger_log_stack_trace = &__c_logger_log_stack_trace_dummy;
	__c_logger_log = &__c_logger_log_dummy;
//	__c_logger_log_binary = &__c_logger_log_binary_dummy;
	__c_logger_log_args = &__c_logger_log_args_dummy;

  __c_logger_log_cmd = &__c_logger_log_cmd_dummy;
  __c_logger_log_cmd_args = &__c_logger_log_cmd_args_dummy;

  __c_logger_objmon_register = &__c_logger_objmon_register_dummy;
  __c_logger_objmon_unregister = &__c_logger_objmon_unregister_dummy;
  __c_logger_objmon_dump = &__c_logger_objmon_dump_dummy;

  __c_logger_set_current_thread_name = &__c_logger_set_current_thread_name_dummy;
  __c_logger_set_config_param = &__c_logger_set_config_param_dummy;
  __c_logger_get_config_param = &__c_logger_get_config_param_dummy;

//  __c_logger_set_verbose_level = &__c_logger_set_verbose_level_dummy;

	if (__logger_dll_handle)
	{
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
#  if LOG_PLATFORM_ANDROID
#	   define LOG_DLL_NAME 		"liblogger_dll_armeabi-v7a.so"
#  else //LOG_PLATFORM_ANDROID
#    ifdef LOG_PLATFORM_MAC
#    define LOG_DLL_NAME        "liblogger_dll.dylib"
#  else //LOG_PLATFORM_MAC
#    define LOG_DLL_NAME 		"liblogger_dll.so"
#  endif //LOG_PLATFORM_MAC
#  endif //LOG_PLATFORM_ANDROID
# endif //LOG_DLL_NAME

#endif //LOG_PLATFORM_WINDOWS


int LOG_CDECL logger_load_dll()
{
#ifndef LOG_PLATFORM_WINDOWS
    char libpath[512];
    int pos;
#endif //LOG_PLATFORM_WINDOWS

	if (__logger_dll_handle)
		return 1;

	if (__logger_try_load)
		return 0;
	
	__logger_try_load = 1;

#ifdef LOG_PLATFORM_WINDOWS
	__logger_dll_handle = LoadLibraryA(LOG_DLL_NAME);
#else //LOG_PLATFORM_WINDOWS


#if LOG_PLATFORM_ANDROID
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

#endif //not LOG_PLATFORM_ANDROID

#endif //LOG_PLATFORM_WINDOWS

  if (!__logger_dll_handle)
    return 0;

  __c_logger_log_args = (void (LOG_CDECL *)(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, va_list args))
    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_log_args");
	
  __c_logger_log = (void (LOG_CDECL *)(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, ...))
    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_log");

//  __c_logger_log_binary = (void (LOG_CDECL *)(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* data, int len))
//    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_log_binary");

  __c_logger_log_cmd = (void (LOG_CDECL *)(int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam))
    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_log_cmd");

  __c_logger_log_cmd_args = (void (LOG_CDECL *)(int cmd_id, int verbose_level, void* caller_addr, const char* function, const char* file, int line, const void* vparam, int iparam, va_list args))
    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_log_cmd_args");

//  __c_logger_log_modules = (void (LOG_CDECL *)(int verbose_level, void* caller_addr, const char* function, const char* file, int line))
//    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_log_modules");

//  __c_logger_log_stack_trace = (void (LOG_CDECL*)(int verbose_level, void* caller_addr, const char* function, const char* file, int line))
//    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_log_stack_trace");

  __c_logger_objmon_register = (void (LOG_CDECL*)(size_t, const char*, void*))
    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_objmon_register");

  __c_logger_objmon_unregister = (void (LOG_CDECL*)(size_t, void*))
    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_objmon_unregister");

  __c_logger_objmon_dump = (void (LOG_CDECL*)(int))
    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_objmon_dump");

  __c_logger_set_current_thread_name = (void(LOG_CDECL*)(const char*))
    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_set_current_thread_name");

  __c_logger_set_config_param = (void(LOG_CDECL*)(const char*, const char*))
    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_set_config_param");

  __c_logger_get_config_param = (int(LOG_CDECL*)(const char*, char*, int))
    LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_get_config_param");

  if (!__c_logger_objmon_register)
    __c_logger_objmon_register = &__c_logger_objmon_register_dummy;

  if (!__c_logger_objmon_unregister)
    __c_logger_objmon_unregister = &__c_logger_objmon_unregister_dummy;

  if (!__c_logger_objmon_dump)
    __c_logger_objmon_dump = &__c_logger_objmon_dump_dummy;

  if (!__c_logger_log_cmd)
    __c_logger_log_cmd = &__c_logger_log_cmd_dummy;

  if (!__c_logger_log_cmd_args)
    __c_logger_log_cmd_args = &__c_logger_log_cmd_args_dummy;


//  if (!__c_logger_log_modules)
//    __c_logger_log_modules = &__c_logger_log_modules_dummy;

//  if (!__c_logger_log_stack_trace)
//    __c_logger_log_stack_trace = &__c_logger_log_stack_trace_dummy;

  if (!__c_logger_set_current_thread_name)
    __c_logger_set_current_thread_name = &__c_logger_set_current_thread_name_dummy;

  if (!__c_logger_set_config_param)
    __c_logger_set_config_param = &__c_logger_set_config_param_dummy;

  if (!__c_logger_get_config_param)
    __c_logger_get_config_param = &__c_logger_get_config_param_dummy;

  if (!__c_logger_log_args || !__c_logger_log || !__c_logger_log_cmd || !__c_logger_log_cmd_args 
      || !__c_logger_objmon_register || !__c_logger_objmon_unregister || !__c_logger_objmon_dump)
	{
		logger_restore_dummies();
		return 0;
	}

	return 1;
}

#endif //LOG_USE_DLL && !LOG_THIS_IS_DLL

