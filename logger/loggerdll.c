
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, 
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.

// Add this file to project if you use logger in separated DLL
// DO NOT ADD BOTH LOGGER.CPP AND LOGGERDLL.C

#include "../logger/logger.h"
#include <stdlib.h>
#include <string.h>

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

void* __logger_dll_handle = NULL;
int __logger_try_load = 0;

int LOG_CDECL logger_load_dll();

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

void LOG_CDECL __c_logger_log_binary_dummy(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* data, int len) 
{ 
	if (logger_load_dll())
		__c_logger_log_binary(verbose_level, caller_addr, function, file, line, data, len);
}

void (LOG_CDECL *__c_logger_log_modules)(int verbose_level, void* caller_addr, const char* function, const char* file, int line) = &__c_logger_log_modules_dummy;
void (LOG_CDECL * __c_logger_log_stack_trace)(int verbose_level, void* caller_addr, const char* function, const char* file, int line) = &__c_logger_log_stack_trace_dummy;
void (LOG_CDECL *__c_logger_log)(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, ...) = &__c_logger_log_dummy;
void (LOG_CDECL *__c_logger_log_args)(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, va_list args) = &__c_logger_log_args_dummy;
void (LOG_CDECL *__c_logger_log_binary)(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* data, int len) = &__c_logger_log_binary_dummy;

void logger_restore_dummies()
{
	__c_logger_log_modules = &__c_logger_log_modules_dummy;
	__c_logger_log_stack_trace = &__c_logger_log_stack_trace_dummy;
	__c_logger_log = &__c_logger_log_dummy;
	__c_logger_log_binary = &__c_logger_log_binary_dummy;
	__c_logger_log_args = &__c_logger_log_args_dummy;

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
#	define LOG_DLL_NAME 		"loggerdll.dll"
#else //LOG_PLATFORM_WINDOWS
#	define LOG_GET_PROC_ADDRESS dlsym
#	define LOG_DLL_NAME 		"libloggerdll.so"
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

#	ifdef LOG_HAVE_UNISTD_H
	readlink(LOG_SELF_PROC_LINK, libpath, sizeof(libpath));

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
#endif //LOG_PLATFORM_WINDOWS

	if (!__logger_dll_handle)
		return 0;

	__c_logger_log_args = (void (LOG_CDECL *)(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, va_list args))
		LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_log_args");
	
	__c_logger_log = (void (LOG_CDECL *)(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, ...))
		LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_log");

	__c_logger_log_binary = (void (LOG_CDECL *)(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* data, int len))
		LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_log_binary");

	__c_logger_log_modules = (void (LOG_CDECL *)(int verbose_level, void* caller_addr, const char* function, const char* file, int line))
		LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_log_modules");

	__c_logger_log_stack_trace = (void (LOG_CDECL*)(int verbose_level, void* caller_addr, const char* function, const char* file, int line))
		LOG_GET_PROC_ADDRESS(__logger_dll_handle, "__c_logger_log_stack_trace");

	if (!__c_logger_log_args || !__c_logger_log || !__c_logger_log_binary || !__c_logger_log_modules || !__c_logger_log_stack_trace)
	{
		logger_restore_dummies();
		return 0;
	}

	return 1;
}

#endif //LOG_USE_DLL && !LOG_THIS_IS_DLL

