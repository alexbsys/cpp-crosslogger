
#ifndef LOGGER_PLUGIN_MANAGER_HEADER
#define LOGGER_PLUGIN_MANAGER_HEADER

#include <log/logger_config.h>
#include <log/logger_pdetect.h>
#include <log/logger_pdefs.h>
#include <log/logger_interfaces.h>
#include <log/logger_shared_ptr.hpp>

#include <log/detail/logger_strutils.h>

#include <log/detail/logger_mt.h>

#include <list>
#include <vector>
#include <map>
#include <iterator>
#include <functional>

namespace logging {
namespace detail {

class plugin_manager {
public:
  plugin_manager(logger_interface* base) : base_(base), plugins_size_cache_(0) {
    LOG_MT_MUTEX_INIT(&mutex_, NULL);

    for (int i = kLogPluginTypeMin; i <= kLogPluginTypeMax; i++) {
      plugins_.insert(std::pair<int, std::list<plugin_data_type> >(i, std::list<plugin_data_type>()));
    }
  }

  ~plugin_manager() {
    detach_all_plugins();

    LOG_MT_MUTEX_DESTROY(&mutex_);
  }

  bool register_plugin_factory(logger_plugin_factory_interface* plugin_factory_interface) {
    if (!plugin_factory_interface)
      return false;

    int plugin_type = plugin_factory_interface->plugin_type();
    if (plugin_type < kLogPluginTypeMin || plugin_type > kLogPluginTypeMax)
      return false;

    LOG_MT_MUTEX_LOCK(&mutex_);
    plugins_factories_.push_back(plugin_factory_interface);
    LOG_MT_MUTEX_UNLOCK(&mutex_);

    return true;
  }

  bool unregister_plugin_factory(logger_plugin_factory_interface* plugin_factory_interface) {
    // TODO: TBD
    (void)plugin_factory_interface;
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

    LOG_MT_MUTEX_LOCK(&mutex_);

    std::list<logger_plugin_factory_interface*>::const_iterator it =
      std::find_if(plugins_factories_.begin(), plugins_factories_.end(), find_plugin_type<logger_plugin_factory_interface>(type));

    if (it == plugins_factories_.end()) {
      LOG_MT_MUTEX_UNLOCK(&mutex_);
      return shared_ptr<logger_plugin_interface>();
    }

    shared_ptr<logger_plugin_interface> plugin_interface = (*it)->create(name.c_str());
    if (!plugin_interface) {
      LOG_MT_MUTEX_UNLOCK(&mutex_);
      return shared_ptr<logger_plugin_interface>();
    }

    LOG_MT_MUTEX_UNLOCK(&mutex_);

    bool result = attach_plugin_internal(plugin_interface, *it);

    if (result)
      return plugin_interface;

    return shared_ptr<logger_plugin_interface>();
  }

  void get_all_plugin_factories(std::vector<logger_plugin_factory_interface*> plugin_factories) const {
    plugin_factories.clear();

    LOG_MT_MUTEX_LOCK(&mutex_);
    std::copy(plugins_factories_.begin(), plugins_factories_.end(), std::back_inserter(plugin_factories));
    LOG_MT_MUTEX_UNLOCK(&mutex_);

  }

  void get_all_plugins(std::vector<shared_ptr<logger_plugin_interface> >& plugins) const {
    plugins.clear();

    LOG_MT_MUTEX_LOCK(&mutex_);

    for (std::map<int, std::list<plugin_data_type> >::const_iterator it = plugins_.cbegin();
      it != plugins_.cend(); it++) {
      for (std::list<plugin_data_type>::const_iterator plugin_it = it->second.cbegin();
        plugin_it != it->second.cend(); plugin_it++) {
        if (plugin_it->plugin_)
          plugins.push_back(plugin_it->plugin_);
      }
    }

    LOG_MT_MUTEX_UNLOCK(&mutex_);
  }

  template<typename TPlugin>
  void query_plugins(int plugin_type, std::vector<shared_ptr<TPlugin> >& plugins) const {
    plugins.clear();

    LOG_MT_MUTEX_LOCK(&mutex_);

    plugins.reserve(plugins_.at(plugin_type).size());

    for (std::list<plugin_data_type>::const_iterator it = plugins_.at(plugin_type).cbegin();
      it != plugins_.at(plugin_type).cend(); it++) {
      shared_ptr<TPlugin> plugin = dynamic_pointer_cast<TPlugin>(it->plugin_);
      if (plugin)
        plugins.push_back(plugin);
    }

    LOG_MT_MUTEX_UNLOCK(&mutex_);
  }

