
#ifndef LOGGER_RUNTIME_DEBUGGING_HEADER
#define LOGGER_RUNTIME_DEBUGGING_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_sysinclib.h"

#include "logger_strutils.h"
#include "logger_utils.h"
#include "logger_moddef.h"

#if defined(LOG_PLATFORM_WINDOWS)

#if defined(LOG_PLATFORM_64BIT)
#define platform_dword_t DWORD64
#elif defined(LOG_PLATFORM_32BIT)
#define platform_dword_t DWORD
#else /*unknown platform*/
#error "Unknown platform"
#endif /*LOG_PLATFORM_32BIT*/

#endif /*LOG_PLATFORM_WINDOWS*/

namespace logging {
namespace detail {

/**
* \class  runtime_debugging helper. Implements stack walking, working with PDB files in runtime
*/
class runtime_debugging {

#if defined(LOG_PLATFORM_WINDOWS)

private:
  static std::string get_pdb_search_path(const std::string& config_sym_path) {
    char pathBuffer[MAX_PATH];
    std::stringstream symSearchPath;

    const char* PdbCurrDir = "\\pdb";
    const int PdbCurrDirLength = 5;  // length of buffer PdbCurrDir include terminate zero char

    if (GetCurrentDirectoryA(sizeof(pathBuffer), pathBuffer))
      symSearchPath << pathBuffer << ";";

    if (strlen(pathBuffer) < sizeof(pathBuffer) - PdbCurrDirLength) {
      strcat(pathBuffer, PdbCurrDir);
      symSearchPath << pathBuffer << ";";
    }

    symSearchPath << utils::get_process_file_path() << ";";
    symSearchPath << utils::get_process_file_path() << PdbCurrDir << ";";

//    std::string config_sym_path = _logger->get_config_param("runtime_debugging::SymPath");
    if (config_sym_path.size()) {
      symSearchPath << config_sym_path << ";";
      symSearchPath << config_sym_path << PdbCurrDir << ";";
    }

    if (strlen(pathBuffer) &&
      strlen(pathBuffer) < sizeof(pathBuffer) - PdbCurrDirLength) {
      strcat(pathBuffer, PdbCurrDir);
    }

    if (GetEnvironmentVariableA("_NT_SYMBOL_PATH", pathBuffer, sizeof(pathBuffer)))
      symSearchPath << pathBuffer << ";";

    if (GetEnvironmentVariableA("_NT_ALTERNATE_SYMBOL_PATH", pathBuffer,
      sizeof(pathBuffer)))
      symSearchPath << pathBuffer << ";";

    if (GetEnvironmentVariableA("SYSTEMROOT", pathBuffer, sizeof(pathBuffer)))
      symSearchPath << pathBuffer << ";";

    std::string result = symSearchPath.str();
    if (result.size() > 0) result = result.substr(0, result.size() - 1);

    return result;
  }

  static const char* get_symbol_type_string(int sym_type) {
    switch (sym_type) {
    case SymNone:
      return "-nosymbols-";
    case SymCoff:
      return "COFF";
    case SymCv:
      return "CV";
    case SymPdb:
      return "PDB";
    case SymExport:
      return "-exported-";
    case SymDeferred:
      return "-deferred-";
    case SymSym:
      return "SYM";
    default:
      return str::stringformat("symtype=%ld", sym_type).c_str();
    }
  }

  static void init_current_process_symbols(const std::string& config_sym_path) {
    static bool isFirstTime = TRUE;
    if (isFirstTime) {
      if (!::SymInitialize(GetCurrentProcess(), get_pdb_search_path(config_sym_path).c_str(), false))
        return;

      unsigned long symOptions = SymGetOptions();
      symOptions |= SYMOPT_LOAD_LINES;
      symOptions &= ~SYMOPT_UNDNAME;
      symOptions &= ~SYMOPT_DEFERRED_LOADS;
      SymSetOptions(symOptions);

      std::list<module_entry_t> modules;
      module_definition::query_module_list(modules);
      load_modules_pdbs(modules);

      isFirstTime = FALSE;
    }
  }

