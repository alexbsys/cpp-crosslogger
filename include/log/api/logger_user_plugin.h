
#ifndef LOGGER_USER_PLUGIN_HEADER
#define LOGGER_USER_PLUGIN_HEADER

#include "logger_config.h"
#include "logger_interfaces.h"

#include "logger_user_plugin_api.h"

#include <sstream>
#include <vector>

namespace logging {

class logger_user_command_plugin : public logger_command_plugin_interface {
public:
  logger_user_command_plugin(void* plugin_obj, const char* name, const char* type_name, const std::vector<int>& cmds_ids, 
    const logger_user_command_plugin_callbacks_type& callbacks)
    : plugin_obj_(plugin_obj)
    , plugin_name_(name ? name : "")
    , plugin_type_name_(type_name ? type_name : "")
    , callbacks_(callbacks)
    , cmds_ids_(cmds_ids) {
  }

  virtual ~logger_user_command_plugin() LOG_METHOD_OVERRIDE {
    if (callbacks_.plugin_destroy_callback)
      callbacks_.plugin_destroy_callback(plugin_obj_);

    plugin_obj_ = NULL;
  }

  const char* type() const LOG_METHOD_OVERRIDE { return plugin_type_name_.c_str(); }

  const char* name() const LOG_METHOD_OVERRIDE { return plugin_name_.c_str(); }

  void get_cmd_ids(int* out_cmd_ids, int max_cmds) const LOG_METHOD_OVERRIDE {
    if (max_cmds < cmds_ids_.size())
      return;

    for (size_t i = 0; i < cmds_ids_.size(); i++) {
      out_cmd_ids[i] = cmds_ids_[i];
    }
  }

  bool cmd(std::string& out_result, int cmd_id, int verb_level, void* addr, const void* vparam, int iparam) LOG_METHOD_OVERRIDE {
    if (!callbacks_.plugin_cmd_callback)
      return false;

    char* buf_ptr = callbacks_.plugin_cmd_callback(plugin_obj_, cmd_id, verb_level, addr, vparam, iparam);
    if (!buf_ptr)
      return false;

    out_result = std::string(buf_ptr);

    if (callbacks_.plugin_free_buffer_callback)
      callbacks_.plugin_free_buffer_callback(plugin_obj_, buf_ptr);

    return true;
  }

  void config_updated(const logging::cfg::KeyValueTypeList& config) LOG_METHOD_OVERRIDE {
    if (!callbacks_.plugin_config_updated_callback)
      return;
    
    std::vector<logger_user_keyvalue_type> keyvals;
    keyvals.resize(config.size() + 1);

    size_t i = 0;

    for (logging::cfg::KeyValueTypeList::const_iterator it = config.cbegin(); it != config.cend(); it++) {
      keyvals[i].key = it->first.c_str();
      keyvals[i].value = it->second.c_str();
      ++i;
    }

    keyvals[i].key = NULL;
    keyvals[i].value = NULL;

    callbacks_.plugin_config_updated_callback(plugin_obj_, &(*keyvals.begin()));
  }

private:
  void* plugin_obj_;
  std::string plugin_name_;
  std::string plugin_type_name_;
  std::vector<int> cmds_ids_;
  logger_user_command_plugin_callbacks_type callbacks_;
};

class logger_user_command_plugin_factory : public logger_plugin_default_factory<logger_user_command_plugin> {
public:
  logger_user_command_plugin_factory(const char* type_name, int* cmds_ids, int cmds_ids_length,
    const logger_user_command_plugin_callbacks_type* callbacks)
    : logger_plugin_default_factory<logger_user_command_plugin>(type_name ? type_name : "", kLogPluginTypeCommand) {
    if (cmds_ids) {
      for (int i = 0; i < cmds_ids_length; i++)
        cmds_ids_.push_back(cmds_ids[i]);
    }
  }

  virtual ~logger_user_command_plugin_factory() LOG_METHOD_OVERRIDE {}

  virtual logging::shared_ptr<logger_plugin_interface> create(const char* name) LOG_METHOD_OVERRIDE {
    if (!callbacks_.plugin_create_callback)
      return logging::shared_ptr<logger_plugin_interface>();

    void* plugin_obj = callbacks_.plugin_create_callback();
    if (!plugin_obj)
      return logging::shared_ptr<logger_plugin_interface>();

    return logging::shared_ptr<logger_plugin_interface>(
      new logger_user_command_plugin(plugin_obj, name, type(), cmds_ids_, callbacks_));
  }

private:
  logger_user_command_plugin_callbacks_type callbacks_;
  std::vector<int> cmds_ids_;
};

}//namespace logging

#endif /*LOGGER_USER_PLUGIN_HEADER*/
