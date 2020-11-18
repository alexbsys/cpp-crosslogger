
#ifndef LOGGER_CRASHHANDLER_COMMAND_PLUGIN
#define LOGGER_CRASHHANDLER_COMMAND_PLUGIN

#include <log/logger_config.h>
#include <log/logger_pdetect.h>
#include <log/logger_pdefs.h>
#include <log/logger_sysinclib.h>
#include <log/logger_interfaces.h>
#include <log/logger_userdefs.h>
#include <log/logger_verbose.h>
#include <log/detail/logger_strutils.h>
#include <log/detail/logger_runtime_debugging.h>

#if LOG_SHARED
#include <log/detail/logger_shmem.h>
#endif /*LOG_SHARED*/

namespace logging {

#if LOG_UNHANDLED_EXCEPTIONS
static void init_unhandled_exceptions_handler(logger_interface* logger_obj);
#endif  // LOG_UNHANDLED_EXCEPTIONS

extern logger_singleton_interface<logger_interface>* _logger;


#if defined(LOG_PLATFORM_WINDOWS)
class unhandled_exceptions_processor {
 private:
  /**
   * \brief    Minidump Windows callback implementation
   * \param    param    Unused
   * \param    input    Input request
   * \param    output   Output information
   * \return   TRUE if information is significant, FALSE if not
   */
  static BOOL CALLBACK minidump_callback(PVOID param,
                                         const PMINIDUMP_CALLBACK_INPUT input,
                                         PMINIDUMP_CALLBACK_OUTPUT output) {
    (void)param;

    if (input == 0 || output == 0) return FALSE;

    switch (input->CallbackType) {
      case IncludeModuleCallback:
      case IncludeThreadCallback:
      case ModuleCallback:
      case ThreadCallback:
      case ThreadExCallback:
      case MemoryCallback:
        return TRUE;

      default:
      case CancelCallback:
        return FALSE;
    }
  }

  /**
   * \brief    Create minidump file for Windows
   * \param    ex_ptrs              Information about crash
   * \param    minidump_file_path   Path to minidump file
   */
  static void create_minidump(EXCEPTION_POINTERS* ex_ptrs,
                              const std::string& minidump_file_path) {
    HANDLE file_handle =
        CreateFileA(minidump_file_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file_handle == NULL || file_handle == INVALID_HANDLE_VALUE) {
      LOG_ERROR("Cannot create minidump file: %s, error: %u", minidump_file_path.c_str(),
                GetLastError());
      return;
    }

    MINIDUMP_EXCEPTION_INFORMATION mdei;

    mdei.ThreadId = GetCurrentThreadId();
    mdei.ExceptionPointers = ex_ptrs;
    mdei.ClientPointers = FALSE;

    MINIDUMP_CALLBACK_INFORMATION mci;

    mci.CallbackRoutine =
        (MINIDUMP_CALLBACK_ROUTINE)unhandled_exceptions_processor::minidump_callback;
    mci.CallbackParam = 0;

    MINIDUMP_TYPE mdt =
        (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithHandleData | MiniDumpWithThreadInfo |
                        MiniDumpWithUnloadedModules);

    BOOL rv = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file_handle,
                                mdt, (ex_ptrs != 0) ? &mdei : 0, 0, &mci);

    if (!rv) {
      LOG_ERROR("MiniDumpWriteDump failed. Error: %u", GetLastError());
    } else {
      LOG_INFO("Minidump created: %s", minidump_file_path.c_str());
    }

    CloseHandle(file_handle);
    return;
  }

 public:
//  static void* prev_exception_filter_;

