

#ifndef LOGGER_SCROLL_FILE_OUTPUT_PLUGIN_HEADER
#define LOGGER_SCROLL_FILE_OUTPUT_PLUGIN_HEADER

#include <log/logger_config.h>
#include <log/logger_pdetect.h>
#include <log/logger_pdefs.h>
#include <log/logger_interfaces.h>
#include <log/logger_verbose.h>

#include <log/detail/logger_utils.h>
#include <log/detail/logger_cfgfn.h>

#include <fstream>

#define LOGGER_HAVE_BUILTIN_SCROLL_FILE_OUTPUT_PLUGIN  1

namespace logging {

class logger_scroll_file_output_plugin : public logger_output_plugin_interface {
public:
  virtual ~logger_scroll_file_output_plugin() {}

  logger_scroll_file_output_plugin(const char* output_name = NULL)
    : cur_file_size_(0)
    , name_(output_name == NULL ? std::string() : output_name)
    , first_write_(true) {
  }

  const char* type() const LOG_METHOD_OVERRIDE {
    return "file_output";
  }

  const char* name() const LOG_METHOD_OVERRIDE {
    return name_.c_str();
  }

  void config_updated(const logging::cfg::KeyValueTypeList& config) LOG_METHOD_OVERRIDE {
    using namespace detail;

    std::string prev_full_log_file_path = config_.full_log_file_path_;
    std::string prev_log_path = config_.log_path_;

    config_.scroll_file_size_ = detail::cfg::get_logcfg_int(config, type(), name(), "ScrollFileSize", 0);
    config_.log_path_ = detail::cfg::get_logcfg_string(config, type(), name(), "LogPath", ".");
    config_.verbose_level_ = detail::cfg::get_logcfg_int(config, type(), name(), "Verbose", logger_verbose_all);
    config_.log_file_name_ = detail::cfg::get_logcfg_string(config, type(), name(), "LogFileName", "$(EXEFILENAME)");
    config_.scroll_file_count_ = detail::cfg::get_logcfg_int(config, type(), name(), "ScrollFileCount", 0);
    config_.scroll_file_every_run_ = detail::cfg::get_logcfg_int(config, type(), name(), "ScrollFileEveryRun", 0) ? true : false;
    config_.create_log_directory_ = detail::cfg::get_logcfg_int(config, type(), name(), "CreateLogDirectory", 1) ? true : false;
    config_.flush_every_write_ = detail::cfg::get_logcfg_int(config, type(), name(), "FlushFileEveryWrite", 0) ? true : false;

    config_.full_log_file_path_ = config_.log_path_ + detail::utils::get_path_separator() + config_.log_file_name_;

    if (config_.log_path_ != prev_log_path && config_.create_log_directory_) {
      utils::create_directory_path(config_.log_path_);
    }

    if (prev_full_log_file_path != config_.full_log_file_path_) {
      scroll_files(config_.scroll_file_every_run_);
    }
  }


  /**
  * \brief    write method. Called every write to file operation
  */
  virtual void write(int verb_level, const std::string& hdr, const std::string& what) LOG_METHOD_OVERRIDE {
    if ((verb_level & config_.verbose_level_) == 0)
      return;

    if (!stream_.is_open())
      stream_.open(config_.full_log_file_path_.c_str(), std::ios::app);

    if (!stream_.is_open())
      return;

    if (is_need_scroll_files()) {
      scroll_files();
    }

    std::string str = hdr;
    if (hdr.size()) str.append(" ");
    str += what + std::string("\n");

    cur_file_size_ += static_cast<int>(str.size());

#if LOG_ANDROID_SYSLOG
    __android_log_write(ANDROID_LOG_INFO, "LOGGER", str.c_str());
#else  // LOG_ANDROID_SYSLOG
    stream_ << str;
#endif  // LOG_ANDROID_SYSLOG

    if (config_.flush_every_write_) {
      stream_.flush();
      stream_.close();
    }
  }

  void flush() LOG_METHOD_OVERRIDE {
    if (!config_.flush_every_write_ && stream_.is_open())
      stream_.flush();
  }

  void close() LOG_METHOD_OVERRIDE {
    if (stream_.is_open())
      stream_.close();
  }

  virtual void detach(logger_interface* logger) LOG_METHOD_OVERRIDE {
    (void)logger;
    flush();
    close();
  }

protected:
  /** Get current log file index. Used for log files scrolling */
  static int get_log_file_index(const std::string& name) {
    size_t last_point_pos = name.find_last_of('.');
    if (last_point_pos == std::string::npos)
      return 0;

    std::string idx = name.substr(last_point_pos);

    if (idx.size())
      idx = idx.substr(1);

    for (size_t i = 0; i < idx.size(); i++)
      if (!isdigit(idx[i]))
        return -1;

    return atoi(idx.c_str());
  }

