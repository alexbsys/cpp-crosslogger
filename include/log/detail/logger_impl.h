
// Logger implementation part
// This file must be included from C++ part

#ifndef LOGGER_IMPL_HEADER
#define LOGGER_IMPL_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_pdefs.h"
#include "logger_interfaces.h"

#include "logger_cfgfn.h"
#include "logger_varargs.h"
#include "logger_verbose.h"

#include "logger_mt.h"
#include "logger_plugin_manager.h"

#include "logger_utils.h"
#include "logger_strutils.h"
#include "logger_stream.h"

#if LOG_SHARED
#include "logger_shmem.h"
#endif /*LOG_SHARED*/

#if LOG_USE_SYSTEMINFO
#include "logger_sysinfo.h"
#endif /*LOG_USE_SYSTEMINFO*/

#if LOG_USE_MODULEDEFINITION
#include "logger_moddef.h"
#endif /*LOG_USE_MODULEDEFINITION*/

namespace logging {
namespace detail {

static const char* default_hdr_format =
    "[$(V)] $(dd).$(MM).$(yyyy) $(hh):$(mm):$(ss).$(ttt) [$(PID):$(TID)] "
    "[$(module)!$(function)]";

class logger_modules_cache {
public:
  bool is_module_in_cache(const char* name, intptr_t base_addr, intptr_t end_addr) const {
    mt::reader_spinlock lock(locker_);
    return find_cached_module(name, base_addr, end_addr);
  }

  std::string get_module_name_by_addr(void* ptr) const {
    intptr_t addr = (intptr_t)ptr;
    std::string result;
    if (!addr) return result;

    mt::reader_spinlock lock(locker_);

    for (size_t i = 0; i < cached_modules_.size(); i++) {
      if (cached_modules_[i].base_addr <= addr && cached_modules_[i].end_addr >= addr) {
        result = cached_modules_[i].name;
        break;
      }
    }

    return result;
  }

  void clear() {
    mt::writer_spinlock lock(locker_);
    cached_modules_.clear();
  }

  bool add_module_to_cache(intptr_t base_addr, intptr_t end_addr, const std::string& module_name) {
    bool result = false;

    mt::writer_spinlock lock(locker_);

    if (!find_cached_module(module_name.c_str(), base_addr, end_addr)) {
      cache_module_t mod;
      mod.base_addr = base_addr;
      mod.end_addr = end_addr;
      mod.name = module_name;
      cached_modules_.push_back(mod);
      result = true;
    }

    return result;
  }

private:
  bool find_cached_module(const char* name, intptr_t base_addr, intptr_t end_addr) const {
    for (std::vector<cache_module_t>::const_iterator i = cached_modules_.cbegin();
      i != cached_modules_.cend(); i++) {
      if (i->base_addr == base_addr && i->end_addr == end_addr && i->name == name)
        return true;
    }
    return false;
  }

  typedef struct _cache_module_t {
    intptr_t base_addr;
    intptr_t end_addr;
    std::string name;
  } cache_module_t;

  std::vector<cache_module_t> cached_modules_;
  mutable mt::read_write_spinlock locker_;
};



#if LOG_USE_MACRO_HEADER_CACHE
struct log_macro_cache_t {
  bool valid_;

  std::string processed_cached_hdr_full;
  std::string processed_cached_hdr_partial;
  struct tm processed_cached_tm;
  std::string processed_cached_src_file;
  std::string processed_cached_function_name;
  std::string processed_cached_module_name;
  std::string processed_cached_hdr_format;
  int processed_cached_global_verb_level;
  int processed_cached_verb_level;
  int processed_cached_line_num;
  int processed_cached_millitm;
  unsigned long processed_cached_pid;
  unsigned long processed_cached_tid;

  mt::read_write_spinlock cache_lock_;

  log_macro_cache_t() : valid_(false), processed_cached_global_verb_level(0), processed_cached_verb_level(0), processed_cached_line_num(0), processed_cached_millitm(0),
    processed_cached_pid(0), processed_cached_tid(0)  {}
};
#endif /*LOG_USE_MACRO_HEADER_CACHE*/




/**
 * \class   Logger main class
 */
class logger : public logger_interface {
 private:
  struct log_config {
    bool process_macro_in_log_text_;
    std::string header_format_;
    bool log_sys_info_;
    int filter_verbose_level_;
    bool load_plugins_from_config_;
    std::string load_plugins_;

    log_config() : process_macro_in_log_text_(false), log_sys_info_(true), filter_verbose_level_(logger_verbose_all)
      ,load_plugins_from_config_(true)
    {}
  };

  log_config log_config_;

  std::map<int, logging::shared_ptr<logger_plugin_interface> > cmd_refs_;
  plugin_manager* plugin_mgr_;

  mt::atomic_long_type ref_counter_;
  bool first_write_;
  cfg::KeyValueTypeList config_;

  cfg::KeyValueTypeList user_code_config_;

  void(*delete_fn_)(logger_interface* ptr, void* user_param);
  void* delete_fn_user_param_;

#if LOG_SHARED
  bool shared_master_;
  void* shared_obj_ptr_;
#endif  // LOG_SHARED

  LOG_MT_MUTEX mt_buffer_lock_;

#if LOG_MULTITHREADED
  /** Log record message structure */
  struct log_record {
    int verb_level_;
    std::string hdr_;
    std::string text_;
    log_record() : verb_level_(logger_verbose_mute) {}
  };

  /** Multithreaded buffer */
  std::list<log_record> mt_buffer_;