  /**
   * \brief  Crash handler exception filter for Windows
   * \param  ex_ptrs  Exception pointers structure
   * \return EXCEPTION_CONTINUE_SEARCH for continue exception processing or result code
   */
  static long __stdcall crash_handler_exception_filter(EXCEPTION_POINTERS* ex_ptrs) {
    using namespace detail;
    logging::logger_interface* logger_obj = NULL;
    
#if LOG_SHARED
    logger_obj = reinterpret_cast<logging::logger_interface*>(logging::detail::shared_obj::try_find_shared_object(0));
#else /*LOG_SHARED*/
    logger_obj = _logger->get();
#endif

    if (!logger_obj) {
      return EXCEPTION_CONTINUE_SEARCH;
    }

    std::string message = str::stringformat(
        "*** Unhandled Exception!\n"
        "   Exception code: 0x%.8X (%s)\n"
        "   Exception flags: %d\n"
        "   Exception address: 0x%.8X\n",
        ex_ptrs->ExceptionRecord->ExceptionCode,
        runtime_debugging::get_system_expection_code_text(
            ex_ptrs->ExceptionRecord->ExceptionCode),
        ex_ptrs->ExceptionRecord->ExceptionFlags,
        ex_ptrs->ExceptionRecord->ExceptionAddress);

    LOGOBJ_TEXT(logger_obj, logger_verbose_fatal, message.c_str());
    logger_obj->flush();
    
    // Log modules
    LOGOBJ_CMD(logger_obj, 0x1002, logger_verbose_fatal, NULL, 0);
    //    LOG_MODULES_FATAL();

    logger_obj->flush();

    char buf[512];

#if LOG_AUTO_DEBUGGING
    std::string config_sym_path;
    if (logger_obj->get_config_param("runtime_debugging::SymPath", buf, sizeof(buf)))
      config_sym_path = buf;

    LOGOBJ_TEXT(logger_obj, logger_verbose_fatal, "Stack trace:\n%s",
              runtime_debugging::get_stack_trace_string(ex_ptrs->ContextRecord, 0, config_sym_path).c_str());
    logger_obj->flush();
#endif  // LOG_AUTO_DEBUGGING

    int millisec;
    struct tm newtime = utils::get_time(millisec);
    std::string exit_on_crash_str;
    if (logger_obj->get_config_param("crash_dump::ExitAppOnCrash", buf, sizeof(buf)))
      exit_on_crash_str = buf;

    bool exit_on_crash = exit_on_crash_str == "1";

    std::string show_message_on_crash_str;
    if (logger_obj->get_config_param("crash_dump::ShowMessageOnCrash", buf, sizeof(buf)))
      show_message_on_crash_str = buf;

    bool show_message_on_crash = show_message_on_crash_str == "1";

    std::string create_dump_file_str;
    if (logger_obj->get_config_param("crash_dump::CreateDumpFile", buf, sizeof(buf)))
      create_dump_file_str = buf;

    bool create_dump_file = create_dump_file_str != "0";

    std::string path;
    if (logger_obj->get_config_param("crash_dump::DumpPath", buf, sizeof(buf)))
      path = buf;

    if (!path.size())
      path = ".";

    std::string file_name;
    if (logger_obj->get_config_param("crash_dump::DumpName", buf, sizeof(buf)))
      file_name = buf;

    if (!file_name.size())
      file_name = "crashdump";

    std::string create_dir;
    if (logger_obj->get_config_param("crash_dump::CreateDumpDirectory", buf, sizeof(buf)))
      create_dir = buf;
    
    if (create_dump_file) {
      if (create_dir.size() && atoi(create_dir.c_str())) {
        utils::create_directory_path(path);
      }

      std::string dump_name = str::stringformat(
        "%s\\%s__%.4d_%.2d_%.2d__%.2d_%.2d_%.2d.dmp",
        path.c_str(),
        file_name.c_str(),
        newtime.tm_year + 1900,
        newtime.tm_mon + 1,
        newtime.tm_mday,
        newtime.tm_hour,
        newtime.tm_min,
        newtime.tm_sec);

      create_minidump(ex_ptrs, dump_name);
    }

    logger_obj->flush();

    if (exit_on_crash) {
      if (show_message_on_crash) {
        FatalAppExitA(-1, message.c_str());
        return -1;
      }
      else {
        ExitProcess(-1);
      }
    }

    std::string addr_str;
    if (logger_obj->get_config_param("crash_dump::PreviousExceptionFilterAddress", buf, sizeof(buf)))
      addr_str = buf;

    long long addr = atoll(addr_str.c_str());
    uint64_t addr64 = addr < 0 ? (-addr + 0x8000000000000000ULL) : addr;
    void * prev_exception_filter = reinterpret_cast<void*>(addr64);


    if (prev_exception_filter) {
      LPTOP_LEVEL_EXCEPTION_FILTER prev_ex_filter = reinterpret_cast<LPTOP_LEVEL_EXCEPTION_FILTER>(prev_exception_filter);
      return prev_ex_filter(ex_ptrs);
    }

    return EXCEPTION_CONTINUE_SEARCH;
  }
};
#else   // defined(LOG_PLATFORM_WINDOWS)

/**
 * \brief  Crash handler for UNIX-based platforms
 * \param  sig     Signal code
 * \param  info    Signal information structure
 * \param  secret  Pointer to context structure
 */
