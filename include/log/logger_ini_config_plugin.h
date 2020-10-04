#ifndef LOGGER_INI_CONFIG_PLUGIN_HEADER
#define LOGGER_INI_CONFIG_PLUGIN_HEADER


#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_pdefs.h"
#include "logger_interfaces.h"

#include "logger_strutils.h"
#include "logger_cfgfn.h"

#include "logger_ini_parser.h"

#include <vector>

namespace logging {

class logger_ini_config_plugin : public logger_config_plugin_interface {
public:
  logger_ini_config_plugin(const char* ini_file_paths = NULL)
    : ini_file_paths_(ini_file_paths == NULL ? std::string() : ini_file_paths) {
  }

  virtual ~logger_ini_config_plugin() {}

  const char* type() const { return "ini_config"; }

  bool reload(cfg::KeyValueTypeList& config) {
#if LOG_INI_CONFIGURATION

    using namespace detail::ini_parser;

    const std::string& ini_file_paths = detail::cfg::get_logcfg_string(config, type(), name(), "IniFilePaths", ".");

    std::vector<std::string> ini_paths;
    detail::str::split(ini_file_paths, ini_paths, ';');

    bool ini_parsed = false;

    for (size_t i = 0; i < ini_paths.size(); i++) {
      if (!ini_paths[i].size()) continue;

      if (ini_parse(ini_paths[i].c_str(), handler, &config) >= 0) {
        ini_parsed = true;
        break;
      }
    }
#endif /*LOG_INI_CONFIGURATION*/
    return true;
  }
protected:
  // config string   type::name::key = value
  static int LOG_CDECL handler(void* user, const char* section, const char* name,
    const char* value) {
    (void)user;

    cfg::KeyValueTypeList* config_ptr = reinterpret_cast<cfg::KeyValueTypeList*>(user);
    cfg::KeyValueTypeList& config(*config_ptr);

    std::string sect(section);
    std::string key(name);
    std::string val(value);

    if (sect.find_first_of(':') == std::string::npos)
      sect.append(":");

    std::string key_path = sect + std::string(":") + key;
    detail::cfg::set_value(config, key_path, val);

    return 1;
  }

private:
  std::string ini_file_paths_;
};


}//namespace logging

#endif /*LOGGER_INI_CONFIG_PLUGIN*/