  LOG_MT_EVENT_TYPE write_event_;
  LOG_MT_EVENT_TYPE buffer_devastated_;
  LOG_MT_THREAD_HANDLE_TYPE  log_thread_handle_;

  /** Logger terminating flag */
  mt::atomic_long_type mt_terminating_;


private:

  /** 
   * \brief    Logger thread function
   * \param    data    Pointer to logger* object
   * \return   thread exit code
   */
  static unsigned long
#ifdef LOG_PLATFORM_WINDOWS
      __stdcall
#endif  // LOG_PLATFORM_WINDOWS
      log_thread_fn(void* data) {
    const int kPollWriteEventMs = 250;
    utils::set_current_thread_name("logger_thread");
    logger* log = reinterpret_cast<logger*>(data);
    
    while (true) {
      std::list<log_record> log_records;
      //std::list<plugin_data> outputs;
      std::vector<shared_ptr<logger_output_plugin_interface> > output_plugins;

      long mt_terminating;

      // mutex locked section
      {
        mt::mutex_scope_lock lock(&log->mt_buffer_lock_);

        if (log->mt_buffer_.size() == 0) {
          mt::wait_event(&log->write_event_, &log->mt_buffer_lock_, true, kPollWriteEventMs);
        }

        // copy all records from log to local and clear log buffer
        std::copy(log->mt_buffer_.begin(), log->mt_buffer_.end(), std::back_inserter(log_records));
        log->mt_buffer_.clear();

        log->plugin_mgr_->query_plugins<logger_output_plugin_interface>(kLogPluginTypeOutput, output_plugins);
        //outputs = log->plugins_[kLogPluginTypeOutput];

        mt_terminating = log->mt_terminating_;
      }

      // notify that log buffer has been devastated
      mt::fire_event(&log->buffer_devastated_);

      while (log_records.size()) {
        const log_record& record = *log_records.begin();

        // call registered outputs
        for (std::vector<shared_ptr<logger_output_plugin_interface> >::const_iterator it = output_plugins.cbegin();
          it != output_plugins.cend(); it++) {
          (*it)->write(record.verb_level_, record.hdr_, record.text_);
        }
        log_records.pop_front();
      }

      if (mt_terminating) {
        for (std::vector<shared_ptr<logger_output_plugin_interface> >::const_iterator it = output_plugins.cbegin();
          it != output_plugins.cend(); it++) {
            (*it)->flush();
            (*it)->close();
        }

        break;
      }
    }

    LOG_MT_THREAD_EXIT(0);
    return 0;
  }

  /**
   * \brief    Handle situation when multithreaded buffer has overflow. mt_buffer_lock_ mutex must be locked before call
   */
  void handle_buffer_overflow(bool stop_caller) {
    const int kWaitTimeoutMs = 30;

    if (stop_caller) {
      // Wait until log process messages
      while (mt_buffer_.size() > LOG_MULTITHREADED_MAX_BUFFERED_MESSAGES) {
        mt::wait_event(&buffer_devastated_, &mt_buffer_lock_, true, kWaitTimeoutMs);
      }
    } else {  
      // Remove messages from buffer
      size_t remove_msgs = (mt_buffer_.size() / 10) + 1;
      if (remove_msgs > mt_buffer_.size())
        remove_msgs = mt_buffer_.size();

      for (size_t i = 0; i < remove_msgs; i++)
        mt_buffer_.pop_front();

      // notify in log about overflow
      log_record overflow_msg;
      overflow_msg.verb_level_ = logger_verbose_fatal;
      overflow_msg.text_ = str::stringformat("*** Log overflow. Removed %d log messages", static_cast<int>(remove_msgs));
      mt_buffer_.push_back(overflow_msg);
    }
  }


  /** Multithreaded put_to_stream */
  void put_to_stream(int verb_level, const std::string& hdr, const std::string& what) {
    handle_first_write();

    mt::mutex_scope_lock lock(&mt_buffer_lock_);

    if (LOG_MULTITHREADED_MAX_BUFFERED_MESSAGES &&
        mt_buffer_.size() > LOG_MULTITHREADED_MAX_BUFFERED_MESSAGES) {
      handle_buffer_overflow(LOG_MULTITHREADED_STOP_CALLER_ON_OVERFLOW);
    }

    log_record record;
    record.hdr_ = hdr;
    record.text_ = what;
    record.verb_level_ = verb_level;

    mt_buffer_.push_back(record);
    mt::fire_event(&write_event_);
  }



#else  // LOG_MULTITHREADED
  /** Single threaded put_to_stream */
  LOG_INLINE void put_to_stream(int verb_level, const std::string& hdr, const std::string& what) {
    handle_first_write();
    std::string text = hdr + std::string(" ") + what + std::string("\n");

    // call registered outputs
    std::vector<shared_ptr<logger_output_plugin_interface> > output_plugins;
    plugin_mgr_->query_plugins<logger_output_plugin_interface>(kLogPluginTypeOutput, output_plugins);

    // call registered outputs
    for (std::vector<shared_ptr<logger_output_plugin_interface> >::const_iterator it = output_plugins.cbegin();
      it != output_plugins.cend(); it++) {
      (*it)->write(verb_level, hdr, what);
    }
  }
#endif  // LOG_MULTITHREADED

  /** Process first write in session */
  void handle_first_write() {
    if (first_write_) {
      first_write_ = false;
      put_to_stream(logger_verbose_all, "", "");
#if LOG_USE_SYSTEMINFO
      if (log_config_.log_sys_info_) 
        put_to_stream(logger_verbose_all, std::string(), query_system_info());
#endif  // LOG_USE_SYSTEMINFO
    }
  }

