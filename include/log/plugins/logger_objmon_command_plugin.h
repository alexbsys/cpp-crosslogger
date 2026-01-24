
#ifndef LOGGER_OBJMON_HEADER
#define LOGGER_OBJMON_HEADER

#include <log/logger_config.h>
#include <log/logger_pdetect.h>
#include <log/logger_pdefs.h>
#include <log/logger_interfaces.h>
#include <log/logger_sysinclib.h>

#include <log/detail/logger_strutils.h>
#include <log/detail/logger_mt.h>
#include <log/detail/logger_utils.h>

#include <log/detail/logger_runtime_debugging.h>
#include <log/detail/logger_cfgfn.h>

#include <sstream>
#include <map>

namespace logging {

class logger_objmon_command_plugin : public logger_command_plugin_interface {
public:
  // User call for get extended objects information for object dump
  typedef int(*UserObjExtendedInfoCallType)(
      void* obj_ptr, const char* type_name, const struct tm* created, int created_ms, const char* creation_trace, char* out_ext_message);

  static const int kMaxOutExtMessageLength = 1024;

  const int kObjectMonitorRegisterCommandId = 0x1010;
  const int kObjectMonitorUnregisterCommandId = 0x1011;
  const int kObjectMonitorDumpCommandId = 0x1012;
  const int kObjectMonitorSetDumpObjectInfoFn = 0x1013;
  const int kObjectMonitorGetRegisteredObjectsCount = 0x1014;
  const int kObjectMonitorDumpToConsoleCommandId = 0x1015;

  logger_objmon_command_plugin(const char* name = NULL)
    : plugin_name_(name ? name : "")
    , use_stacktrace_(false)
    , user_obj_extended_info_call_ptr_(nullptr) {}

  virtual ~logger_objmon_command_plugin() LOG_METHOD_OVERRIDE {}

  const char* type() const LOG_METHOD_OVERRIDE { return "objmon_cmd"; }

  const char* name() const LOG_METHOD_OVERRIDE { return plugin_name_.c_str(); }

  void get_cmd_ids(int* out_cmd_ids, int max_cmds) const LOG_METHOD_OVERRIDE {
    if (max_cmds > 6) {
      out_cmd_ids[0] = kObjectMonitorRegisterCommandId;
      out_cmd_ids[1] = kObjectMonitorUnregisterCommandId;
      out_cmd_ids[2] = kObjectMonitorDumpCommandId;
      out_cmd_ids[3] = kObjectMonitorSetDumpObjectInfoFn;
      out_cmd_ids[4] = kObjectMonitorGetRegisteredObjectsCount;
      out_cmd_ids[5] = kObjectMonitorDumpToConsoleCommandId;
    }
  }

  bool cmd(std::string& out_result, int cmd_id, int verb_level, void* addr, const void* vparam, int iparam) LOG_METHOD_OVERRIDE {
    (void)iparam;
    (void)addr;
    (void)verb_level;

    std::stringstream out_stream;
    bool processed = false;

    if (cmd_id == kObjectMonitorRegisterCommandId) {
      const struct { unsigned int hash; const char* type_name; void* ptr; } *param_ptr;
      reinterpret_cast<const unsigned char*&>(param_ptr) = reinterpret_cast<const unsigned char*>(vparam);
      objmon_register(out_stream, param_ptr->hash, param_ptr->type_name, param_ptr->ptr);
      processed = true;
    }

    if (cmd_id == kObjectMonitorUnregisterCommandId) {
      const struct { unsigned int hash; void* ptr; } *param_ptr;
      reinterpret_cast<const unsigned char*&>(param_ptr) = reinterpret_cast<const unsigned char*>(vparam);
      objmon_unregister(out_stream, param_ptr->hash, param_ptr->ptr);
      processed = true;
    }

    if (cmd_id == kObjectMonitorDumpCommandId) {
      objmon_dump(out_stream);
      processed = true;
    }

    if (cmd_id == kObjectMonitorDumpToConsoleCommandId) {
      objmon_dump(out_stream);
      std::cout << out_stream.str() << std::endl;
      out_stream.clear();
      processed = true;
    }

    if (cmd_id == kObjectMonitorSetDumpObjectInfoFn) {
      UserObjExtendedInfoCallType call_ptr = (UserObjExtendedInfoCallType)(vparam);
      objmon_set_user_object_extended_info_call(call_ptr);
      processed = true;
    }

    if (cmd_id == kObjectMonitorGetRegisteredObjectsCount) {
      int* presult = (int*)vparam;
      int objects_count = objmon_get_registered_objects_count();
      if (presult)
        *presult = objects_count;
      processed = true;
    }

    out_result = out_stream.str();
    return processed;
  }

