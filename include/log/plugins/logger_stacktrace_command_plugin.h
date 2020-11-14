
#ifndef LOGGER_STACKTRACE_COMMAND_PLUGIN
#define LOGGER_STACKTRACE_COMMAND_PLUGIN

#include <log/logger_interfaces.h>
#include <log/detail/logger_strutils.h>
#include <log/detail/logger_runtime_debugging.h>
#include <log/detail/logger_cfgfn.h>

#include <sstream>

#define LOGGER_HAVE_BUILTIN_STACKTRACE_COMMAND_PLUGIN  1

namespace logging {

class logger_stacktrace_command_plugin : public logger_command_plugin_interface {
public:
  const int kStacktraceCommandId = 0x1003;

  logger_stacktrace_command_plugin(const char* name = NULL) : plugin_name_(name ? name : "") {}
  virtual ~logger_stacktrace_command_plugin() LOG_METHOD_OVERRIDE {}

  const char* type() const LOG_METHOD_OVERRIDE { return "stacktrace_cmd"; }

  const char* name() const LOG_METHOD_OVERRIDE { return plugin_name_.c_str(); }

  void get_cmd_ids(int* out_cmd_ids, int max_cmds) const LOG_METHOD_OVERRIDE {
    if (max_cmds > 1) {
      out_cmd_ids[0] = kStacktraceCommandId;
    }
  }

  bool cmd(std::string& out_result, int cmd_id, int verb_level, void* addr, const void* vparam, int iparam) LOG_METHOD_OVERRIDE {
    (void)vparam;
    (void)iparam;

    using namespace detail;
    if (cmd_id != kStacktraceCommandId)
      return false;

    get_current_stack_trace_string(&out_result);
    return true;
  }

  void config_updated(const logging::cfg::KeyValueTypeList& config) {
    config_sym_path_ = detail::cfg::get_logcfg_string(config, "runtime_debugging", std::string(), "SymPath", ".");
  }

private:

#ifdef LOG_PLATFORM_WINDOWS
  LONG WINAPI exception_catch_stack_trace_filter(EXCEPTION_POINTERS* exp,
    DWORD exp_code,
    std::string* stack_trace,
    std::string* sym_path) {
    (void)exp_code;

    std::string config_sym_path = *sym_path;
    *stack_trace = detail::runtime_debugging::get_stack_trace_string(exp->ContextRecord, 2, config_sym_path);
    return EXCEPTION_EXECUTE_HANDLER;
  }

#ifdef LOG_COMPILER_MSVC
#pragma optimize("", off)
#endif  // LOG_COMPILER_MSVC

  void get_current_stack_trace_string(std::string* stack_trace) {
    __try {
      volatile int a = 10;
      volatile int b = 0;
      a = a / b;  // division by zero
    }
    __except (exception_catch_stack_trace_filter(GetExceptionInformation(),
      GetExceptionCode(), stack_trace, &config_sym_path_)) {
    }
  }

#ifdef LOG_COMPILER_MSVC
#pragma optimize("", on)
#endif  // LOG_COMPILER_MSVC

#else   // LOG_PLATFORM_WINDOWS

  void get_current_stack_trace_string(std::string* stack_trace) {
    const int kStackTraceMaxDepth = 1024;

    void* bt[kStackTraceMaxDepth];
    int bt_size = backtrace(bt, kStackTraceMaxDepth);
    *stack_trace = runtime_debugging::get_stack_trace_string(bt, bt_size, 1);
  }
#endif  // LOG_PLATFORM_WINDOWS

  std::string plugin_name_;
  std::string config_sym_path_;
};

class logger_stacktrace_command_plugin_factory : public logger_plugin_default_factory<logger_stacktrace_command_plugin> {
public:
  logger_stacktrace_command_plugin_factory()
    : logger_plugin_default_factory<logger_stacktrace_command_plugin>("stacktrace_cmd", kLogPluginTypeCommand) {}
  virtual ~logger_stacktrace_command_plugin_factory() LOG_METHOD_OVERRIDE {}
};

}//namespace logging

#endif /*LOGGER_STACKTRACE_COMMAND_PLUGIN*/
