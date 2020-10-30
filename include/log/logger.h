
// Smart logger library for C/C++
/// alexb 2009-2020

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// LOG MACROSES:
// $(yyyy) $(yy) - year
// $(MM) $(M) - month
// $(dd) $(d) - day
// $(hh) $(h) - hour
// $(mm) $(m) - min
// $(ss) $(s) - sec
// $(ttt) $(t) - milliseconds
// $(V) $(v) - verbose
// $(MODULE) - current module name full
// $(module) - current module name short
// $(function) - function name
// $(line) - line number
// $(srcfile) - source file
// $(PID) - process ID
// $(TID) - thread ID

// CONFIG PATH & NAMES MACROSES:
// $(SYSTEMPATH) - path to Windows\System
// $(WINDOWSPATH) - path to Windows
// $(COMMONAPPDATA) - path to common application data
// $(USERAPPDATA) - path to current user application data
// $(DESKTOPDIR) - path to desktop
// $(TEMPPATH) - path to temp directory
// $(CURRENTDIR) - current process directory
// $(EXEDIR) - path to process EXE
// $(EXEFILENAME) - process EXE file name without extension
// $(EXEFULLFILENAME) - process EXE file name with extension

// only if LOG_USE_MODULEDEFINITION
// $(MODULEDIR) - module directory
// $(MODULEFILENAME) - module file name without extension
// $(MODULEFULLFILENAME) - module file name with extension

// Sample INI file

// [logger]
// LogPath=$(USERAPPDATA)\$(EXEFILENAME)\Log
// Verbose=255
// HeaderFormat=$(V)|$(dd).$(MM).$(yyyy)|$(hh):$(mm):$(ss).$(ttt) ($(module)!$(function))
// LogFileName=$(EXEFILENAME).log
// LogSysInfo=1
// ScrollFileCount=70
// ScrollFileSize=1638400
// ScrollFileEveryRun=1
// RegistryConfigPath=HKCU\Software\$(EXEFILENAME)\Logging

#ifndef __LOGGER_HEADER
#define __LOGGER_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_pdefs.h"

#ifdef LOG_COMPILER_MSVC
#pragma once
#endif /*LOG_COMPILER_MSVC*/


#include "logger_userdefs.h"

#if !LOG_ENABLED

#else  // LOG_ENABLED

#  include "logger_cfgcheck.h"

#  include "logger_varargs.h"
#  include "logger_get_caller_addr.h"

#  include "logger_verbose.h"




#if defined(LOG_CPP) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))
/// Inproc logger implementation

#include "logger_sysinclib.h"

#if LOG_SHARED
#include "logger_shmem.h"
#endif /*LOG_SHARED*/


#include "logger_interfaces.h"

#include "logger_shared_ptr.hpp"

//#define LOG_SET_VERBOSE_LEVEL(l) logging::configurator.set_verbose_level(l)

/*  TODO: remove this after plugins implementation
#if LOG_CONFIGURE_FROM_REGISTRY
#define LOG_SET_REG_CONFIG_PATH(p) logging::configurator.set_reg_config_path(p)
#else  // LOG_CONFIGURE_FROM_REGISTRY
#define LOG_SET_REG_CONFIG_PATH(p)
#endif  // LOG_CONFIGURE_FROM_REGISTRY

#if LOG_INI_CONFIGURATION
#define LOG_SET_INI_CONFIG_PATHS(p) logging::configurator.set_ini_file_find_paths(p)
#else  // LOG_INI_CONFIGURATION
#define LOG_SET_INI_CONFIG_PATHS(p)
#endif  // LOG_INI_CONFIGURATION
*/





#if LOG_USE_OWN_VSNPRINTF
#include "logger_xprintf.h"
#endif //LOG_USE_OWN_VSNPRINTF


#include "logger_strutils.h"

#if LOG_INI_CONFIGURATION
#include "logger_ini_parser.h"
#endif  // LOG_INI_CONFIGURATION

#include "logger_cfgfn.h"


#include "logger_utils.h"

#if LOG_USE_MODULEDEFINITION
#include "logger_moddef.h"
#endif  // LOG_USE_MODULEDEFINITION


#if LOG_CONFIGURE_FROM_REGISTRY && defined(LOG_PLATFORM_WINDOWS)
#include "logger_win_registry.h"