  std::string process_config_macros_value(std::string val) const {
    while (true) {
      bool replaced_some = false;

      std::vector<shared_ptr<logger_config_macro_plugin_interface> > config_macro_plugins;
      plugin_mgr_->query_plugins<logger_config_macro_plugin_interface>(kLogPluginTypeConfigMacro, config_macro_plugins);
      
      for (std::vector<shared_ptr<logger_config_macro_plugin_interface> >::const_iterator it = config_macro_plugins.cbegin();
        it != config_macro_plugins.cend(); it++) {
        replaced_some |= (*it)->process(val);
      }

      if (!replaced_some)
        break;
    }

    return val;
  }


  template<typename TCmdPlugin>
  void register_command_plugin(logging::shared_ptr<TCmdPlugin> cmd_plugin) {
    const int kMaxCmdsPerPlugin = 32;

    int cmd_ids[kMaxCmdsPerPlugin];
    memset(cmd_ids, 0, sizeof(cmd_ids));
    cmd_plugin->get_cmd_ids(cmd_ids, kMaxCmdsPerPlugin);

    for (int i = 0; i < kMaxCmdsPerPlugin; i++) {
      if (!cmd_ids[i])
        continue;

      if (cmd_refs_.find(cmd_ids[i]) == cmd_refs_.end()) {
        cmd_refs_.insert(std::pair<int, shared_ptr<logger_plugin_interface> >(cmd_ids[i], cmd_plugin));
      }
    }
  }

 public:
  int ref() { 
    long refs = mt::atomic_increment(&ref_counter_);
    return refs;
  }

  int deref() {
    long refs = mt::atomic_decrement(&ref_counter_);
    if (refs <= 0) {
      // this is same as 'delete this'
      if (delete_fn_) delete_fn_(this, delete_fn_user_param_);
    }

    return refs;
  }

  int ref_counter() const { return ref_counter_; }

  void set_config_param(const char* key, const char* value) {
    std::string val(value ? value : "");
    cfg::set_value(user_code_config_, key, process_config_macros_value(val));

    update_config();
  }

  // TODO: maybe need to add remove_config_param_override ?

  int get_config_param(const char* key, char* buffer, int buffer_size) const {
    std::string val = cfg::get_value(config_, key);
    if (buffer && buffer_size) {
      *buffer = 0;
      if (buffer_size > static_cast<int>(val.size()) + 1) {
        memcpy(buffer, val.c_str(), val.size() + 1);
        return 1;
      }
    }
    return 0;
  }

  bool register_plugin_factory(logger_plugin_factory_interface* plugin_factory_interface) {
    return plugin_mgr_->register_plugin_factory(plugin_factory_interface);
  }

  bool unregister_plugin_factory(logger_plugin_factory_interface* plugin_factory_interface) {
    return plugin_mgr_->unregister_plugin_factory(plugin_factory_interface);
  }


  void flush() {
    mt::mutex_scope_lock lock(&mt_buffer_lock_);

#if LOG_MULTITHREADED
    const int kPollBufferDevastatedEventMs = 100;

    // flush multithreaded buffer
    while(mt_buffer_.size()) {
      if (mt::wait_event(&buffer_devastated_, &mt_buffer_lock_, true, kPollBufferDevastatedEventMs))
        break;
    }
#endif  // LOG_MULTITHREADED

    std::vector<shared_ptr<logger_output_plugin_interface> > output_plugins;
    plugin_mgr_->query_plugins(kLogPluginTypeOutput, output_plugins);

    // flush registered outputs
    for (std::vector<shared_ptr<logger_output_plugin_interface> >::const_iterator it = output_plugins.cbegin();
      it != output_plugins.cend(); it++) {
      (*it)->flush();
    }
  }

  log_stream stream(int verb_level, void* addr, const char* function_name, const char* source_file, int line_number) {
    return log_stream(this, verb_level, addr, function_name, source_file, line_number);
  }

  bool attach_plugin(logger_plugin_interface* plugin_interface) LOG_METHOD_OVERRIDE {
    shared_ptr<logger_plugin_interface> plugin_ptr(plugin_interface);
    return plugin_mgr_->attach_plugin(plugin_ptr);
  }

  bool detach_plugin(logger_plugin_interface* plugin_interface) LOG_METHOD_OVERRIDE {
    return plugin_mgr_->detach_plugin(plugin_interface);
  }

  unsigned int get_version() const LOG_METHOD_OVERRIDE {
    return 0x02010101;
  }

  bool is_master() const LOG_METHOD_OVERRIDE {
#if LOG_SHARED
    return shared_master_;
#else /*LOG_SHARED*/
    return true;
#endif /*LOG_SHARED*/
  }