  static void load_modules_pdbs(const std::list<module_entry_t>& modules) {
    for (std::list<module_entry_t>::const_iterator it = modules.begin();
      it != modules.end(); ++it) {
      SymLoadModule(GetCurrentProcess(), 0, it->image_name.c_str(), it->module_name.c_str(),
        it->base_address, it->size);
    }
  }

public:
  static const char* get_system_expection_code_text(unsigned long exceptionCode) {
    switch (exceptionCode) {
    case EXCEPTION_ACCESS_VIOLATION:
      return "ACCESS VIOLATION";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      return "ARRAY BOUNDS EXCEEDED";
    case EXCEPTION_BREAKPOINT:
      return "BREAKPOINT";
    case EXCEPTION_DATATYPE_MISALIGNMENT:
      return "DATATYPE MISALIGNMENT";
    case EXCEPTION_FLT_DENORMAL_OPERAND:
      return "FLT DENORMAL OPERAND";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
      return "FLT DIVIDE BY ZERO";
    case EXCEPTION_FLT_INEXACT_RESULT:
      return "FLT INEXACT RESULT";
    case EXCEPTION_FLT_INVALID_OPERATION:
      return "FLT INVALID OPERATION";
    case EXCEPTION_FLT_OVERFLOW:
      return "FLT OVERFLOW";
    case EXCEPTION_FLT_STACK_CHECK:
      return "FLT STACK CHECK";
    case EXCEPTION_FLT_UNDERFLOW:
      return "FLT UNDERFLOW";
    case EXCEPTION_ILLEGAL_INSTRUCTION:
      return "ILLEGAL INSTRUCTION";
    case EXCEPTION_IN_PAGE_ERROR:
      return "IN PAGE ERROR";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
      return "INT DIVIDE BY ZERO";
    case EXCEPTION_INT_OVERFLOW:
      return "INT OVERFLOW";
    case EXCEPTION_INVALID_DISPOSITION:
      return "INVALID DISPOSITION";
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      return "NONCONTINUABLE EXCEPTION";
    case EXCEPTION_PRIV_INSTRUCTION:
      return "PRIV INSTRUCTION";
    case EXCEPTION_SINGLE_STEP:
      return "SINGLE STEP";
    case EXCEPTION_STACK_OVERFLOW:
      return "STACK OVERFLOW";
    case DBG_CONTROL_C:
      return "DBG CONTROL C ";
    default:
      return "<unkown exception>";
    }
  }

  struct stack_frame_t {
    std::string undecorated_sym_name;
    std::string undecorated_full_sym_name;

    DWORD offset_from_line_bytes;

    platform_dword_t offset_from_symbol_bytes;
    platform_dword_t program_counter;
    platform_dword_t module_base;

    const char* sym_type;

    std::string module_file_name;
    std::string module_name;

    std::string src_file_name;
    int src_file_line;

    stack_frame_t()
      : offset_from_symbol_bytes(0),
      sym_type(NULL),
      src_file_line(0),
      offset_from_line_bytes(0),
      program_counter(0),
      module_base(0) {}
  };