static void crash_handler(int sig, siginfo_t* info, void* secret) {
  using namespace detail;

  void* trace[1024];
  ucontext_t* uc = (ucontext_t*)secret;
  logging::logger_interface* logger_obj = NULL;

#ifdef LOG_PLATFORM_64BIT
  void* pc = (void*)uc->uc_mcontext.gregs[REG_RIP];
#endif  // LOG_PLATFORM_64BIT

#ifdef LOG_PLATFORM_32BIT
  void* pc = (void*)uc->uc_mcontext.gregs[REG_EIP];
#endif  // LOG_PLATFORM_32BIT

#if LOG_SHARED
  logger_obj = reinterpret_cast<logging::logger_interface*>(logging::detail::shared_obj::try_find_shared_object(0));
#else /*LOG_SHARED*/
  logger_obj = _logger->get();
#endif

  if (!logger_obj) {
    return;
  }

  std::string message = str::stringformat("*** Got signal %d, faulty address %p, from %p", sig,
                                     info->si_addr, pc);

  LOGOBJ_TEXT(logger_obj, logger_verbose_fatal, message.c_str());
  logger_obj->flush();


  // log modules
  LOGOBJ_CMD(logger_obj, 0x1002, logger_verbose_fatal, NULL, 0);
//  LOG_MODULES_FATAL;

  int trace_size = backtrace(trace, 1024);
  trace[1] = pc;

  std::string result =
      "\n" + runtime_debugging::get_stack_trace_string(trace, trace_size, 0);

  LOGOBJ_TEXT(logger_obj, logger_verbose_fatal, "*** STACKTRACE ***");
  LOGOBJ_TEXT(logger_obj, logger_verbose_fatal, result.c_str());
  LOGOBJ_TEXT(logger_obj, logger_verbose_fatal, "*** END STACKTRACE ***");

  logger_obj->flush();

  // dump objects
  LOGOBJ_CMD(logger_obj, 0x1012, logger_verbose_fatal, NULL, 0);
  logger_obj->flush();


#if LOG_RELEASE_ON_APP_CRASH
//  _logger.release();
#endif  // LOG_RELEASE_ON_APP_CRASH

#if LOG_SHOW_MESSAGE_ON_FATAL_CRASH
  printf("%s\n", message.c_str());
#endif  // LOG_SHOW_MESSAGE_ON_FATAL_CRASH

  exit(-1);
}

#endif  // defined(LOG_PLATFORM_WINDOWS)

static void init_unhandled_exceptions_handler(logger_interface* logger_obj) {
#ifdef LOG_PLATFORM_WINDOWS
  using namespace detail;

  void* prev_exception_filter = reinterpret_cast<void*>(SetUnhandledExceptionFilter(
      unhandled_exceptions_processor::crash_handler_exception_filter));

  uint64_t addr = reinterpret_cast<uint64_t>(prev_exception_filter);
  std::string addr_str = str::stringformat(LOG_FMT_U64, addr);

  logger_obj->set_config_param("crash_dump::PreviousExceptionFilterAddress", addr_str.c_str());


#else   // LOG_PLATFORM_WINDOWS
  struct sigaction sa;

  sa.sa_sigaction = crash_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  sigaction(SIGSEGV, &sa, NULL);
#endif  // LOG_PLATFORM_WINDOWS
}


class logger_crashhandler_command_plugin : public logger_command_plugin_interface {
public:
  const int kCrashHandlerInstallCommandId = 0x100A;
  const int kCrashHandlerUninstallCommandId = 0x100B;
  
  // for internal use, user should not call it directly
  const int kCrashExecutePrivateCommandId = 0x100C;

  logger_crashhandler_command_plugin(const char* name = NULL) : plugin_name_(name ? name : ""), logger_(NULL) {}
  virtual ~logger_crashhandler_command_plugin() LOG_METHOD_OVERRIDE {}

  const char* type() const LOG_METHOD_OVERRIDE { return "crashhandler_cmd"; }

  const char* name() const LOG_METHOD_OVERRIDE { return plugin_name_.c_str(); }

  void get_cmd_ids(int* out_cmd_ids, int max_cmds) const LOG_METHOD_OVERRIDE {
    if (max_cmds < 3)
      return;
    out_cmd_ids[0] = kCrashHandlerInstallCommandId;
    out_cmd_ids[1] = kCrashHandlerUninstallCommandId;
    out_cmd_ids[2] = kCrashExecutePrivateCommandId;
  }

  bool cmd(std::string& out_result, int cmd_id, int verb_level, void* addr, const void* vparam, int iparam) LOG_METHOD_OVERRIDE {
    (void)iparam;
    (void)vparam;
    (void)addr;
    (void)verb_level;

    std::stringstream sstream;
    bool result = false;

    if (cmd_id == kCrashHandlerInstallCommandId) {
      result = true;
      init_unhandled_exceptions_handler(logger_);
    }
    else if (cmd_id == kCrashHandlerUninstallCommandId) {
      result = true;
    }
    else if (cmd_id == kCrashExecutePrivateCommandId) {
      result = true;
    }

    if (result)
      out_result = sstream.str();

    return result;
  }

  virtual bool attach(logger_interface* logger) LOG_METHOD_OVERRIDE { logger_ = logger; return true; }
  virtual void detach(logger_interface* logger) LOG_METHOD_OVERRIDE { (void)logger; logger_ = NULL;  }

private:
  std::string plugin_name_;
  logger_interface* logger_;
};

class logger_crashhandler_command_plugin_factory : public logger_plugin_default_factory<logger_crashhandler_command_plugin> {
public:
  logger_crashhandler_command_plugin_factory()
    : logger_plugin_default_factory<logger_crashhandler_command_plugin>("crashhandler_cmd", kLogPluginTypeCommand) {}
  virtual ~logger_crashhandler_command_plugin_factory() LOG_METHOD_OVERRIDE {}
};

}//namespace logging

#endif /*LOGGER_CRASHHANDLER_COMMAND_PLUGIN*/