  /**
   * \brief    Logger object constructor
   */
  logger(void(*delete_fn)(logger_interface* ptr, void* user_param), void* delete_fn_user_param)
      : ref_counter_(0)
      , first_write_(true)
      , delete_fn_(delete_fn)
      , delete_fn_user_param_(delete_fn_user_param)
#if LOG_SHARED
      , shared_master_(false)
      , shared_obj_ptr_(NULL)
#endif  // LOG_SHARED

#if LOG_MULTITHREADED
      , mt_terminating_(0)
#endif  // LOG_MULTITHREADED
  {
#if LOG_SHARED
    if (shared_obj::try_find_shared_object(0) == NULL) {
      shared_obj_ptr_ =
          shared_obj::create_shared_object(0, dynamic_cast<logger_interface*>(this));
      shared_master_ = shared_obj_ptr_ ? true : false;
    }
#endif  /*LOG_SHARED*/

    plugin_mgr_ = new plugin_manager(this);

    LOG_MT_MUTEX_INIT(&mt_buffer_lock_, NULL);

    reload_config();

#if LOG_MULTITHREADED
    mt::create_event(&write_event_);
    mt::create_event(&buffer_devastated_);

    log_thread_handle_ = mt::thread_start(&log_thread_fn, this);

//#  ifdef LOG_PLATFORM_WINDOWS
//
//    DWORD thread_id;
//    log_thread_handle_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&log_thread_fn,
//                                     this, 0, &thread_id);    
//#  else   /*LOG_PLATFORM_WINDOWS*/
//    pthread_create(&log_thread_handle_, NULL, (void* (*)(void*)) & log_thread_fn, this);
//#  endif  /*LOG_PLATFORM_WINDOWS*/

#endif  /*LOG_MULTITHREADED*/
  }

  /**
   * \brief  Logger object destructor
   */
  virtual ~logger() {
#if LOG_SHARED
    if (shared_master_ && shared_obj::free_shared_object(0)) {
      shared_obj_ptr_ = NULL;
      shared_master_ = false;
#if LOG_CHECKED
      assert(shared_obj::try_find_shared_object(0) == NULL);
#endif /*LOG_CHECKED*/
    }
#endif  /*LOG_SHARED*/

#if LOG_MULTITHREADED
    // Send termination signal
    mt::atomic_increment(&mt_terminating_);
    mt::fire_event(&write_event_);

    mt::thread_join(&log_thread_handle_);

    mt::close_event(&write_event_);
    mt::close_event(&buffer_devastated_);
#endif /*LOG_MULTITHREADED*/

    // Notify outputs about flush and close
    std::vector<shared_ptr<logger_output_plugin_interface> > output_plugins;
    plugin_mgr_->query_plugins(kLogPluginTypeOutput, output_plugins);


    for (std::vector<shared_ptr<logger_output_plugin_interface> >::const_iterator it = output_plugins.cbegin();
      it != output_plugins.cend(); it++) {
      (*it)->flush();
      (*it)->close();
    }

    plugin_mgr_->detach_all_plugins();

    delete plugin_mgr_;
    plugin_mgr_ = NULL;

#if LOG_MULTITHREADED
    LOG_MT_MUTEX_DESTROY(&mt_buffer_lock_);
#endif  // LOG_MULTITHREADED
  }

  /**
   * \brief    Load plugins from load section in specified config list
   * \param    Config values list
   * \return   Count of new modules load
   * \detail   Logger will load plugins from parameter 'logger::LoadPlugins = plugin1 plugin2 plugin3:plug3name1', etc.
   *           or from section 'load': 'load:plugin1=1', 'load:plugin1:plug1name=1' (values can be any), etc.
   *           Formula for logger::LoadPlugins is  plugin_type:plugin_name or plugin_type for create plugin without name (default).
   *           Formula for load:  load:plugin_type:plugin_name or load:plugin_type for create plugin without name (default).
   *           If plugin has been loaded already, nothing happens.
   * \note     Currently on plugin creation error it is no reaction: user will be not notified. Need some channel for user notification
   */
  int load_plugins_from_config(const cfg::KeyValueTypeList& config) {
    const std::string kConfigLoggerSectionName = "logger";
    const std::string kConfigLoadPluginsParamName = "LoadPlugins";
    
    int modules_loaded = 0;
    std::vector<shared_ptr<logger_plugin_interface> > new_plugins;

    // process [logger] LoadPlugins=plugin1 plugin2:name plugin3  values
    std::string load_plugins = cfg::get_logcfg_string(config, kConfigLoggerSectionName, std::string(), kConfigLoadPluginsParamName);
    std::vector<std::string> plugins_for_load;
    str::split(load_plugins, plugins_for_load);

    for (size_t i = 0; i < plugins_for_load.size(); i++) {
      std::vector<std::string> key_name;

      if (!plugins_for_load[i].size())
        continue;

      str::split(plugins_for_load[i], key_name, ':');
      std::string key = key_name[0];
      std::string name;

      if (key_name.size() > 1)
        name = key_name[1];

      if (plugin_mgr_->is_plugin_attached(key.c_str(), name.c_str()))
        continue; // loaded already

      shared_ptr<logger_plugin_interface> new_plugin = plugin_mgr_->create_and_attach_plugin_from_factory(key, name);

      if (new_plugin) {
        new_plugins.push_back(new_plugin);
        ++modules_loaded;
      } else {
        //TODO: notify user that plugin cannot be loaded
      }
    }

    // process [load] section
    for (cfg::KeyValueTypeList::const_iterator it = config.cbegin(); it != config.cend(); it++) {
      const std::string kPluginLoadSectionPrefix = "load:";

      if (!str::starts_with(it->first, kPluginLoadSectionPrefix))
        continue;

      if (atoi(it->second.c_str()) == 0)
        continue; // do not load

      std::string key_name = it->first.substr(kPluginLoadSectionPrefix.size());

      std::string key;
      std::string name;

      // split 'key:name' or 'key' to separated strings
      std::vector<std::string> key_name_split;
      str::split(key_name, key_name_split, ':');
      if (key_name_split.size() == 2) {
        key = key_name_split[0];
        name = key_name_split[1];
      } else if (key_name_split.size() == 1) {
        key = key_name_split[0];
      }
      else continue; // error

      if (plugin_mgr_->is_plugin_attached(key.c_str(), name.c_str()))
        continue; // loaded already

      shared_ptr<logger_plugin_interface> new_plugin = plugin_mgr_->create_and_attach_plugin_from_factory(key, name);
      if (new_plugin) {
        new_plugins.push_back(new_plugin);
        ++modules_loaded;
      } else {
        //TODO: notify user that plugin cannot be loaded
      }
    }

    // update config and register all new plugins
    for (size_t i = 0; i < new_plugins.size(); i++) {
      shared_ptr<logger_plugin_interface> plugin_interface = new_plugins[i];

      plugin_interface->config_updated(config_);

      // register plugin as command
      if (plugin_interface->plugin_type() == kLogPluginTypeCommand) {
        shared_ptr<logger_command_plugin_interface> cmd_plugin = dynamic_pointer_cast<logger_command_plugin_interface>(plugin_interface);
        if (cmd_plugin) {
          register_command_plugin<logger_command_plugin_interface>(cmd_plugin);
        }
      }

      if (plugin_interface->plugin_type() == kLogPluginTypeArgsCommand) {
        shared_ptr<logger_args_command_plugin_interface> cmd_plugin = dynamic_pointer_cast<logger_args_command_plugin_interface>(plugin_interface);
        if (cmd_plugin) {
          register_command_plugin<logger_args_command_plugin_interface>(cmd_plugin);
        }
      }

    }

    return modules_loaded;
  }