  virtual bool attach(logger_interface* logger) LOG_METHOD_OVERRIDE {
    (void)logger;

    LOG_MT_MUTEX_INIT(&mutex_objmon_, NULL);
    return true;
  }

  virtual void detach(logger_interface* logger) LOG_METHOD_OVERRIDE {
    (void)logger;

    LOG_MT_MUTEX_DESTROY(&mutex_objmon_);
  }

  void config_updated(const logging::cfg::KeyValueTypeList& config) LOG_METHOD_OVERRIDE {
    config_sym_path_ = detail::cfg::get_logcfg_string(config, "runtime_debugging", std::string(), "SymPath", ".");
    use_stacktrace_ = detail::cfg::get_logcfg_int(config, "objmon", std::string(), "UseStacktrace", 0);
  }

protected:
  void objmon_register(
      std::stringstream& out_stream,
      size_t hash_code,
      const char* type_name,
      void* ptr) {

    std::string stacktrace;
    if (use_stacktrace_) {
#ifdef LOG_PLATFORM_WINDOWS
      detail::runtime_debugging::get_current_stack_trace_string(&stacktrace, &config_sym_path_, 3);
#else /*LOG_PLATFORM_WINDOWS*/
      detail::runtime_debugging::get_current_stack_trace_string(&stacktrace, 3);
#endif /*LOG_PLATFORM_WINDOWS*/
    }

    obj_info_type obj_info;
    obj_info.ptr_ = ptr;
    obj_info.type_name_ = type_name;
    obj_info.stacktrace_ = stacktrace;
    obj_info.created_time_ = logging::detail::utils::get_time(obj_info.created_time_ms_);
    obj_info.thread_id_ = logging::detail::utils::get_thread_id();

    lock_objmon();

    if (objmon_objs_.find(hash_code) == objmon_objs_.end()) {
      objmon_objs_.insert(
        std::pair<size_t, std::list<obj_info_type> >(hash_code, std::list<obj_info_type>()));
    }

    objmon_objs_[hash_code].push_back(obj_info);

    unlock_objmon();
    out_stream << detail::str::stringformat("[objmon] register instance %p, %s", obj_info.ptr_, obj_info.type_name_);
  }

  void objmon_unregister(std::stringstream& out_stream, size_t hash_code, void* ptr) {
    lock_objmon();

    if (objmon_objs_.find(hash_code) == objmon_objs_.end()) {
      objmon_objs_.insert(
        std::pair<size_t, std::list<obj_info_type> >(hash_code, std::list<obj_info_type>()));
    }

    std::list<obj_info_type>& obj_ptrs = objmon_objs_[hash_code];

    auto it = std::find_if(obj_ptrs.begin(), obj_ptrs.end(),
      [ptr](const obj_info_type& obj_info) { return obj_info.ptr_ == ptr; });

    if (it != obj_ptrs.end()) {
      out_stream << detail::str::stringformat("[objmon] delete instance %p, %s. Created tid=%lu %d/%d/%d %d:%d:%d.%d", it->ptr_, it->type_name_,
                                              it->thread_id_,
                                              it->created_time_.tm_year + 1900, it->created_time_.tm_mon+1, it->created_time_.tm_mday,
                                              it->created_time_.tm_hour, it->created_time_.tm_min, it->created_time_.tm_sec, it->created_time_ms_);
      if (use_stacktrace_) {
        out_stream << ", stacktrace:" << std::endl << it->stacktrace_ << std::endl;
      }

      obj_ptrs.erase(it);
    }
    else {
      out_stream << detail::str::stringformat("[objmon] delete request, but object not found. %p", ptr);
    }

    unlock_objmon();
  }