  static std::list<stack_frame_t> get_stack_frames(HANDLE threadHandle, CONTEXT& c, const std::string& config_sym_path) {
    static CRITICAL_SECTION cs;
    static bool cs_initialized = false;

    if (!cs_initialized) {
      InitializeCriticalSection(&cs);
      cs_initialized = true;
    }

    EnterCriticalSection(&cs);

#ifdef _AMD64_
    const intptr_t debug_failed_rip = 0xCCCCCCCCCCCCCCCC;
    DWORD image_type = IMAGE_FILE_MACHINE_AMD64;
#else   //_AMD64_
    const intptr_t debug_failed_rip = 0xCCCCCCCC;
    DWORD image_type = IMAGE_FILE_MACHINE_I386;
#endif  //_AMD64_
    HANDLE process_handle = GetCurrentProcess();

    std::list<stack_frame_t> frames;

    static IMAGEHLP_SYMBOL* sym_ptr = NULL;
    if (sym_ptr == NULL) {
      sym_ptr = (IMAGEHLP_SYMBOL*)malloc(MAX_PATH + sizeof(IMAGEHLP_SYMBOL));
      if (!sym_ptr) {
        LeaveCriticalSection(&cs);
        return frames;
      }
    }

    init_current_process_symbols(config_sym_path);

#ifndef _AMD64_
    STACKFRAME stack_frame;
#else   //_AMD64_
    STACKFRAME64 stack_frame;
#endif  //_AMD64_

    memset(&stack_frame, 0, sizeof(stack_frame));

#ifndef _AMD64_
    stack_frame.AddrPC.Offset = c.Eip;
    stack_frame.AddrFrame.Offset = c.Ebp;
    stack_frame.AddrStack.Offset = c.Esp;
#else   //_AMD64_
    stack_frame.AddrPC.Offset = c.Rip;
    stack_frame.AddrFrame.Offset = c.Rbp;
    stack_frame.AddrStack.Offset = c.Rsp;
#endif  //_AMD64_
    stack_frame.AddrPC.Mode = AddrModeFlat;
    stack_frame.AddrFrame.Mode = AddrModeFlat;
    stack_frame.AddrStack.Mode = AddrModeFlat;

    memset(sym_ptr, 0, MAX_PATH + sizeof(IMAGEHLP_SYMBOL));
    sym_ptr->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
    sym_ptr->MaxNameLength = MAX_PATH;

    IMAGEHLP_LINE line_info;
    memset(&line_info, 0, sizeof line_info);
    line_info.SizeOfStruct = sizeof(line_info);

    IMAGEHLP_MODULE module_info;
    memset(&module_info, 0, sizeof(module_info));
    module_info.SizeOfStruct = sizeof(module_info);

    for (int walkedStackFrames = 0;; ++walkedStackFrames) {
      // WARNING: in some cases we need to provide NULL as context when running in x86
      // context. But in most cases it does not working, try to provide context 			in all
      //cases

      if (!::StackWalk(image_type, process_handle, threadHandle, &stack_frame,
        /*image_type == IMAGE_FILE_MACHINE_I386 ? NULL :*/ &c, NULL,
        &::SymFunctionTableAccess, &::SymGetModuleBase, NULL)) {
        break;
      }

      stack_frame_t frame;
      frame.sym_type = get_symbol_type_string(SymNone);

      if (stack_frame.AddrPC.Offset != 0 &&
        stack_frame.AddrPC.Offset != debug_failed_rip) {
        frame.program_counter = stack_frame.AddrPC.Offset;

        if (::SymGetSymFromAddr(process_handle, stack_frame.AddrPC.Offset,
          &frame.offset_from_symbol_bytes, sym_ptr)) {
          char undName[MAX_PATH] = { 0 };
          char undFullName[MAX_PATH] = { 0 };

          UnDecorateSymbolName(sym_ptr->Name, undName, MAX_PATH, UNDNAME_NAME_ONLY);
          UnDecorateSymbolName(sym_ptr->Name, undFullName, MAX_PATH, UNDNAME_COMPLETE);

          frame.undecorated_sym_name = strlen(undName) > 0 ? undName : sym_ptr->Name;
          frame.undecorated_full_sym_name = undFullName;
        }

        if (::SymGetLineFromAddr(process_handle, stack_frame.AddrPC.Offset,
          &frame.offset_from_line_bytes, &line_info)) {
          frame.src_file_name = line_info.FileName;
          frame.src_file_line = line_info.LineNumber;
        }

        if (::SymGetModuleInfo(process_handle, stack_frame.AddrPC.Offset, &module_info)) {
          frame.module_name = module_info.ModuleName;
          frame.module_base = module_info.BaseOfImage;
          frame.sym_type = get_symbol_type_string(module_info.SymType);
          frame.module_file_name = module_info.LoadedImageName;
        }
      }
      frames.push_back(frame);
    }

    LeaveCriticalSection(&cs);
    return frames;
  }

