
#ifndef LOGGER_BINARY_COMMAND_PLUGIN
#define LOGGER_BINARY_COMMAND_PLUGIN

#include <log/logger_interfaces.h>
#include <log/detail/logger_strutils.h>
#include <sstream>

namespace logging {

class logger_binary_command_plugin : public logger_command_plugin_interface {
public:
  const int kBinaryCommandId = 0x1001;

  logger_binary_command_plugin(const char* name = NULL) : plugin_name_(name ? name : "") {}
  virtual ~logger_binary_command_plugin() {}

  const char* type() const LOG_METHOD_OVERRIDE { return "binary_cmd"; }

  const char* name() const LOG_METHOD_OVERRIDE { return plugin_name_.c_str(); }

  void get_cmd_ids(int* out_cmd_ids, int max_cmds) const LOG_METHOD_OVERRIDE {
    if (max_cmds > 1) {
      out_cmd_ids[0] = kBinaryCommandId;
    }
  }

  bool cmd(std::string& out_result, int cmd_id, int verb_level, void* addr, const void* vparam, int iparam) LOG_METHOD_OVERRIDE {
    if (cmd_id != kBinaryCommandId)
      return false;

    std::stringstream ss;
    log_binary(ss, reinterpret_cast<const char*>(vparam), iparam);
    out_result = ss.str();
    return true;
  }

private:
  static void log_binary(std::ostream& stream, const char* data, int len) {
    using namespace logging::detail;

    const int output_byte_width = 16;
    int current_element = 0;

    while (current_element < len) {
      int line_start_element = current_element;
      stream << str::stringformat("%.4X: ", line_start_element);

      for (int i = 0; i < output_byte_width; i++) {
        if (current_element < len)
          stream << str::stringformat("%.2X ",
            static_cast<unsigned char>(data[current_element++]));
        else
          stream << "   ";
      }

      stream << "|";
      for (int j = line_start_element; j < line_start_element + output_byte_width; j++) {
        if (j >= len) break;
        stream << (static_cast<unsigned char>(data[j]) >= '!' ? data[j] : '.');
      }

      stream << std::endl;
    }
  }

  std::string plugin_name_;
};

class logger_binary_command_plugin_factory : public logger_plugin_default_factory<logger_binary_command_plugin> {
public:
  logger_binary_command_plugin_factory()
    : logger_plugin_default_factory<logger_binary_command_plugin>("binary_cmd", kLogPluginTypeCommand) {}
  virtual ~logger_binary_command_plugin_factory() LOG_METHOD_OVERRIDE {}
};

}//namespace logging

#endif /*LOGGER_BINARY_COMMAND_PLUGIN*/
