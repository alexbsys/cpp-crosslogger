
#ifndef LOGGER_INTERFACES_HEADER
#define LOGGER_INTERFACES_HEADER

#include "logger_config.h"
#include "logger_pdefs.h"
#include "logger_cfgfn.h"
#include <string>

namespace logging {

namespace detail {
  class log_stream;
}//namespace detail

struct logger_interface;

enum log_plugin_type {
  kLogPluginTypeInvalid = 0,
  kLogPluginTypeOutput = 1,
  kLogPluginTypeConfigMacro = 2,
  kLogPluginTypeConfig = 3,
  kLogPluginTypeHeaderMacro = 4,
  kLogPluginTypeCommand = 5,
  kLogPluginTypeArgsCommand = 6,

  kLogPluginTypeMin = 1,
  kLogPluginTypeMax = 6
};

struct logger_plugin_interface {
  virtual ~logger_plugin_interface() {}

  virtual int plugin_type() const { return kLogPluginTypeInvalid; }
  virtual const char* type() const { return ""; }
  virtual const char* name() const { return ""; }
  virtual const char* deps() const { return ""; }

  /**
   * \brief    configuration update handler. Logger called this method every time when configuration has been updated
   */
  virtual void config_updated(const logging::cfg::KeyValueTypeList& config) {}

  virtual bool attach(logger_interface* logger) { return true; }
  virtual void detach(logger_interface* logger) {}
};

struct logger_plugin_factory_interface {
  virtual ~logger_plugin_factory_interface() {}

  virtual int plugin_type() const { return kLogPluginTypeInvalid; }
  virtual const char* type() const { return ""; }
  virtual logger_plugin_interface* create(const char* name) = 0;
  virtual void destroy(logger_plugin_interface* plugin_interface) = 0;
};


template<typename T>
class logger_plugin_default_factory : public logger_plugin_factory_interface {
public:
  logger_plugin_default_factory(const std::string& plugin_type_name, int plugin_type)
    : plugin_type_name_(plugin_type_name), plugin_type_(plugin_type) {}

  virtual ~logger_plugin_default_factory() {}

  const char* type() const { return plugin_type_name_.c_str(); }
  int plugin_type() const { return plugin_type_; }

  virtual logger_plugin_interface* create(const char* name) {
    return new T(name);
  }

  virtual void destroy(logger_plugin_interface* plugin_interface) {
    if (dynamic_cast<T*>(plugin_interface)) {
      delete plugin_interface;
    }
  }

private:
  std::string plugin_type_name_;
  int plugin_type_;
};


/**
 * \struct    logger_output_interface interface. User can implement this interface for custom
 *            output implementation
 */
struct logger_output_plugin_interface : public virtual logger_plugin_interface {
  virtual ~logger_output_plugin_interface() {}

  virtual int plugin_type() const { return kLogPluginTypeOutput; }

  /** 
   * \brief    write method. Called every write to file operation
   */
  virtual void write(int verb_level, const std::string& hdr, const std::string& what) {}

  /**
   * \brief    flush output method
   */
  virtual void flush() {}

  /**
   * \brief    close output method
   */
  virtual void close() {}
};

struct logger_config_macro_plugin_interface : public virtual logger_plugin_interface {
  virtual ~logger_config_macro_plugin_interface() {}

  virtual int plugin_type() const { return kLogPluginTypeConfigMacro; }
  virtual bool process(std::string& str) { return false;  }
};

struct logger_config_plugin_interface : public virtual logger_plugin_interface {
  virtual ~logger_config_plugin_interface() {}

  virtual int plugin_type() const { return kLogPluginTypeConfig; }
  virtual bool reload(cfg::KeyValueTypeList& config) { return false;  }
};


struct logger_command_plugin_interface : public virtual logger_plugin_interface {
  virtual ~logger_command_plugin_interface() {}

  virtual void get_cmd_ids(int* out_cmd_ids, int max_cmds) const {}