  void objmon_dump(std::stringstream& out_stream) {
    lock_objmon();

    char* extended_info_buf = nullptr;

    if (user_obj_extended_info_call_ptr_) {
      extended_info_buf = reinterpret_cast<char*>(malloc(kMaxOutExtMessageLength + 1));
    }

    out_stream << "*** OBJECT LIST ***" << std::endl;
    for (auto it = objmon_objs_.begin(); it != objmon_objs_.end(); it++) {
      if (it->second.size() == 0) continue;

      out_stream << " ** OBJECT TYPE: " << it->first << ", " << it->second.begin()->type_name_
        << std::endl;

      for (const obj_info_type& obj_info : it->second) {
        out_stream << "++  ptr: " << detail::str::stringformat("%.8X", obj_info.ptr_) << ", created tid=" << obj_info.thread_id_ << " "
                   << obj_info.created_time_.tm_year + 1900 << "/" << obj_info.created_time_.tm_mon + 1 << "/" << obj_info.created_time_.tm_mday
                   << " " << obj_info.created_time_.tm_hour << ":" << obj_info.created_time_.tm_min << ":" << obj_info.created_time_.tm_sec << "." <<obj_info.created_time_ms_;

        if (use_stacktrace_) {
          out_stream << ", stacktrace \n" << obj_info.stacktrace_ << std::endl;
        }

        if (user_obj_extended_info_call_ptr_) {

          if (extended_info_buf)
            memset(extended_info_buf, 0, kMaxOutExtMessageLength+1);

          int r = user_obj_extended_info_call_ptr_(
                obj_info.ptr_,
                it->second.begin()->type_name_,
                &obj_info.created_time_,
                obj_info.created_time_ms_,
                obj_info.stacktrace_.c_str(),
                extended_info_buf);

          if (r > 0) {
            out_stream << "  Extended obj info: " << extended_info_buf << std::endl;
          }
        }

        out_stream << std::endl;
      }
    }

    if (extended_info_buf) {
      free(extended_info_buf);
      extended_info_buf = nullptr;
    }

    unlock_objmon();
  }

  int objmon_get_registered_objects_count() const {
    lock_objmon();
    int obj_count = 0;

    for (auto it = objmon_objs_.begin(); it != objmon_objs_.end(); it++) {
      obj_count += static_cast<int>(it->second.size());
    }

    unlock_objmon();

    return obj_count;
  }

  void objmon_set_user_object_extended_info_call(UserObjExtendedInfoCallType call_ptr) {
    lock_objmon();
    user_obj_extended_info_call_ptr_ = call_ptr;
    unlock_objmon();
  }

private:
  LOG_INLINE void lock_objmon() const {
    LOG_MT_MUTEX_LOCK(&mutex_objmon_);
  }

  LOG_INLINE void unlock_objmon() const {
    LOG_MT_MUTEX_UNLOCK(&mutex_objmon_);
  }

  struct obj_info_type {
    void* ptr_;              /// Pointer to object
    const char* type_name_;  /// Object type name
    std::string stacktrace_;
    struct tm created_time_;
    int created_time_ms_;
    unsigned long thread_id_;
  };

  std::map<size_t, std::list<obj_info_type> > objmon_objs_; /// Objects storage (hashcode,object info)
  mutable LOG_MT_MUTEX mutex_objmon_;
  std::string plugin_name_;
  std::string config_sym_path_;
  bool use_stacktrace_;
  UserObjExtendedInfoCallType user_obj_extended_info_call_ptr_;
};

class logger_objmon_command_plugin_factory : public logger_plugin_default_factory<logger_objmon_command_plugin> {
public:
  logger_objmon_command_plugin_factory()
    : logger_plugin_default_factory<logger_objmon_command_plugin>("objmon_cmd", kLogPluginTypeCommand) {}
  virtual ~logger_objmon_command_plugin_factory() LOG_METHOD_OVERRIDE {}
};

}//namespace logging




#endif /*LOGGER_OBJMON_HEADER*/