#endif  //(LOG_CONFIGURE_FROM_REGISTRY || LOG_USE_SYSTEMINFO) && defined(LOG_PLATFORM_WINDOWS)


#if LOG_AUTO_DEBUGGING
#include "logger_runtime_debugging.h"
#endif  // LOG_AUTO_DEBUGGING

#if LOG_USE_SYSTEMINFO
#include "logger_sysinfo.h"
#endif  // LOG_USE_SYSTEMINFO

#include "logger_mt.h"

namespace logging {

////////////////////    Helpers     ////////////////////


/// Singleton template implementation
template <typename _TIf, typename _TImpl = _TIf>
class singleton : public logger_singleton_interface<_TIf> {
 public:
  singleton(int (_TIf::*ref_fn)() = NULL, int (_TIf::*deref_fn)() = NULL,
            int (_TIf::*ref_cnt_fn)() const = NULL, _TIf* ptr = NULL, bool need_delete = false)
      : ptr_(NULL),
        need_delete_(false),
        ref_fn_(ref_fn),
        deref_fn_(deref_fn),
        ref_cnt_fn_(ref_cnt_fn) {
    if (ptr) reset(ptr, need_delete);
  }

  ~singleton() { release(); }

  __inline _TIf* operator->() { return get(); }

  static void delete_fn(_TIf* obj, void* user_param) {
    delete obj;
  }

  _TIf* get() {
    if (!ptr_) {
      ptr_ = new _TImpl(&delete_fn, this);
      need_delete_ = true;

      if (ref_fn_) (ptr_->*ref_fn_)();
    }

    return ptr_;
  }

  void reset(_TIf* ptr, bool need_delete = true) {
    release();

    need_delete_ = need_delete;
    ptr_ = ptr;

    if (ref_fn_) (ptr_->*ref_fn_)();
  }

  void release() {
    _TIf* ptr = ptr_;
    bool need_delete = need_delete_;

    ptr_ = NULL;
    need_delete_ = false;

    if (deref_fn_ && ptr) (ptr->*deref_fn_)();

    if (ref_cnt_fn_ && ptr) {
      if ((ptr->*ref_cnt_fn_)()) ptr = NULL;
    }

    if (ptr && need_delete_) delete ptr;
  }

 private:
  _TIf* ptr_;
  bool need_delete_;

  int (_TIf::*ref_fn_)();
  int (_TIf::*deref_fn_)();
  int (_TIf::*ref_cnt_fn_)() const;
};

namespace detail {
  class logger;
}//namespace detail

extern logger_singleton_interface<logger_interface>* _logger;

//////////////////////////////////////////////////////////////

namespace detail {

//////////////////////////////////////////////////////////////

static const char* default_hdr_format =
    "[$(V)] $(dd).$(MM).$(yyyy) $(hh):$(mm):$(ss).$(ttt) [$(PID):$(TID)] "
    "[$(module)!$(function)]";

}//namespace detail

 //LogSysInfo
 //Verbose
 //HeaderFormat