  virtual int plugin_type() const { return kLogPluginTypeCommand; }
  virtual bool cmd(std::string& out_result, int cmd_id, int verb_level, void* addr, const void* vparam, int iparam) { return false; }
};

struct logger_args_command_plugin_interface : public virtual logger_plugin_interface {
  virtual ~logger_args_command_plugin_interface() {}

  virtual void get_cmd_ids(int* out_cmd_ids, int max_cmds) const {}

  virtual int plugin_type() const { return kLogPluginTypeArgsCommand; }
  virtual bool cmd_args(int cmd_id, int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number, const void* vparam, int iparam, va_list arguments) {
    return false;
  }
};


/**
* \struct   logger_interface  interface. User do not need to use it directly, only via
*           macroses LOG_XXXX because they put function name, source file and line number
*           automatically
*/
struct logger_interface {
  virtual ~logger_interface() {}

  /** Log binary data */
  virtual void log_binary(int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number, const char* data,
    int len) = 0;

  /** Log format, like printf to log */
  virtual void LOG_CDECL log(int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number, const char* format,
    ...) = 0;

  /** Log arguments va_list, like vsprintf to log */
  virtual void log_args(int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number, const char* format,
    va_list arguments) = 0;

  /** Logger stream interface implementation (LOGS_XXXX macroses) */
  virtual detail::log_stream stream(int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number) = 0;

  /** Flush all outputs */
  virtual void flush() = 0;

  /** Reload configuration */
  virtual void reload_config() = 0;

  virtual void dump_state(int verb_level) = 0;

  /** Add logger output interface */
  virtual bool attach_plugin(logger_plugin_interface* plugin_interface, void(*plugin_delete_fn)(logger_interface*, logger_plugin_interface*) = NULL) = 0;

  /** Remove logger output interface */
  virtual bool detach_plugin(logger_plugin_interface* plugin_interface, bool delete_plugin = true) = 0;

  virtual bool register_plugin_factory(logger_plugin_factory_interface* plugin_factory_interface) = 0;
  virtual bool unregister_plugin_factory(logger_plugin_factory_interface* plugin_factory_interface) = 0;

  virtual void set_config_param(const char* key, const char* value) = 0;
  virtual const char* get_config_param(const char* key) const = 0;

  virtual void log_cmd(int cmd_id, int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number, const void* vparam, int iparam) = 0;

  virtual void log_cmd_args(int cmd_id, void* addr, const char* function_name,
    const char* source_file, int line_number, const void* vparam, int iparam, va_list args) = 0;

  virtual void LOG_CDECL log_cmd_vargs(int cmd_id, void* addr, const char* function_name,
    const char* source_file, int line_number, const void* vparam, int iparam, ...) = 0;

#if LOG_USE_MODULEDEFINITION
  /** Log all loaded modules information */
  virtual void log_modules(int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number) = 0;
#endif  /*LOG_USE_MODULEDEFINITION*/

#if LOG_AUTO_DEBUGGING
  /** Log current position stack trace */
  virtual void log_stack_trace(int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number) = 0;
#endif  // LOG_AUTO_DEBUGGING
  /** Log exception text */
  virtual void log_exception(int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number,
    const char* user_msg) = 0;

  /** Log std::exception */
  virtual void log_exception(int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number,
    std::exception* e) = 0;

#if LOG_USE_OBJMON
  /** Register object instance */
  virtual void log_objmon_register(size_t hash_code, const char* type_name,
    void* ptr) = 0;

  /** Unregister object instance */
  virtual void log_objmon_unregister(size_t hash_code, void* ptr) = 0;

  /** Dump all registered objects instances */
  virtual void log_objmon_dump(int verb_level) = 0;
#endif  // LOG_USE_OBJMON

  virtual void ref() = 0;
  virtual void deref() = 0;
  virtual int ref_counter() = 0;
};


}//namespace logging


#endif /*LOGGER_INTERFACES_HEADER*/
