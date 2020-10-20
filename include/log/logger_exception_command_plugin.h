
#ifndef LOGGER_EXCEPTION_COMMAND_PLUGIN
#define LOGGER_EXCEPTION_COMMAND_PLUGIN

#include "logger_config.h"
#include "logger_interfaces.h"
#include <sstream>

namespace logging {

class logger_exception_command_plugin : public logger_command_plugin_interface {
public:
  const int kStdExceptionCommandId  = 0x1008;
  const int kExceptionTextCommandId = 0x1009;

  logger_exception_command_plugin(const char* name = NULL) : plugin_name_(name ? name : "") {}
  virtual ~logger_exception_command_plugin() {}

  const char* type() const { return "exception_cmd"; }

  const char* name() const { return plugin_name_.c_str(); }

  void get_cmd_ids(int* out_cmd_ids, int max_cmds) const {
    if (max_cmds < 2)
      return;
    out_cmd_ids[0] = kStdExceptionCommandId;
    out_cmd_ids[1] = kExceptionTextCommandId;
  }

  virtual bool cmd(std::string& out_result, int cmd_id, int verb_level, void* addr, const void* vparam, int iparam) {
    std::stringstream sstream;
    bool result = false;

    if (cmd_id == kExceptionTextCommandId) {
      log_exception_text(sstream, reinterpret_cast<const char*>(vparam));
      result = true;
    }
    else if (cmd_id == kStdExceptionCommandId) {
      log_std_exception(sstream, reinterpret_cast<const std::exception*>(vparam));
      result = true;
    }
      
    if (result)
      out_result = sstream.str();

    return result;
  }

private:
  void log_exception_text(std::stringstream& sstream, const char* user_msg) {
    sstream << "*** Exception occured";
    if (user_msg)
      sstream << ": " << user_msg;
    
    sstream << std::endl;
  }

  void log_std_exception(std::stringstream& sstream, const std::exception* e) {
    std::string message;
    if (e != NULL) {
#if LOG_RTTI_ENABLED
      message += std::string("type: ") + std::string(typeid(*e).name()) + ". ";
#endif  // LOG_RTTI_ENABLED
      if (e->what()) message += std::string(e->what());
    }
    log_exception_text(sstream, message.c_str());
  }

private:
  std::string plugin_name_;
};

class logger_exception_command_plugin_factory : public logger_plugin_default_factory<logger_exception_command_plugin> {
public:
  logger_exception_command_plugin_factory()
    : logger_plugin_default_factory<logger_exception_command_plugin>("exception_cmd", kLogPluginTypeCommand) {}
  virtual ~logger_exception_command_plugin_factory() {}
};

}//namespace logging

#endif /*LOGGER_EXCEPTION_COMMAND_PLUGIN*/