 //LogPath
 //LogFileName
 //ScrollFileCount
 //ScrollFileSize
 //ScrollFileEveryRun
 //RegistryConfigPath


#if LOG_TEST_DO_NOT_WRITE_FILE
#include <sstream>
#undef LOG_FLUSH_FILE_EVERY_WRITE
#define LOG_FLUSH_FILE_EVERY_WRITE 1l
#endif  // LOG_TEST_DO_NOT_WRITE_FILE


//////////////////////////////////////////////////////////////


#if LOG_CONFIGURE_FROM_REGISTRY

class log_registry_configurator {
 public:
  static bool configure(std::string key_path) {
    if (key_path == "") key_path = LOG_REGISTRY_DEFAULT_KEY;

    size_t storage_delim = key_path.find_first_of("\\/");
    if (storage_delim == key_path.npos) return false;

    std::string storage = key_path.substr(0, storage_delim);
    HKEY base_key = log_registry_helper::reg_base_key_by_string(storage);

    std::string path = key_path.substr(storage_delim + 1);

    std::string log_path;
    if (log_registry_helper::get_reg_sz_value(base_key, path, "LogPath", log_path))
      configurator.set_log_path(log_path);

    std::string hdr_format;
    if (log_registry_helper::get_reg_sz_value(base_key, path, "HeaderFormat", hdr_format))
      configurator.set_hdr_format(hdr_format);

    unsigned long verbose_level;
    if (log_registry_helper::get_reg_dword_value(base_key, path, "Verbose",
                                                 verbose_level))
      configurator.set_verbose_level(verbose_level);

#if LOG_USE_SYSTEMINFO
    unsigned long log_sys_info;
    if (log_registry_helper::get_reg_dword_value(base_key, path, "LogSystemInfo",
                                                 log_sys_info))
      configurator.set_need_sys_info(log_sys_info ? true : false);
#endif  // LOG_USE_SYSTEMINFO

    unsigned long log_scroll_file_size;
    if (log_registry_helper::get_reg_dword_value(base_key, path, "ScrollFileSize",
                                                 log_scroll_file_size))
      configurator.set_log_scroll_file_size(log_scroll_file_size);

    unsigned long log_scroll_file_count;
    if (log_registry_helper::get_reg_dword_value(base_key, path, "ScrollFileCount",
                                                 log_scroll_file_count))
      configurator.set_log_scroll_file_count(log_scroll_file_count);

    unsigned long log_scroll_file_every_run;
    if (log_registry_helper::get_reg_dword_value(base_key, path, "ScrollFileEveryRun",
                                                 log_scroll_file_every_run))
      configurator.set_log_scroll_file_every_run(log_scroll_file_every_run ? true
                                                                           : false);

    unsigned long log_enabled;
    if (log_registry_helper::get_reg_dword_value(base_key, path, "LogEnabled",
                                                 log_enabled)) {
      if (!log_enabled) configurator.set_verbose_level(logger_verbose_mute);
    }

    return true;
  }
};

#endif  // LOG_CONFIGURE_FROM_REGISTRY

//////////////////////////////////////////////////////////////



namespace detail {

/**
 * \class    log_stream helper. Implements logger stream interface. User must not use it directly
 */
class log_stream {
public:
  log_stream(logger_interface* logger_obj, int verb_level, void* addr,
    const char* function_name, const char* source_file, int line_number)
    : logger_obj_(logger_obj)
    , verb_level_(verb_level)
    , addr_(addr)
    , function_name_(function_name)
    , source_file_(source_file)
    , line_number_(line_number) {
    logger_obj_->ref();
  }

  log_stream(const log_stream& stream)
    : logger_obj_(stream.logger_obj_)
    , verb_level_(stream.verb_level_)
    , addr_(stream.addr_)
    , function_name_(stream.function_name_)
    , source_file_(stream.source_file_), line_number_(stream.line_number_) {
    ss_.str(stream.ss_.str());
    logger_obj_->ref();
  }

  virtual ~log_stream() {
    logger_obj_->log(verb_level_, addr_, function_name_, source_file_, line_number_, "%s", ss_.str().c_str());
    logger_obj_->deref();
  }

  template<typename T>
  log_stream& operator <<(const T& t) {
    ss_ << t;
    return *this;
  }

private:
  logger_interface* logger_obj_;
  std::stringstream ss_;
  int verb_level_;
  void* addr_;
  const char* function_name_;
  const char* source_file_;
  int line_number_;
};


////////////////////  Logger implementation  //////////////////////////


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


class plugin_manager {
public:
  plugin_manager(logger_interface* base) : base_(base) {
    for (int i = kLogPluginTypeMin; i <= kLogPluginTypeMax; i++) {
      plugins_.insert(std::pair<int, std::list<plugin_data> >(i, std::list<plugin_data>()));
    }
  }

  ~plugin_manager() {
    detach_all_plugins();
  }

  bool register_plugin_factory(logger_plugin_factory_interface* plugin_factory_interface) {
    if (!plugin_factory_interface)
      return false;

    int plugin_type = plugin_factory_interface->plugin_type();
    if (plugin_type < kLogPluginTypeMin || plugin_type > kLogPluginTypeMax)
      return false;

    plugins_factories_.push_back(plugin_factory_interface);

    return true;
  }

  bool unregister_plugin_factory(logger_plugin_factory_interface* plugin_factory_interface) {
    // TODO: TBD
    return false;
  }

