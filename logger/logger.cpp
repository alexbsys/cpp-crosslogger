// logger.cpp
// Smart logger library for C/C++
// Alex V. Bobryshev 2009-2015

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, 
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.

// You need to add this file only if you need to use C tail of library
// DO NOT ADD BOTH LOGGER.CPP AND LOGGERDLL.C

#include "logger.h"

#if !LOG_USE_DLL || defined(LOG_THIS_IS_DLL)

DEFINE_LOGGER;

///////   C tail  ///////

#ifndef LOG_PLATFORM_WINDOWS
extern "C"
void __cxa_pure_virtual() {}
#endif //LOG_PLATFORM_WINDOWS


#if LOG_ENABLED

extern "C"
void __c_logger_log_args(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, va_list args)
{
	logging::_logger->log_args(verbose_level, caller_addr, function, file, line, format, args);
}

extern "C"
void __c_logger_log(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	logging::_logger->log_args(verbose_level, caller_addr, function, file, line, format, arguments);

	va_end(arguments);
}

extern "C"
void __c_logger_log_binary(int verbose_level, void* caller_addr, const char* function, const char* file, int line, const char* data, int len)
{
    logging::_logger->log_binary(verbose_level, caller_addr, function, file, line, data, len);
}

extern "C"
void __c_logger_log_modules(int verbose_level, void* caller_addr, const char* function, const char* file, int line)
{
#if LOG_USE_MODULEDEFINITION
    logging::_logger->log_modules(verbose_level, caller_addr, function, file, line);
#endif //LOG_USE_MODULEDEFINITION
}

extern "C"
void __c_logger_log_stack_trace(int verbose_level, void* caller_addr, const char* function, const char* file, int line)
{
#if LOG_AUTO_DEBUGGING
    logging::_logger->log_stack_trace(verbose_level, caller_addr, function, file, line);
#endif //LOG_AUTO_DEBUGGING
}
//
#endif //LOG_ENABLED

#endif // !LOG_USE_DLL || defined(LOG_THIS_IS_DLL)
