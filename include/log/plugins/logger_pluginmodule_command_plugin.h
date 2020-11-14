
#ifndef LOGGER_PLUGINMODULE_COMMAND_PLUGIN_HEADER
#define LOGGER_PLUGINMODULE_COMMAND_PLUGIN_HEADER

#include "logger_config.h"
#include "logger_interfaces.h"
#include <sstream>

namespace logging {

class logger_pluginmodule_command_plugin : public logger_command_plugin_interface {
public:
  const int kLoadPluginModuleCommandId  = 0x1020;
  const int kAutoLoadPluginModulesCommandId = 0x1021;

  logger_pluginmodule_command_plugin(const char* name = NULL) : plugin_name_(name ? name : "") {}
  virtual ~logger_pluginmodule_command_plugin() LOG_METHOD_OVERRIDE {}

  const char* type() const LOG_METHOD_OVERRIDE { return "pluginmodule_cmd"; }

  const char* name() const LOG_METHOD_OVERRIDE { return plugin_name_.c_str(); }

  void get_cmd_ids(int* out_cmd_ids, int max_cmds) const LOG_METHOD_OVERRIDE {
    if (max_cmds < 2)
      return;
    out_cmd_ids[0] = kLoadPluginModuleCommandId;
    out_cmd_ids[1] = kAutoLoadPluginModulesCommandId;
  }

  bool cmd(std::string& out_result, int cmd_id, int verb_level, void* addr, const void* vparam, int iparam) LOG_METHOD_OVERRIDE {
    std::stringstream sstream;
    bool result = false;

    if (cmd_id == kLoadPluginModuleCommandId) {
      result = true;
    }
    else if (cmd_id == kAutoLoadPluginModulesCommandId) {
      result = true;
    }
      
    if (result)
      out_result = sstream.str();

    return result;
  }

  void config_updated(const logging::cfg::KeyValueTypeList& config) LOG_METHOD_OVERRIDE {
  
  }


private:
  std::string plugin_name_;
};

class logger_pluginmodule_command_plugin_factory : public logger_plugin_default_factory<logger_pluginmodule_command_plugin> {
public:
  logger_pluginmodule_command_plugin_factory()
    : logger_plugin_default_factory<logger_pluginmodule_command_plugin>("pluginmodule_cmd", kLogPluginTypeCommand) {}
  virtual ~logger_pluginmodule_command_plugin_factory() LOG_METHOD_OVERRIDE {}
};

}//namespace logging

#endif /*LOGGER_PLUGINMODULE_COMMAND_PLUGIN_HEADER*/