  static std::string get_stack_trace_string(HANDLE threadHandle, CONTEXT& c,
    int ignoreFunctions = 0, const std::string& config_sym_path = std::string()) {
    std::stringstream sstream;
    std::list<stack_frame_t> stackFrames = get_stack_frames(threadHandle, c, config_sym_path);

    int n = 0;

    for (std::list<stack_frame_t>::iterator i = stackFrames.begin();
      i != stackFrames.end(); i++, n++) {
      if (ignoreFunctions-- > 0) continue;

      stack_frame_t& frame = *i;

      sstream << "[" << n << "] "
        << "<--";
      if (frame.undecorated_sym_name.size())
        sstream << str::stringformat(" %s +%ld bytes ", frame.undecorated_sym_name.c_str(),
          frame.offset_from_symbol_bytes);

      if (frame.undecorated_full_sym_name.size())
        sstream << str::stringformat("[FUNCTION: %s] ",
          frame.undecorated_full_sym_name.c_str());

      if (frame.src_file_name.size() || frame.src_file_line)
        sstream << str::stringformat("[Line: %s(%lu) +%ld bytes] ", frame.src_file_name.c_str(),
          frame.src_file_line, frame.offset_from_line_bytes);

      if (frame.module_name.size()) {
#ifdef _AMD64_
        sstream << stringformat("[Mod:  %s, base: %.8X%.8X] ", frame.module_name.c_str(),
          (uint32_t)(frame.module_base >> 32),
          (uint32_t)frame.module_base);
#else   //_AMD64_
        sstream << str::stringformat("[Mod:  %s, base: %08lxh] ", frame.module_name.c_str(),
          frame.module_base);
#endif  //_AMD64_
      }

#if LOG_STACKTRACE_DETECT_MODNAME_IF_NOT_FOUND
      if (!frame.module_name.size() && frame.program_counter) {
        std::string mod_detected_name = module_definition::module_name_by_addr(
          reinterpret_cast<void*>(frame.program_counter));
        if (mod_detected_name.size())
          sstream << str::stringformat("[DETECTED Mod:  %s] ", mod_detected_name.c_str());
      }
#endif  // LOG_STACKTRACE_DETECT_MODNAME_IF_NOT_FOUND

#ifdef _AMD64_
      sstream << str::stringformat("\n Offset: 0x%.8X%.8X",
        (uint32_t)(frame.program_counter >> 32),
        (uint32_t)frame.program_counter);
#else   //_AMD64_
      sstream << str::stringformat("\n Offset: 0x%.8X", frame.program_counter);
#endif  //_AMD64_

      sstream << str::stringformat(" Sym:  type: %s, file: %s\n", frame.sym_type,
        frame.module_file_name.c_str());
    }
    return sstream.str();
  }

  static std::string get_stack_trace_string(CONTEXT* c, int ignoreFunctions = 0, const std::string& config_sym_path = std::string()) {
    return get_stack_trace_string(NULL, *c, ignoreFunctions, config_sym_path);
  }

#else  // defined(LOG_PLATFORM_WINDOWS)

public:
  static std::string get_stack_trace_string(void* trace[], int trace_len,
    int ignore_functions = 0) {
    std::string result;
    char** bt_syms;
    int i;

    size_t funcnamesize = 256;
    char* funcname = (char*)malloc(funcnamesize);

    bt_syms = backtrace_symbols(trace, trace_len);

    int index = 1;

    for (i = 1 + ignore_functions; i < trace_len; i++) {
      std::string cur_full_sym = bt_syms[i];
      char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

      result += stringformat(" [%d] <-- (%p) ", index++, trace[i]);

      // find parentheses and +address offset surrounding the mangled name:
      // ./module(function+0x15c) [0x8048a6d]
      for (char* p = bt_syms[i]; *p; ++p) {
        if (*p == '(')
          begin_name = p;
        else if (*p == '+')
          begin_offset = p;
        else if (*p == ')' && begin_offset) {
          end_offset = p;
          break;
        }
      }

      if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
        *begin_name++ = '\0';
        *begin_offset++ = '\0';
        *end_offset = '\0';

        int status;
        char* ret = abi::__cxa_demangle(begin_name, funcname, &funcnamesize, &status);
        if (status == 0) {
          funcname = ret;  // use possibly realloc()-ed string
          result += stringformat(" %s: %s+%s\n", bt_syms[i], funcname, begin_offset);
        }
        else {
          // demangling failed. Output function name as a C function with
          // no arguments.
          result += stringformat(" %s: %s()+%s\n", bt_syms[i], begin_name, begin_offset);
        }
      }
      else {
        // couldn't parse the line? print the whole line.
        result += stringformat(" %s\n", bt_syms[i]);
      }

      result += "    ";
      result += cur_full_sym;
      result += "\n";
    }

    free(bt_syms);
    free(funcname);

    return result;
  }

#endif  // defined(LOG_PLATFORM_WINDOWS)
};





}//namespace detail
}//namespace logging


#endif /*LOGGER_RUNTIME_DEBUGGING_HEADER*/
