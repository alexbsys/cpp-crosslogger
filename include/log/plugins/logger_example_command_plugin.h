
#ifndef LOGGER_EXAMPLE_COMMAND_PLUGIN
#define LOGGER_EXAMPLE_COMMAND_PLUGIN

#include <log/logger_interfaces.h>
#include <log/detail/logger_strutils.h>
#include <sstream>

namespace logging {

class logger_example_command_plugin : public logger_command_plugin_interface {
public:
  const int kExampleCommandId = 0x6020;

  logger_example_command_plugin(const char* name = NULL) : plugin_name_(name ? name : "") {}
  virtual ~logger_example_command_plugin() {}

  const char* type() const LOG_METHOD_OVERRIDE { return "example_cmd"; }

  const char* name() const LOG_METHOD_OVERRIDE { return plugin_name_.c_str(); }

  void get_cmd_ids(int* out_cmd_ids, int max_cmds) const LOG_METHOD_OVERRIDE {
    if (max_cmds > 1) {
      out_cmd_ids[0] = kExampleCommandId;
    }
  }

  bool cmd(std::string& out_result, int cmd_id, int verb_level, void* addr, const void* vparam, int iparam) LOG_METHOD_OVERRIDE {
    (void)addr;
    (void)verb_level;

    if (cmd_id != kExampleCommandId)
      return false;

    /* Place here some action. out_result is result string, user may set something to it */
    out_result = "Hello World";
    return true;
  }

private:
  std::string plugin_name_;
};

class logger_example_command_plugin_factory : public logger_plugin_default_factory<logger_example_command_plugin> {
public:
  logger_example_command_plugin_factory()
    : logger_plugin_default_factory<logger_binary_command_plugin>("example_cmd", kLogPluginTypeCommand) {}
  virtual ~logger_example_command_plugin_factory() LOG_METHOD_OVERRIDE {}
};

}//namespace logging

#endif /*LOGGER_EXAMPLE_COMMAND_PLUGIN*/