  bool is_need_scroll_files() const {
    if (!config_.scroll_file_size_) return false;
    int file_size = cur_file_size_;
    return file_size > static_cast<int>(config_.scroll_file_size_);
  }

  /**  Scroll log files if needed */
  void scroll_files(bool force = false) {
    using namespace detail;

    if (!force && !config_.scroll_file_size_) return;

    bool need_scroll = force;
    if (!need_scroll)
      need_scroll = is_need_scroll_files();

    if (need_scroll) {
      std::map<int, std::string> log_files;
      int max_index = 0;

      cur_file_size_ = 0;

#ifdef LOG_PLATFORM_WINDOWS
      WIN32_FIND_DATAA find_data;

      std::string mask = config_.full_log_file_path_ + ".*";
      HANDLE find_handle = FindFirstFileA(mask.c_str(), &find_data);

      if (find_handle != INVALID_HANDLE_VALUE) {
        do {
          std::string name = find_data.cFileName;
          int index = get_log_file_index(name);

          if (index > max_index) max_index = index;

          if (index >= 0) {
            log_files.insert(std::pair<int, std::string>(index, name));
          }

        } while (FindNextFileA(find_handle, &find_data));

        FindClose(find_handle);
      }

#else   // LOG_PLATFORM_WINDOWS

      std::string find_pattern = config_.log_file_name_ + ".*";

      DIR* dir_obj = opendir(config_.log_path_.c_str());
      if (dir_obj) {
        struct dirent* file_obj = NULL;

        while (NULL != (file_obj = readdir(dir_obj))) {
          int ret =
            fnmatch(find_pattern.c_str(), file_obj->d_name, FNM_PATHNAME | FNM_PERIOD);
          if (ret == 0) {
            int index = get_log_file_index(file_obj->d_name);

            if (index > max_index) max_index = index;

            if (index) {
              log_files.insert(std::pair<int, std::string>(index, file_obj->d_name));
            }
          }
        }
        closedir(dir_obj);
      }
#endif  // LOG_PLATFORM_WINDOWS

      if (stream_.is_open()) {
        stream_.flush();
        stream_.close();
      }

      for (unsigned int i = max_index; i > 0; i--) {
        if (log_files.find(i) == log_files.end())
          continue;

        unsigned int new_index = i + 1;
        const std::string& name = log_files[i];

        if (config_.scroll_file_count_ &&
          config_.scroll_file_count_ < new_index) {
          std::string file_path_for_delete = config_.log_path_ + utils::get_path_separator() + name;

          utils::delete_file(file_path_for_delete);
          continue;
        }

        char new_index_buffer[32];
        sprintf(new_index_buffer, "%d", new_index);

        std::string new_name =
          name.substr(0, name.find_last_of('.')) + std::string(".") + std::string(new_index_buffer);

        utils::move_file((config_.log_path_ + utils::get_path_separator() + name),
          (config_.log_path_ + utils::get_path_separator() + new_name));
      }

      utils::move_file(config_.full_log_file_path_, config_.full_log_file_path_ + ".1");

      if (!config_.flush_every_write_) {
        if (!stream_.is_open())
          stream_.open(config_.full_log_file_path_.c_str(), std::ios::app);
      }
    }
  }

  struct file_output_config {
    std::string log_path_;
    std::string log_file_name_;
    std::string full_log_file_path_;
    bool flush_every_write_;
    size_t scroll_file_size_;
    size_t scroll_file_count_;
    bool scroll_file_every_run_;
    bool create_log_directory_;
    int verbose_level_;

    file_output_config() :
      flush_every_write_(false), scroll_file_size_(0),
      scroll_file_count_(0), scroll_file_every_run_(false), 
      create_log_directory_(true), 
      verbose_level_(logger_verbose_all)
    {}
  };

  file_output_config config_;

private:
  std::ofstream stream_;
  int cur_file_size_;
  std::string name_;
  bool first_write_;
};


class logger_scroll_file_output_plugin_factory : public logger_plugin_default_factory<logger_scroll_file_output_plugin> {
public:
  logger_scroll_file_output_plugin_factory() 
    : logger_plugin_default_factory<logger_scroll_file_output_plugin>("file_output", kLogPluginTypeOutput) {}
  virtual ~logger_scroll_file_output_plugin_factory() {}
};

}//namespace logging

#endif /*LOGGER_SCROLL_FILE_OUTPUT_PLUGIN_HEADER*/