  void decode_logger_config() {
    const std::string kLoggerConfigTypeName = "logger";

    log_config_.log_sys_info_ = cfg::get_logcfg_int(config_, kLoggerConfigTypeName, std::string(), "LogSysInfo", 1) ? true : false;
    log_config_.filter_verbose_level_ = cfg::get_logcfg_int(config_, kLoggerConfigTypeName, std::string(), "Verbose", logger_verbose_all);
    log_config_.header_format_ = cfg::get_logcfg_string(config_, kLoggerConfigTypeName, std::string(), "HdrFormat", default_hdr_format);
    log_config_.process_macro_in_log_text_ = cfg::get_logcfg_int(config_, kLoggerConfigTypeName, std::string(), "ProcessMacroInLogText", 0) ? true : false;
    log_config_.load_plugins_from_config_ = cfg::get_logcfg_int(config_, kLoggerConfigTypeName, std::string(), "LoadPluginsFromConfig", 1) ? true : false;
    log_config_.load_plugins_ = cfg::get_logcfg_string(config_, kLoggerConfigTypeName, std::string(), "LoadPlugins", std::string());
  }

  void reload_config_from_plugins() {
    std::vector<shared_ptr<logger_config_plugin_interface> > config_plugins;
    plugin_mgr_->query_plugins<logger_config_plugin_interface>(kLogPluginTypeConfig, config_plugins);

    for (std::vector<shared_ptr<logger_config_plugin_interface> >::const_iterator it = config_plugins.cbegin();
      it != config_plugins.cend(); it++) {
        (*it)->reload(config_);
    }
  }

  void update_config() {
    do {
      // override all config values from user code config because it has highest priority
      for (cfg::KeyValueTypeList::iterator it = user_code_config_.begin(); it != user_code_config_.end(); it++) {
        it->second = process_config_macros_value(it->second);
        cfg::set_value(config_, it->first, it->second);
      }

      // process macroses
      for (cfg::KeyValueTypeList::iterator it = config_.begin(); it != config_.end(); it++) {
        it->second = process_config_macros_value(it->second);
      }

      // update global logger config firstly
      decode_logger_config();

      int modules_loaded;

      // process load section
      if (log_config_.load_plugins_from_config_) { // load plugins from all configs include ini files, registry, etc
        modules_loaded = load_plugins_from_config(config_);
      }
      else { // load plugins only from user-code config
        modules_loaded = load_plugins_from_config(user_code_config_);
      }

      if (!modules_loaded)
        break;
      else {
        reload_config_from_plugins();
      }
    } while (true);

    // Notify all plugins that config has been reloaded
    std::vector<shared_ptr<logger_plugin_interface> > plugins;
    plugin_mgr_->get_all_plugins(plugins);

    for (size_t i = 0; i < plugins.size(); i++)
      plugins[i]->config_updated(config_);
  }

  void reload_config() {
    reload_config_from_plugins();
    update_config();
  }

  void dump_state(int verb_level) {
    put_to_stream(verb_level, std::string(), "*** Logger actual config ***");

    for (cfg::KeyValueTypeList::iterator it = config_.begin(); it != config_.end(); it++) {
      std::string s = it->first + std::string(" = ") + it->second;
      put_to_stream(verb_level, std::string(), s);
    }

    put_to_stream(verb_level, std::string(), "*** Logger loaded plugins ***");

    std::vector<shared_ptr<logger_plugin_interface> > plugins;
    plugin_mgr_->get_all_plugins(plugins);
    for (size_t i = 0; i < plugins.size(); i++) {
      shared_ptr<logger_plugin_interface> plugin = plugins[i];

      std::string s = str::stringformat("Plugin Name: %s, Instance name: %s, Plugin Type: %d, Addr: %.8X",
          plugin->type(), plugin->name(),
          plugin->plugin_type(), plugin.get());

      put_to_stream(verb_level, std::string(), s);
    }

    put_to_stream(verb_level, std::string(), "*** Logger plugins factories ***");

    std::vector<logger_plugin_factory_interface*> plugin_factories;
    plugin_mgr_->get_all_plugin_factories(plugin_factories);

    for (std::vector<logger_plugin_factory_interface*>::const_iterator it = plugin_factories.cbegin(); it != plugin_factories.cend(); it++) {
      std::string s = str::stringformat("Plugin Name: %s, Plugin Type: %d", (*it)->type(), (*it)->plugin_type());
      put_to_stream(verb_level, std::string(), s);
    }

    put_to_stream(verb_level, std::string(), "*** Logger status ends ***");
  }