  /** Add logger output interface */
  bool attach_plugin(shared_ptr<logger_plugin_interface> plugin_interface) {
    if (!plugin_interface)
      return false;

    int plugin_type = plugin_interface->plugin_type();
    if (plugin_type < kLogPluginTypeMin || plugin_type > kLogPluginTypeMax)
      return false;

    // check that plugin was not added already

    LOG_MT_MUTEX_LOCK(&mutex_);

    for (std::list<plugin_data_type>::iterator it = plugins_.at(plugin_type).begin(); it != plugins_.at(plugin_type).end(); it++) {
      if (it->plugin_ == plugin_interface) {
        LOG_MT_MUTEX_UNLOCK(&mutex_);
        return false;
      }
    }

    LOG_MT_MUTEX_UNLOCK(&mutex_);

    bool result = attach_plugin_internal(plugin_interface, NULL);

    return result;
  }

  /** Detach plugin from logger */
  bool detach_plugin(const logger_plugin_interface* plugin_interface) {
    bool result = false;

    LOG_MT_MUTEX_LOCK(&mutex_);
    result = detach_plugin_internal(plugin_interface);
    LOG_MT_MUTEX_UNLOCK(&mutex_);

    return result;
  }

  void detach_all_plugins() {
    LOG_MT_MUTEX_LOCK(&mutex_);

    for (int i = kLogPluginTypeMin; i <= kLogPluginTypeMax; i++) {
      while (plugins_.at(i).size()) {
        detach_plugin_internal(plugins_.at(i).begin()->plugin_.get());
      }
    }

    LOG_MT_MUTEX_UNLOCK(&mutex_);
  }

  bool is_plugin_attached(const char* type, const char* name) const {
    LOG_MT_MUTEX_LOCK(&mutex_);

    for (int i = kLogPluginTypeMin; i <= kLogPluginTypeMax; i++) {
      for (std::list<plugin_data_type>::const_iterator it = plugins_.at(i).begin(); it != plugins_.at(i).end(); it++) {
        if (type && !str::compare(it->plugin_->type(), type))
          continue;

        if (name && !str::compare(it->plugin_->name(), name))
          continue;

        LOG_MT_MUTEX_UNLOCK(&mutex_);
        return true;
      }
    }

    LOG_MT_MUTEX_UNLOCK(&mutex_);
    return false;
  }

  size_t plugins_count() const {
    return plugins_size_cache_;
  }


protected:
  bool attach_plugin_internal(shared_ptr<logger_plugin_interface> plugin_interface,
    logger_plugin_factory_interface* plugin_factory) {

    plugin_data_type attached_plugin;
    attached_plugin.plugin_ = plugin_interface;
    attached_plugin.factory_ = plugin_factory;

    if (plugin_interface->plugin_type() < kLogPluginTypeMin || plugin_interface->plugin_type() > kLogPluginTypeMax)
      return false;

    if (!plugin_interface->attach(base_))
      return false;

    LOG_MT_MUTEX_LOCK(&mutex_);

    plugins_.at(plugin_interface->plugin_type()).push_back(attached_plugin);
    plugins_size_cache_++;

    LOG_MT_MUTEX_UNLOCK(&mutex_);

    return true;
  }

  bool detach_plugin_internal(const logger_plugin_interface* plugin_interface) {
    if (!plugin_interface)
      return false;

    int plugin_type = plugin_interface->plugin_type();
    if (plugin_type < kLogPluginTypeMin || plugin_type > kLogPluginTypeMax)
      return false;

    bool result = false;

    for (std::list<plugin_data_type>::iterator it = plugins_.at(plugin_type).begin(); it != plugins_.at(plugin_type).end(); it++) {
      if (it->plugin_.get() != plugin_interface)
        continue;

      it->plugin_->detach(base_);
      plugins_.at(plugin_type).erase(it);
      plugins_size_cache_--;
      result = true;
      break;
    }

    return result;
  }

#ifdef __APPLE__
#define UNARY_FUNCTION  std::__unary_function
#else //__APPLE__
#define UNARY_FUNCTION  std::unary_function
#endif //__APPLE__

  template<typename TPlug>
  struct find_plugin_type /*: UNARY_FUNCTION<TPlug*, bool>*/ {
    std::string type_;
    find_plugin_type(const std::string& type) :type_(type) { }
    bool operator()(TPlug* const& obj) const {
      return str::compare(type_.c_str(), obj->type());
    }
  };

private:
  struct plugin_data_type {
    shared_ptr<logger_plugin_interface> plugin_;
    logger_plugin_factory_interface* factory_;
    plugin_data_type() : plugin_(), factory_(NULL) {}
  };

  std::map<int, std::list<plugin_data_type> > plugins_;
  std::list<logger_plugin_factory_interface*> plugins_factories_;
  logger_interface* base_;

  mutable LOG_MT_MUTEX mutex_;
  std::atomic<size_t> plugins_size_cache_;
};

}//namespace detail
}//namespace logging

#endif /*LOGGER_PLUGIN_MANAGER_HEADER*/
