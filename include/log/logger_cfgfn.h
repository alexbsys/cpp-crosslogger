
#ifndef LOGGER_CFG_FUNCTIONS_HEADER
#define LOGGER_CFG_FUNCTIONS_HEADER

#include <string>
#include <list>
#include <stdlib.h>

namespace logging {

namespace cfg {
  typedef std::pair<std::string, std::string> KeyValueType;
  typedef std::list<KeyValueType> KeyValueTypeList;
}//namespace cfg


namespace detail {
namespace cfg {
  using namespace logging::cfg;
  
  // to prevent temporary reference returns
  static const std::string empty_string;

  static const std::string& get_value(
    const KeyValueTypeList& params,
    const std::string& param_name,
    const std::string& default_value = empty_string,
    bool* is_found = nullptr) {
    if (is_found) *is_found = false;

    for (const KeyValueType& param : params) {
      if (param.first == param_name) {
        if (is_found) *is_found = true;
        return param.second;
      }
    }

    return default_value;
  }

  static void set_value(
    KeyValueTypeList& params,
    const std::string& param_name,
    const std::string& new_value,
    std::string* prev_value = nullptr,
    bool* is_found = nullptr) {
    if (is_found) *is_found = false;
    if (prev_value) *prev_value = std::string();

    for (KeyValueType& param : params) {
      if (param.first == param_name) {
        if (is_found) *is_found = true;
        if (prev_value) *prev_value = param.second;

        param.second = new_value;
        return;
      }
    }

    KeyValueType keyval(param_name, new_value);
    params.push_back(keyval);
  }

  static bool has_value(
    const KeyValueTypeList& params,
    const std::string& param_name) {
    for (const KeyValueType& param : params) {
      if (param.first == param_name)
        return true;
    }
    return false;
  }

  static size_t values_count(
    const KeyValueTypeList& params,
    const std::string& param_name) {
    size_t count = 0;

    for (const KeyValueType& param : params) {
      if (param.first == param_name)
        ++count;
    }
    return count;
  }


  static const std::string& get_logcfg_string(const KeyValueTypeList& params, const std::string& type,
    const std::string& name, const std::string& key, const std::string& default_value = empty_string, bool* is_found = NULL) {

    bool is_found_dummy = false;
    if (is_found == NULL)
      is_found = &is_found_dummy;

    //all params
    const std::string& result_qualified = get_value(params, type + std::string(":") + name + std::string(":") + key, default_value, is_found);
    if (*is_found)
      return result_qualified;

    //skip name
    const std::string& result_skipname = get_value(params, type + std::string("::") + key, default_value, is_found);
    if (*is_found)
      return result_skipname;

    //global section value
    const std::string& result_loggersection = get_value(params, std::string("logger::") + key, default_value, is_found);
    if (*is_found)
      return result_loggersection;

    // no section value
    const std::string& result_nosection = get_value(params, std::string("::") + key, default_value, is_found);
    if (*is_found)
      return result_nosection;

    // global parameter value
    const std::string& result_global = get_value(params, key, default_value, is_found);
    if (*is_found)
      return result_global;

    return default_value;
  }

  static int get_logcfg_int(const KeyValueTypeList& params, const std::string& type,
    const std::string& name, const std::string& key, int default_value = 0, bool* is_found = NULL) {
    bool is_found_dummy = false;
    if (is_found == NULL)
      is_found = &is_found_dummy;

    int ret = atoi(get_logcfg_string(params, type, name, key, std::string("0"), is_found).c_str());
    if (*is_found == false) {
      return default_value;
    }

    return ret;
  }



}//namespace cfg
}//namespace detail
}//namespace logging

#endif /*LOGGER_CFG_FUNCTIONS_HEADER*/