  /**
  * \brief   Log text information method
  * \param   verb_level     Verbose level
  * \param   addr           Caller address
  * \param   function_name  Caller function name
  * \param   source_file    Caller source file name
  * \param   line_number    Caller source file line number
  * \param   format         Format string
  * \param   ...            Format arguments
  */
  void LOG_CDECL log(int verb_level, void* addr, const char* function_name,
                     const char* source_file, int line_number, const char* format, ...) LOG_METHOD_OVERRIDE {
    if (!is_message_enabled(verb_level)) return;

    va_list arguments;
    va_start(arguments, format);
    log_args(verb_level, addr, function_name, source_file, line_number, format, arguments);
    va_end(arguments);
  }

  /**
   * \brief    Log format with va_list arguments
   * \param    verb_level    Verbose level
   * \param    addr           Caller address
   * \param    function_name  Caller function name
   * \param    src_file       Caller source file
   * \param    line_num       Caller source file line number
   * \param    format         Format string
   * \param    arguments      va_list arguments
   */
  void log_args(int verb_level, void* addr, const char* function_name,
                const char* src_file, int line_num, const char* format,
                va_list arguments) LOG_METHOD_OVERRIDE {
    if (!is_message_enabled(verb_level)) return;

    std::string module_name = try_get_module_name_fast(addr);
    std::string header =
        log_process_macros(log_config_.header_format_, verb_level, src_file,
                           function_name, line_num, module_name.c_str());
    std::string result;

    bool process_macro_in_log_text = log_config_.process_macro_in_log_text_;

    if (process_macro_in_log_text) {
      std::string text = log_process_macroses_nocache(
        format, verb_level, src_file, function_name, line_num, module_name.c_str());
      str::format_arguments_list(result, text.c_str(), arguments);
    }
    else {
      str::format_arguments_list(result, format, arguments);
    }

    put_to_stream(verb_level, header, result);
  }

  void log_cmd(int cmd_id, int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number, const void* vparam, int iparam) LOG_METHOD_OVERRIDE {

    std::map<int, shared_ptr<logger_plugin_interface> >::const_iterator cmd_it = cmd_refs_.find(cmd_id);
    if (cmd_it == cmd_refs_.end())
      return;

    shared_ptr<logger_command_plugin_interface> cmd_plugin = dynamic_pointer_cast<logger_command_plugin_interface>(cmd_it->second);
    if (!cmd_plugin)
      return;

    std::string module_name = try_get_module_name_fast(addr);
    std::string header =
      log_process_macros(log_config_.header_format_, verb_level,
        source_file, function_name, line_number, module_name.c_str());
    
    std::string result;
    bool ret = cmd_plugin->cmd(result, cmd_id, verb_level, addr, vparam, iparam);
    if (ret) {
      put_to_stream(verb_level, header, result);
    }
  }

  void log_cmd_args(int cmd_id, int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number, const void* vparam, int iparam, va_list args) {

    std::map<int, shared_ptr<logger_plugin_interface> >::const_iterator cmd_it = cmd_refs_.find(cmd_id);
    if (cmd_it == cmd_refs_.end())
      return;

    shared_ptr<logger_args_command_plugin_interface> args_cmd_plugin = dynamic_pointer_cast<logger_args_command_plugin_interface>(cmd_it->second);
    if (!args_cmd_plugin)
      return;

    std::string module_name = try_get_module_name_fast(addr);
    std::string header =
      log_process_macros(log_config_.header_format_, verb_level,
        source_file, function_name, line_number, module_name.c_str());

    std::string result;
    bool ret = args_cmd_plugin->cmd_args(result, cmd_id, verb_level, addr, vparam, iparam, args);
    if (ret) {
      put_to_stream(verb_level, header, result);
    }

  }

  void LOG_CDECL log_cmd_vargs(int cmd_id, int verb_level, void* addr, const char* function_name,
    const char* source_file, int line_number, const void* vparam, int iparam, ...) {
    //TODO: TBD  
  }

#if !LOG_USE_MODULEDEFINITION
  LOG_INLINE std::string try_get_module_name_fast(void* ptr) {
    (void)ptr;
    return std::string();
  }
#endif  //! LOG_USE_MODULEDEFINITION


#if LOG_USE_MODULEDEFINITION

#if LOG_USE_MODULES_CACHE
  logger_modules_cache modules_cache_;
#endif  // LOG_USE_MODULES_CACHE

  std::string try_get_module_name_fast(void* ptr) {
#if LOG_USE_MODULES_CACHE
    std::string result = modules_cache_.get_module_name_by_addr(ptr);
    if (result.size())
      return result;

    // rescan all modules
    std::list<module_entry_t> modules;
    if (module_definition::query_module_list(modules)) {
      modules_cache_.clear();

      for (std::list<module_entry_t>::iterator i = modules.begin(); i != modules.end();
           i++) {
        modules_cache_.add_module_to_cache(i->base_address, i->base_address + i->size, i->module_name);
      }
    }
#endif  // LOG_USE_MODULES_CACHE

    return module_definition::module_name_by_addr(ptr);
  }
#endif  // LOG_USE_MODULEDEFINITION


