
#ifndef LOGGER_MODULES_COMMAND_PLUGIN
#define LOGGER_MODULES_COMMAND_PLUGIN

#include <log/logger_interfaces.h>
#include <log/detail/logger_strutils.h>
#include <log/detail/logger_moddef.h>

#include <sstream>

namespace logging {

  class logger_modules_command_plugin : public logger_command_plugin_interface {
  public:
    const int kModulesCommandId = 0x1002;

    logger_modules_command_plugin(const char* name = NULL) : plugin_name_(name ? name : "") {}
    virtual ~logger_modules_command_plugin() LOG_METHOD_OVERRIDE {}

    const char* type() const LOG_METHOD_OVERRIDE { return "modules_cmd"; }

    const char* name() const LOG_METHOD_OVERRIDE { return plugin_name_.c_str(); }

    void get_cmd_ids(int* out_cmd_ids, int max_cmds) const LOG_METHOD_OVERRIDE {
      if (max_cmds > 1) {
        out_cmd_ids[0] = kModulesCommandId;
      }
    }

    bool cmd(std::string& out_result, int cmd_id, int verb_level, void* addr, const void* vparam, int iparam) LOG_METHOD_OVERRIDE {
      using namespace detail;
      if (cmd_id != kModulesCommandId)
        return false;

      std::stringstream sstream;

      std::list<module_entry_t> modules;
      if (!module_definition::query_module_list(modules)) 
        return false;

      sstream << "Base\tSize\tModule name\tImage name\tVersion info" << std::endl;
      sstream << "------------------------------------------------" << std::endl;

      for (std::list<module_entry_t>::iterator i = modules.begin(); i != modules.end();
        i++) {
        const module_entry_t& mod_entry = *i;

#ifdef LOG_PLATFORM_64BIT
        sstream << stringformat("0x%.8X%.8X", static_cast<uint32_t>(mod_entry.baseAddress >> 32),
          mod_entry.baseAddress) << "\t";
#endif  // LOG_PLATFORM_64BIT

#ifdef LOG_PLATFORM_32BIT
        sstream << str::stringformat("0x%.8X", mod_entry.base_address) << "\t";
#endif  // LOG_PLATFORM_32BIT

        sstream << mod_entry.size << "\t" << mod_entry.module_name.c_str() << "\t";
        sstream << mod_entry.image_name.c_str() << "\t";

        if (mod_entry.lang_id)
          sstream << "LangId:" << str::stringformat("%.4X", mod_entry.lang_id) << "\t";

        if (mod_entry.file_version.size())
          sstream << "FileVersion:"
          << "\"" << mod_entry.file_version << "\""
          << "\t";

        if (mod_entry.product_version.size())
          sstream << "ProductVersion:"
          << "\"" << mod_entry.product_version << "\""
          << "\t";

        if (mod_entry.company_name.size())
          sstream << "CompanyName:"
          << "\"" << mod_entry.company_name << "\""
          << "\t";

        if (mod_entry.file_description.size())
          sstream << "FileDescription:"
          << "\"" << mod_entry.file_description << "\""
          << "\t";

        sstream << std::endl;
      }

      out_result = sstream.str();
      return true;
    }

  private:
    std::string plugin_name_;
  };

  class logger_modules_command_plugin_factory : public logger_plugin_default_factory<logger_modules_command_plugin> {
  public:
    logger_modules_command_plugin_factory()
      : logger_plugin_default_factory<logger_modules_command_plugin>("modules_cmd", kLogPluginTypeCommand) {}
    virtual ~logger_modules_command_plugin_factory() {}
  };

}//namespace logging

#endif /*LOGGER_MODULES_COMMAND_PLUGIN*/
