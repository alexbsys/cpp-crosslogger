
#ifndef LOGGER_OBJMON_HEADER
#define LOGGER_OBJMON_HEADER

#include <log/logger_config.h>
#include <log/logger_pdetect.h>
#include <log/logger_pdefs.h>
#include <log/logger_interfaces.h>
#include <log/logger_sysinclib.h>

#include <log/detail/logger_strutils.h>
#include <log/detail/logger_mt.h>

#include <sstream>
#include <map>

namespace logging {

class logger_objmon_command_plugin : public logger_command_plugin_interface {
public:
  const int kObjectMonitorRegisterCommandId = 0x1010;
  const int kObjectMonitorUnregisterCommandId = 0x1011;
  const int kObjectMonitorDumpCommandId = 0x1012;

  logger_objmon_command_plugin(const char* name = NULL) : plugin_name_(name ? name : "") {}
  virtual ~logger_objmon_command_plugin() LOG_METHOD_OVERRIDE {}

  const char* type() const LOG_METHOD_OVERRIDE { return "objmon_cmd"; }

  const char* name() const LOG_METHOD_OVERRIDE { return plugin_name_.c_str(); }

  void get_cmd_ids(int* out_cmd_ids, int max_cmds) const LOG_METHOD_OVERRIDE {
    if (max_cmds > 3) {
      out_cmd_ids[0] = kObjectMonitorRegisterCommandId;
      out_cmd_ids[1] = kObjectMonitorUnregisterCommandId;
      out_cmd_ids[2] = kObjectMonitorDumpCommandId;
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

protected:
  void objmon_register(std::stringstream& out_stream, size_t hash_code, const char* type_name, void* ptr) {
    obj_info_type obj_info;
    obj_info.ptr_ = ptr;
    obj_info.type_name_ = type_name;

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
      out_stream << detail::str::stringformat("[objmon] delete instance %p, %s\n", it->ptr_, it->type_name_);
      obj_ptrs.erase(it);
    }
    else {
      out_stream << detail::str::stringformat("[objmon] delete request, but object not found. %p\n", ptr);
    }

    unlock_objmon();
  }

  void objmon_dump(std::stringstream& out_stream) {
    lock_objmon();

    out_stream << "*** OBJECT LIST ***" << std::endl;
    for (auto it = objmon_objs_.begin(); it != objmon_objs_.end(); it++) {
      if (it->second.size() == 0) continue;

      out_stream << " ** OBJECT TYPE: " << it->first << ", " << it->second.begin()->type_name_
        << std::endl;

      for (const obj_info_type& obj_info : it->second) {
        out_stream << "  ptr: " << detail::str::stringformat("%.8X", obj_info.ptr_) << std::endl;
      }
    }

    unlock_objmon();
  }

private:
  LOG_INLINE void lock_objmon() {
    LOG_MT_MUTEX_LOCK(&mutex_objmon_);
  }

  LOG_INLINE void unlock_objmon() {
    LOG_MT_MUTEX_UNLOCK(&mutex_objmon_);
  }

  struct obj_info_type {
    void* ptr_;              /// Pointer to object
    const char* type_name_;  /// Object type name
  };

  std::map<size_t, std::list<obj_info_type> > objmon_objs_; /// Objects storage (hashcode,object info)
  LOG_MT_MUTEX mutex_objmon_;
  std::string plugin_name_;
};

class logger_objmon_command_plugin_factory : public logger_plugin_default_factory<logger_objmon_command_plugin> {
public:
  logger_objmon_command_plugin_factory()
    : logger_plugin_default_factory<logger_objmon_command_plugin>("objmon_cmd", kLogPluginTypeCommand) {}
  virtual ~logger_objmon_command_plugin_factory() LOG_METHOD_OVERRIDE {}
};

}//namespace logging




#endif /*LOGGER_OBJMON_HEADER*/