 private:
#if LOG_USE_MACRO_HEADER_CACHE
  log_macro_cache_t cache;
#endif  // LOG_USE_MACRO_HEADER_CACHE

  std::string log_process_macroses_nocache(std::string format, 
                                           int verbose, 
                                           const char* src_file,
                                           const char* function_name,
                                           int line_num,
                                           const char* module_name) {
#if LOG_USE_MACRO_HEADER_CACHE
    std::string result = log_process_macros_setlen(format, module_name);
    return log_process_macros_often(result, verbose, src_file, function_name, line_num);
#else   // LOG_USE_MACRO_HEADER_CACHE
    return log_process_macros(result, verbose, src_file, function_name, line_num,
                              module_name);
#endif  // LOG_USE_MACRO_HEADER_CACHE
  }

  std::string log_process_macros(std::string format, int verb_level, 
                                 const char* src_file, const char* function_name,
                                 int line_num, const char* module_name) {
    std::string result;

#if LOG_USE_MACRO_HEADER_CACHE
    bool recreate = false;
    bool update_hdr_format = false;

    cache.cache_lock_.read_lock();

    if (log_config_.header_format_ != cache.processed_cached_hdr_format ||
        log_config_.filter_verbose_level_ != cache.processed_cached_global_verb_level) {
      recreate = true;
      update_hdr_format = true;
    }

    unsigned long pid = utils::get_process_id(), tid = utils::get_thread_id();

    int millisec;
    struct tm newtime = utils::get_time(millisec);

    if (!recreate && cache.valid_ && cache.processed_cached_hdr_partial.size() &&
        pid == cache.processed_cached_pid &&
        newtime.tm_year == cache.processed_cached_tm.tm_year &&
        newtime.tm_mon == cache.processed_cached_tm.tm_mon &&
        newtime.tm_mday == cache.processed_cached_tm.tm_mday &&
        newtime.tm_hour == cache.processed_cached_tm.tm_hour &&
        newtime.tm_min == cache.processed_cached_tm.tm_min &&
        !strcmp(module_name, cache.processed_cached_module_name.c_str())) {
      if (tid == cache.processed_cached_tid &&
          newtime.tm_sec == cache.processed_cached_tm.tm_sec &&
          millisec == cache.processed_cached_millitm &&
          line_num == cache.processed_cached_line_num &&
          !strcmp(cache.processed_cached_function_name.c_str(), function_name) &&
          verb_level == cache.processed_cached_verb_level) {
        result = cache.processed_cached_hdr_full;
      } else {
        result = log_process_macros_often(cache.processed_cached_hdr_partial, verb_level,
                                          src_file, function_name, line_num);
      }

      cache.cache_lock_.read_unlock();
      cache.cache_lock_.write_lock();

      if (update_hdr_format) {
        cache.processed_cached_hdr_format = log_config_.header_format_;
        cache.processed_cached_global_verb_level = log_config_.filter_verbose_level_;
      }

      cache.processed_cached_millitm = millisec;
      cache.processed_cached_hdr_full = result;
      cache.processed_cached_src_file = src_file;
      cache.processed_cached_function_name = function_name;
      cache.processed_cached_line_num = line_num;
      cache.processed_cached_tm = newtime;
      cache.processed_cached_verb_level = verb_level;
      cache.processed_cached_pid = pid;
      cache.processed_cached_tid = tid;

      cache.cache_lock_.write_unlock();
      return result;
    }
#endif  // LOG_USE_MACRO_HEADER_CACHE

    result = log_process_macros_setlen(format, module_name);

#if LOG_USE_MACRO_HEADER_CACHE
    cache.cache_lock_.read_unlock();
    cache.cache_lock_.write_lock();

    if (update_hdr_format) {
      cache.processed_cached_hdr_format = log_config_.header_format_;
      cache.processed_cached_global_verb_level = log_config_.filter_verbose_level_;
    }

    cache.processed_cached_hdr_partial = result;
#endif  // LOG_USE_MACRO_HEADER_CACHE

    result = log_process_macros_often(result, verb_level, src_file, function_name, line_num);

#if LOG_USE_MACRO_HEADER_CACHE
    cache.processed_cached_hdr_full = result;
    cache.processed_cached_module_name = module_name;
    cache.processed_cached_millitm = millisec;
    cache.processed_cached_hdr_full = result;
    cache.processed_cached_src_file = src_file;
    cache.processed_cached_function_name = function_name;
    cache.processed_cached_line_num = line_num;
    cache.processed_cached_tm = newtime;
    cache.processed_cached_verb_level = verb_level;
    cache.processed_cached_pid = pid;
    cache.processed_cached_tid = tid;

    cache.valid_ = true;

    cache.cache_lock_.write_unlock();
#endif  // LOG_USE_MACRO_HEADER_CACHE

    return result;
  }