  /**
  * \brief    Create new plugin instance from factory and attach it
  * \param    type    Plugin type name
  * \param    name    Instance name
  * \return   true - plugin successfully created and attached,
  *           false - plugin was not attached, reasons may be: plugin with same type and name already attached, plugin factory was not found for specified type,
  *                   plugin factory cannot create plugin object, plugin created but cannot be attached
  */
  shared_ptr<logger_plugin_interface> create_and_attach_plugin_from_factory(const std::string& type, const std::string& name) {
    if (is_plugin_attached(type.c_str(), name.c_str()))
      return shared_ptr<logger_plugin_interface>();

    std::list<logger_plugin_factory_interface*>::const_iterator it =
      std::find_if(plugins_factories_.begin(), plugins_factories_.end(), find_plugin_type<logger_plugin_factory_interface>(type));

    if (it == plugins_factories_.end())
      return shared_ptr<logger_plugin_interface>();

    shared_ptr<logger_plugin_interface> plugin_interface = (*it)->create(name.c_str());
    if (!plugin_interface)
      return shared_ptr<logger_plugin_interface>();

    bool result = attach_plugin_internal(plugin_interface, *it);
    if (result)
      return plugin_interface;

    return shared_ptr<logger_plugin_interface>();
  }

  void get_all_plugin_factories(std::vector<logger_plugin_factory_interface*> plugin_factories) const {
    plugin_factories.clear();
    std::copy(plugins_factories_.begin(), plugins_factories_.end(), std::back_inserter(plugin_factories));
  }

  void get_all_plugins(std::vector<shared_ptr<logger_plugin_interface> >& plugins) const {
    plugins.clear();

    for (std::map<int, std::list<plugin_data> >::const_iterator it = plugins_.cbegin();
      it != plugins_.cend(); it++) {
      for (std::list<plugin_data>::const_iterator plugin_it = it->second.cbegin(); 
        plugin_it != it->second.cend(); plugin_it++) {
          if (plugin_it->plugin_)
            plugins.push_back(plugin_it->plugin_);
      }
    }
  }

  template<typename TPlugin>
  void query_plugins(int plugin_type, std::vector<shared_ptr<TPlugin> >& plugins) const {
    plugins.clear();
    plugins.reserve(plugins_.at(plugin_type).size());

    for (std::list<plugin_data>::const_iterator it = plugins_.at(plugin_type).cbegin();
      it != plugins_.at(plugin_type).cend(); it++) {
      shared_ptr<TPlugin> plugin = dynamic_pointer_cast<TPlugin>(it->plugin_);
      if (plugin)
        plugins.push_back(plugin);
    }
  }

  /** Add logger output interface */
  bool attach_plugin(shared_ptr<logger_plugin_interface> plugin_interface) {
    if (!plugin_interface)
      return false;

    int plugin_type = plugin_interface->plugin_type();
    if (plugin_type < kLogPluginTypeMin || plugin_type > kLogPluginTypeMax)
      return false;

    // check that plugin was not added already
    for (std::list<plugin_data>::iterator it = plugins_.at(plugin_type).begin(); it != plugins_.at(plugin_type).end(); it++) {
      if (it->plugin_ == plugin_interface)
        return false;
    }

    return attach_plugin_internal(plugin_interface, NULL);
  }

  /** Detach plugin from logger */
  bool detach_plugin(shared_ptr<logger_plugin_interface> plugin_interface) {
    if (!plugin_interface)
      return false;

    int plugin_type = plugin_interface->plugin_type();
    if (plugin_type < kLogPluginTypeMin || plugin_type > kLogPluginTypeMax)
      return false;

    bool result = false;

    for (std::list<plugin_data>::iterator it = plugins_.at(plugin_type).begin(); it != plugins_.at(plugin_type).end(); it++) {
      if (it->plugin_ != plugin_interface)
        continue;

      it->plugin_->detach(base_);
      plugins_.at(plugin_type).erase(it);
      result = true;
      break;
    }

    return result;
  }

  void detach_all_plugins() {
    for (int i = kLogPluginTypeMin; i <= kLogPluginTypeMax; i++) {
      while (plugins_.at(i).size()) {
        detach_plugin(plugins_.at(i).begin()->plugin_);
      }
    }
  }

