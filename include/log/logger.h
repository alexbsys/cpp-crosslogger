
// Smart logger library for C/C++
/// alexb 2009-2020

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// LOG MACROSES:
// $(yyyy) $(yy) - year
// $(MM) $(M) - month
// $(dd) $(d) - day
// $(hh) $(h) - hour
// $(mm) $(m) - min
// $(ss) $(s) - sec
// $(ttt) $(t) - milliseconds
// $(V) $(v) - verbose
// $(MODULE) - current module name full
// $(module) - current module name short
// $(function) - function name
// $(line) - line number
// $(srcfile) - source file
// $(PID) - process ID
// $(TID) - thread ID

// CONFIG PATH & NAMES MACROSES:
// $(SYSTEMPATH) - path to Windows\System
// $(WINDOWSPATH) - path to Windows
// $(COMMONAPPDATA) - path to common application data
// $(USERAPPDATA) - path to current user application data
// $(DESKTOPDIR) - path to desktop
// $(TEMPPATH) - path to temp directory
// $(CURRENTDIR) - current process directory
// $(EXEDIR) - path to process EXE
// $(EXEFILENAME) - process EXE file name without extension
// $(EXEFULLFILENAME) - process EXE file name with extension

// only if LOG_USE_MODULEDEFINITION
// $(MODULEDIR) - module directory
// $(MODULEFILENAME) - module file name without extension
// $(MODULEFULLFILENAME) - module file name with extension

// Sample INI file

// [logger]
// LogPath=$(USERAPPDATA)\$(EXEFILENAME)\Log
// Verbose=255
// HeaderFormat=$(V)|$(dd).$(MM).$(yyyy)|$(hh):$(mm):$(ss).$(ttt) ($(module)!$(function))
// LogFileName=$(EXEFILENAME).log
// LogSysInfo=1
// ScrollFileCount=70
// ScrollFileSize=1638400
// ScrollFileEveryRun=1
// RegistryConfigPath=HKCU\Software\$(EXEFILENAME)\Logging

#ifndef __LOGGER_HEADER
#define __LOGGER_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_pdefs.h"

#ifdef LOG_COMPILER_MSVC
#pragma once
#endif /*LOG_COMPILER_MSVC*/

#include "logger_userdefs.h"

#if LOG_ENABLED
#  include "logger_cfgcheck.h"
#  include "logger_get_caller_addr.h"
#  include "logger_verbose.h"

#if defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))
/// logger implementation
#include "logger_interfaces.h"
#include "logger_shared_ptr.hpp"

#include "logger_cfgfn.h"
#include "logger_utils.h"

#include "logger_singleton.h"
#include "logger_impl.h"

namespace logging {
extern logger_singleton_interface<logger_interface>* _logger;
};  // namespace logging

#else  // defined(__cplusplus) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))
#include "logger_c.h"
#endif  // defined(__cplusplus) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))

#endif  // LOG_ENABLED

#endif  //__LOGGER_HEADER