  static std::string log_process_macros_setlen(std::string format, const char* module_name) {
    const char* format_str = format.c_str();

    bool macro_yyyy = str::contains(format_str, "$(yyyy)");
    bool macro_yy = str::contains(format_str, "$(yy)");
    bool macro_MM = str::contains(format_str, "$(MM)");
    bool macro_M = str::contains(format_str, "$(M)");
    bool macro_dd = str::contains(format_str, "$(dd)");
    bool macro_d = str::contains(format_str, "$(d)");
    bool macro_hh = str::contains(format_str, "$(hh)");
    bool macro_h = str::contains(format_str, "$(h)");
    bool macro_mm = str::contains(format_str, "$(mm)");
    bool macro_m = str::contains(format_str, "$(m)");

    bool macro_module = str::contains(format_str, "$(module)");
    bool macro_MODULE = str::contains(format_str, "$(MODULE)");

    bool macro_PID = str::contains(format_str, "$(PID)");

    if (macro_yyyy || macro_yy || macro_MM || macro_M || macro_dd || macro_d ||
        macro_hh || macro_h || macro_mm || macro_m || macro_PID) {
      int millisec;
      struct tm newtime = utils::get_time(millisec);

      if (macro_yyyy)
        format = str::replace(format, "$(yyyy)", str::stringformat("%.4d", newtime.tm_year + 1900));
      if (macro_yy)
        format = str::replace(format, "$(yy)", str::stringformat("%.2d", newtime.tm_year - 100));
      if (macro_M)
        format = str::replace(format, "$(M)", str::stringformat("%d", newtime.tm_mon + 1));
      if (macro_MM)
        format = str::replace(format, "$(MM)", str::stringformat("%.2d", newtime.tm_mon + 1));
      if (macro_d) format = str::replace(format, "$(d)", str::stringformat("%d", newtime.tm_mday));
      if (macro_dd)
        format = str::replace(format, "$(dd)", str::stringformat("%.2d", newtime.tm_mday));
      if (macro_h) format = str::replace(format, "$(h)", str::stringformat("%d", newtime.tm_hour));
      if (macro_hh)
        format = str::replace(format, "$(hh)", str::stringformat("%.2d", newtime.tm_hour));
      if (macro_m) format = str::replace(format, "$(m)", str::stringformat("%d", newtime.tm_min));
      if (macro_mm)
        format = str::replace(format, "$(mm)", str::stringformat("%.2d", newtime.tm_min));

      if (macro_PID)
        format = str::replace(format, "$(PID)", str::stringformat("%d", utils::get_process_id()));
    }

    if ((macro_module || macro_MODULE) && strlen(module_name)) {
      if (macro_MODULE) format = str::replace(format, "$(MODULE)", module_name);

      if (macro_module) {
        std::string resultName = module_name;
        size_t position = resultName.find_last_of("\\/");
        if (position == resultName.npos)
          position = 0;
        else
          position++;

        format = str::replace(format, "$(module)", resultName.substr(position));
      }
    }
    return format;
  }

  /**
   * \brief    Process log header macroses which used often
   * \param    format         Format string
   * \param    verb_level     Verbose level
   * \param    src_file       Source file name
   * \param    function_name  Function name
   * \param    line_num       Source file line number
   */
  static std::string log_process_macros_often(std::string format, int verb_level, 
                                       const char* src_file, const char* function_name, int line_num) {
    const char* format_str = format.c_str();

    bool macro_ss = str::contains(format_str, "$(ss)");
    bool macro_s = str::contains(format_str, "$(s)");
    bool macro_ttt = str::contains(format_str, "$(ttt)");
    bool macro_t = str::contains(format_str, "$(t)");

    bool macro_srcfile = str::contains(format_str, "$(srcfile)");
    bool macro_line = str::contains(format_str, "$(line)");
    bool macro_function = str::contains(format_str, "$(function)");

    bool macro_V = str::contains(format_str, "$(V)");
    bool macro_v = str::contains(format_str, "$(v)");

    bool macro_tid = str::contains(format_str, "$(TID)");

    if (macro_ss || macro_s || macro_ttt || macro_t) {
      int millisec;
      struct tm newtime = utils::get_time(millisec);

      if (macro_s) format = str::replace(format, "$(s)", str::stringformat("%d", newtime.tm_sec));
      if (macro_ss)
        format = str::replace(format, "$(ss)", str::stringformat("%.2d", newtime.tm_sec));
      if (macro_t) format = str::replace(format, "$(t)", str::stringformat("%d", millisec));
      if (macro_ttt) format = str::replace(format, "$(ttt)", str::stringformat("%.3d", millisec));
    }

    if (macro_V) format = str::replace(format, "$(V)", get_verbose_string(verb_level));
    if (macro_v) format = str::replace(format, "$(v)", str::stringformat("%d", verb_level));

    if (macro_srcfile && src_file)
      format = str::replace(format, "$(srcfile)", src_file);

    if (macro_line && line_num >= 0)
      format = str::replace(format, "$(line)", str::stringformat("%d", line_num));

    if (macro_function && function_name)
      format = str::replace(format, "$(function)", function_name);

    if (macro_tid) {
      unsigned long tid = utils::get_thread_id();
      format = str::replace(format, "$(TID)", str::stringformat("%d", tid));
    }

    return format;
  }

  /**
   * \brief    Check is message enabled with specified verbose level
   * \param    verb_level    Verbose level value
   * \return   true - message is enabled for output, false - disabled
   */
  LOG_INLINE bool is_message_enabled(int verb_level) const {
    return (log_config_.filter_verbose_level_ & verb_level) ? true : false;
  }

  /**
   * \brief    Get verbose level value string description
   * \param    verb_level      Verbose level value
   * \return   Pointer to constant zero-ended string with description
   */
  static const char* get_verbose_string(int verb_level) {
    switch (verb_level) {
      case logger_verbose_mute:
        return "MUTE";
      case logger_verbose_fatal:
        return "FATAL";
      case logger_verbose_error:
        return "ERROR";
      case logger_verbose_info:
        return "INFO";
      case logger_verbose_warning:
        return "WARNING";
      default:
      case logger_verbose_debug:
        return "DEBUG";
    }
  }
}; // class logger



}//namespace detail
}//namespace logging


#endif /*LOGGER_IMPL_HEADER*/