  bool is_plugin_attached(const char* type, const char* name) const {
    for (int i = kLogPluginTypeMin; i <= kLogPluginTypeMax; i++) {
      for (std::list<plugin_data>::const_iterator it = plugins_.at(i).begin(); it != plugins_.at(i).end(); it++) {
        if (type && !str::compare(it->plugin_->type(), type))
          continue;

        if (name && !str::compare(it->plugin_->name(), name))
          continue;

        return true;
      }
    }

    return false;
  }


protected:
  bool attach_plugin_internal(shared_ptr<logger_plugin_interface> plugin_interface,
    logger_plugin_factory_interface* plugin_factory) {

    plugin_data attached_plugin;
    attached_plugin.plugin_ = plugin_interface;
    attached_plugin.factory_ = plugin_factory;

    if (plugin_interface->plugin_type() < kLogPluginTypeMin || plugin_interface->plugin_type() > kLogPluginTypeMax)
      return false;

    if (!plugin_interface->attach(base_))
      return false;

    plugins_.at(plugin_interface->plugin_type()).push_back(attached_plugin);
    return true;
  }

  template<typename TPlug>
  struct find_plugin_type : std::unary_function<TPlug*, bool> {
    std::string type_;
    find_plugin_type(const std::string& type) :type_(type) { }
    bool operator()(TPlug* const& obj) const {
      return str::compare(type_.c_str(), obj->type());
    }
  };

private:
  struct plugin_data {
    shared_ptr<logger_plugin_interface> plugin_;
    logger_plugin_factory_interface* factory_;
    plugin_data() : plugin_(), factory_(NULL) {}
  };

  std::map<int, std::list<plugin_data> > plugins_;
  std::list<logger_plugin_factory_interface*> plugins_factories_;
  logger_interface* base_;
};


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

  std::map<int, logging::detail::shared_ptr<logger_plugin_interface> > cmd_refs_;

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

#if LOG_MULTITHREADED
  /** Log record message structure */
  struct log_record {
    int verb_level_;
    std::string hdr_;
    std::string text_;
    log_record() : verb_level_(logger_verbose_mute) {}
  };

  /** buffer lock object */
  LOG_MT_MUTEX mt_buffer_lock_;

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
    utils::set_current_thread_name("logger_thread");
    logger* log = reinterpret_cast<logger*>(data);
    
