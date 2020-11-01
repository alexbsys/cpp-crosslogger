
#ifndef LOGGER_WIN_REGISTRY_CONFIG_PLUGIN_HEADER
#define LOGGER_WIN_REGISTRY_CONFIG_PLUGIN_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_pdefs.h"
#include "logger_interfaces.h"

#include "logger_strutils.h"
#include "logger_cfgfn.h"

#include "logger_win_registry.h"

#include <vector>

#define LOGGER_HAVE_BUILTIN_WIN_REGISTRY_CONFIG_PLUGIN  1

namespace logging {

class logger_win_registry_config_plugin : public logger_config_plugin_interface {
public:
  logger_win_registry_config_plugin(const char* plugin_name = NULL)
    : plugin_name_(plugin_name == NULL ? std::string() : plugin_name) {
  }

  virtual ~logger_win_registry_config_plugin() {}

  const char* type() const { return "win_registry_config"; }
  const char* name() const { return plugin_name_.c_str(); }

  bool reload(cfg::KeyValueTypeList& config) {
    const std::string kLoggerConfigTypeName = "logger";
    const std::string kRegistryConfigPathParamName = "RegistryConfigPath";

    std::string key_path = detail::cfg::get_logcfg_string(config, kLoggerConfigTypeName, 
      std::string(), kRegistryConfigPathParamName, std::string());

    if (key_path == "") key_path = LOG_REGISTRY_DEFAULT_KEY;

    size_t storage_delim = key_path.find_first_of("\\/");
    if (storage_delim == key_path.npos) return false;

    std::string storage = key_path.substr(0, storage_delim);
    HKEY base_key = detail::log_registry_helper::reg_base_key_by_string(storage);

    std::string path = key_path.substr(storage_delim + 1);

    return get_parameters_recursive(base_key, path, std::string(), config);
  }

protected:
  static bool get_parameters_recursive(HKEY base_key, 
    const std::string& reg_key_path, std::string section_base, cfg::KeyValueTypeList& config) {
    using namespace detail;

    std::vector<std::string> subkeys;
    std::vector<std::string> values;
    if (!log_registry_helper::reg_query_subkeys_values(base_key, reg_key_path, subkeys, values))
      return false;

    for (size_t i = 0; i < values.size(); i++) {
      bool key_read;
      std::string val;

      std::string key_path;
      
      if (section_base == std::string()) {
        key_path = std::string("logger::") + values[i];
      } else {
        key_path = section_base + std::string(":") + values[i];
      }

      key_read = log_registry_helper::get_reg_sz_value(base_key, reg_key_path, values[i], val);
      if (!key_read) {
        unsigned long dword_val;
        key_read = log_registry_helper::get_reg_dword_value(base_key, reg_key_path, values[i], dword_val);
          
        if (key_read)
          val = str::stringformat("%u", dword_val);
      }

      if (key_read)
        detail::cfg::set_value(config, key_path, val);
    }

    for (size_t i = 0; i < subkeys.size(); i++) {
      std::string subkey_reg_path = reg_key_path + std::string("\\") + subkeys[i];
      std::string subkey_section_base = section_base + std::string(":") + subkeys[i];

      get_parameters_recursive(base_key, subkey_reg_path, subkey_section_base, config);
    }
  }

private:
  std::string plugin_name_;
};

class logger_win_registry_config_plugin_factory : public logger_plugin_default_factory<logger_win_registry_config_plugin> {
public:
  logger_win_registry_config_plugin_factory()
    : logger_plugin_default_factory<logger_win_registry_config_plugin>("win_registry_config", kLogPluginTypeConfig) {}
  virtual ~logger_win_registry_config_plugin_factory() LOG_METHOD_OVERRIDE {}
};

}//namespace logging

#endif /*LOGGER_WIN_REGISTRY_CONFIG_PLUGIN_HEADER*/