    while (true) {
      std::list<log_record> log_records;
      //std::list<plugin_data> outputs;
      std::vector<shared_ptr<logger_output_plugin_interface> > output_plugins;

      long mt_terminating;

      // mutex locked section
      {
        LOG_MT_MUTEX_LOCK(&log->mt_buffer_lock_);

        if (log->mt_buffer_.size() == 0) {
          mt::wait_event(&log->write_event_, &log->mt_buffer_lock_, true, 250);
        }

        // copy all records from log to local and clear log buffer
        std::copy(log->mt_buffer_.begin(), log->mt_buffer_.end(), std::back_inserter(log_records));
        log->mt_buffer_.clear();

        log->plugin_mgr_->query_plugins<logger_output_plugin_interface>(kLogPluginTypeOutput, output_plugins);
        //outputs = log->plugins_[kLogPluginTypeOutput];

        LOG_MT_MUTEX_UNLOCK(&log->mt_buffer_lock_);

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

    LOG_MT_MUTEX_LOCK(&mt_buffer_lock_);

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
	  LOG_MT_MUTEX_UNLOCK(&mt_buffer_lock_);
  }



#else  // LOG_MULTITHREADED
  /** Single threaded put_to_stream */
  __inline void put_to_stream(int verb_level, const std::string& hdr, const std::string& what) {
    handle_first_write();
    std::string text = hdr + std::string(" ") + what + std::string("\n");

    // call registered outputs
    for (std::list<plugin_data>::const_iterator it = plugins_.at(kLogPluginTypeOutput).cbegin();
      it != plugins_.at(kLogPluginTypeOutput).cend(); it++) {
      logger_output_plugin_interface* output_plugin = dynamic_cast<logger_output_plugin_interface*>(it->plugin_);
      if (output_plugin)
        output_plugin->write(verb_level, hdr, what);
    }
    /*
#if LOG_ANDROID_SYSLOG
    __android_log_write(ANDROID_LOG_INFO, "LOGGER", text.c_str());
#else  // LOG_ANDROID_SYSLOG

#if LOG_FLUSH_FILE_EVERY_WRITE
#if !LOG_TEST_DO_NOT_WRITE_FILE
    std::ofstream stream(configurator.get_full_log_file_path().c_str(), std::ios::app);
    stream << what;
#endif  // LOG_TEST_DO_NOT_WRITE_FILE
#else   // LOG_FLUSH_FILE_EVERY_WRITE
    if (!stream_.is_open())
      stream_.open(configurator.get_full_log_file_path().c_str(), std::ios::app);

    stream_ << text;
#endif  // LOG_FLUSH_FILE_EVERY_WRITE
#endif  // LOG_ANDROID_SYSLOG

    cur_file_size_ += static_cast<int>(what.size());
    */
  }
#endif  // LOG_MULTITHREADED

  /** Process first write in session */
  void handle_first_write() {
    if (first_write_) {
      first_write_ = false;
      put_to_stream(logger_verbose_all, "", "\n");
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
  void register_command_plugin(logging::detail::shared_ptr<TCmdPlugin> cmd_plugin) {
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

  const char* get_config_param(const char* key) const {
    return cfg::get_value(config_, key).c_str();
  }


  bool register_plugin_factory(logger_plugin_factory_interface* plugin_factory_interface) {
    return plugin_mgr_->register_plugin_factory(plugin_factory_interface);
  }

  bool unregister_plugin_factory(logger_plugin_factory_interface* plugin_factory_interface) {
    return plugin_mgr_->unregister_plugin_factory(plugin_factory_interface);
  }


  void flush() {
#if LOG_MULTITHREADED
    LOG_MT_MUTEX_LOCK(&mt_buffer_lock_);

    // flush multithreaded buffer
    while(mt_buffer_.size()) {
      if (mt::wait_event(&buffer_devastated_, &mt_buffer_lock_, true, 100))
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

#if LOG_MULTITHREADED
    LOG_MT_MUTEX_UNLOCK(&mt_buffer_lock_);
#endif  // LOG_MULTITHREADED
  }


  log_stream stream(int verb_level, void* addr, const char* function_name, const char* source_file, int line_number) {
    return log_stream(this, verb_level, addr, function_name, source_file, line_number);
  }

  bool attach_plugin(logger_plugin_interface* plugin_interface) LOG_METHOD_OVERRIDE {
    shared_ptr<logger_plugin_interface> plugin_ptr(plugin_interface);
    return plugin_mgr_->attach_plugin(plugin_ptr);
  }


  unsigned int get_version() const LOG_METHOD_OVERRIDE {
    return 0x02010101;
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

#if LOG_MULTITHREADED
    LOG_MT_MUTEX_INIT(&mt_buffer_lock_, NULL);
#endif /*LOG_MULTITHREADED*/

    reload_config();

#if LOG_MULTITHREADED
    mt::create_event(&write_event_);
    mt::create_event(&buffer_devastated_);

#  ifdef LOG_PLATFORM_WINDOWS

    DWORD thread_id;
    log_thread_handle_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&log_thread_fn,
                                     this, 0, &thread_id);    
#  else   /*LOG_PLATFORM_WINDOWS*/
    pthread_create(&log_thread_handle_, NULL, (void* (*)(void*)) & log_thread_fn, this);
#  endif  /*LOG_PLATFORM_WINDOWS*/
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
      }
    }

    // process [load] section
    for (cfg::KeyValueTypeList::const_iterator it = config.cbegin(); it != config.cend(); it++) {
      const std::string load_prefix = "load:";

      if (!str::starts_with(it->first, load_prefix))
        continue;

      if (atoi(it->second.c_str()) == 0)
        continue; // do not load

      std::string key_name = it->first.substr(load_prefix.size());

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
    log_config_.filter_verbose_level_ = cfg::get_logcfg_int(config_, kLoggerConfigTypeName, std::string(), "FilterVerboseLevel", logger_verbose_all);
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
//#if LOG_INI_CONFIGURATION
//    log_ini_configurator::configure(configurator.get_ini_file_find_paths().c_str());
//#endif  // LOG_INI_CONFIGURATION

//#if LOG_CONFIGURE_FROM_REGISTRY
//    log_registry_configurator::configure(configurator.get_reg_config_path());
//#endif  // LOG_CONFIGURE_FROM_REGISTRY

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
  __inline std::string try_get_module_name_fast(void* ptr) {
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
  __inline bool is_message_enabled(int verb_level) const {
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

//////////////////////////////////////////////////////////////

}//namespace detail


};  // namespace logging

///////////////   C tail  ///////////////////////

#else  // defined(__cplusplus) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))


#if LOG_USE_DLL
extern void(LOG_CDECL* __c_logger_log_modules)(int verbose_level, void* caller_addr,
                                               const char* function, const char* file,
                                               int line);
#endif  // LOG_USE_DLL


#if LOG_USE_MODULEDEFINITION
#if !LOG_USE_DLL
void __c_logger_log_modules(int verbose_level, void* caller_addr, const char* function,
                            const char* file, int line);
#endif  //! LOG_USE_DLL
#endif  // LOG_USE_MODULEDEFINITION

#if LOG_USE_DLL
extern void(LOG_CDECL* __c_logger_log_stack_trace)(int verbose_level, void* caller_addr,
                                                   const char* function, const char* file,
                                                   int line);
#endif  // LOG_USE_DLL

#if LOG_AUTO_DEBUGGING
#if !LOG_USE_DLL
void __c_logger_log_stack_trace(int verbose_level, void* caller_addr,
                                const char* function, const char* file, int line);
#endif  //! LOG_USE_DLL
#endif  // LOG_AUTO_DEBUGGING

#if LOG_USE_DLL
extern void(LOG_CDECL* __c_logger_log)(int verbose_level, void* caller_addr,
                                       const char* function, const char* file, int line,
                                       const char* format, ...);
extern void(LOG_CDECL* __c_logger_log_args)(int verbose_level, void* caller_addr,
                                            const char* function, const char* file,
                                            int line, const char* format, va_list args);

extern void(LOG_CDECL* __c_logger_log_cmd)(int cmd_id, int verbose_level, void* caller_addr,
  const char* function, const char* file,
  int line, const void* vparam, int iparam);

extern void(LOG_CDECL* __c_logger_log_cmd_args)(int cmd_id, int verbose_level, void* caller_addr,
  const char* function, const char* file,
  int line, const void* vparam, int iparam, va_list args);

/*
extern void(LOG_CDECL* __c_logger_log_binary)(int verbose_level, void* caller_addr,
                                              const char* function, const char* file,
                                              int line, const char* data, int len);

extern void(LOG_CDECL* __c_logger_set_verbose_level)(int verbose_level);
*/
extern void(LOG_CDECL* __c_logger_objmon_register)(size_t hash_code,
                                                   const char* type_name, void* ptr);
extern void(LOG_CDECL* __c_logger_objmon_unregister)(size_t hash_code, void* ptr);
extern void(LOG_CDECL* __c_logger_objmon_dump)(int verbose_level);

extern void(LOG_CDECL* __c_logger_set_current_thread_name)(const char* thread_name);

extern void(LOG_CDECL* __c_logger_set_config_param)(const char* key, const char* value);
extern int(LOG_CDECL* __c_logger_get_config_param)(const char* key, char* value, int buffer_size);


#else  // LOG_USE_DLL
void __c_logger_log(int verbose_level, void* caller_addr, const char* function,
                    const char* file, int line, const char* format, ...);
void __c_logger_log_args(int verbose_level, void* caller_addr, const char* function,
                         const char* file, int line, const char* format, va_list args);

void __c_logger_log_cmd(int cmd_id, int verbose_level, void* caller_addr, const char* function,
  const char* file, int line, const void* vparam, int iparam);

void __c_logger_log_cmd_args(int cmd_id, int verbose_level, void* caller_addr, const char* function,
  const char* file, int line, const void* vparam, int iparam, va_list args);

//void __c_logger_log_binary(int verbose_level, void* caller_addr, const char* function,
//                           const char* file, int line, const char* data, int len);

//void __c_logger_objmon_register(size_t hash_code, const char* type_name, void* ptr);
//void __c_logger_objmon_unregister(size_t hash_code, void* ptr);
//void __c_logger_objmon_dump(int verbose_level);

void __c_logger_set_config_param(const char* key, const char* value);
int __c_logger_get_config_param(const char* key, char* value, int buffer_size);


void __c_logger_set_current_thread_name(const char* thread_name);

#endif  // LOG_USE_DLL

#if defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#endif  // defined(__cplusplus) && (!LOG_USE_DLL || defined(LOG_THIS_IS_DLL))

#endif  // LOG_ENABLED

#endif  //__LOGGER_HEADER
