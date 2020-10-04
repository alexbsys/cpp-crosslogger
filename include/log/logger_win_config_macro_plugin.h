#ifndef LOGGER_WIN_CONFIG_MACRO_PLUGIN
#define LOGGER_WIN_CONFIG_MACRO_PLUGIN


#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_pdefs.h"
#include "logger_interfaces.h"
#include "logger_strutils.h"
#include "logger_utils.h"

#if LOG_USE_MODULEDEFINITION
#include "logger_moddef.h"
#endif /*LOG_USE_MODULEDEFINITION*/

namespace logging {

class logger_win_config_macro_plugin : public logger_config_macro_plugin_interface {
public:
  logger_win_config_macro_plugin(const char* plugin_name = NULL) 
    : plugin_name_(plugin_name ? plugin_name : std::string()) {}

  virtual ~logger_win_config_macro_plugin() {}

  const char* type() const {
    return "win_config_macro";
  }

  const char* name() const { return plugin_name_.c_str(); }

  virtual bool process(std::string& str) {
    int replaced;
    str = process_config_macro(str, replaced);
    return replaced > 0;
  }

protected:
  /**
  * \brief    Process configuration macroses
  * \param    str     configuration string with macroses
  * \return   String with replaced macroses
  */
  static std::string process_config_macro(std::string str, int& replaced) {
    using namespace detail;

    replaced = 0;

    if (str::contains(str.c_str(), "$(CURRENTDIR)")) {
#ifdef LOG_PLATFORM_WINDOWS
      char current_dir[MAX_PATH + 1];
      GetCurrentDirectoryA(MAX_PATH, current_dir);
#else  // LOG_PLATFORM_WINDOWS
      char current_dir[512];

#ifdef LOG_HAVE_UNISTD_H
      if (getcwd(current_dir, 512)) current_dir[0] = 0;
#else   // LOG_HAVE_UNISTD_H
      strcpy(current_dir, "./");
#endif  // LOG_HAVE_UNISTD_H

#endif  // LOG_PLATFORM_WINDOWS

      str = str::replace(str, "$(CURRENTDIR)", current_dir);
      ++replaced;
    }

    if (str::contains(str.c_str(), "$(TEMPPATH)")) {
#ifdef LOG_PLATFORM_WINDOWS
      char temp_path[MAX_PATH + 1];
      GetTempPathA(MAX_PATH, temp_path);
#else   // LOG_PLATFORM_WINDOWS
      const char* temp_path = P_tmpdir;
#endif  // LOG_PLATFORM_WINDOWS

      str = str::replace(str, "$(TEMPPATH)", temp_path);
      ++replaced;
    }

#ifdef LOG_PLATFORM_WINDOWS
    if (str::contains(str.c_str(), "$(SYSTEMPATH)")) {
      char system_path[MAX_PATH + 1];
      GetSystemDirectoryA(system_path, MAX_PATH);

      str = str::replace(str, "$(SYSTEMPATH)", system_path);
      ++replaced;
    }

    if (str::contains(str.c_str(), "$(WINDOWSPATH)")) {
      char windows_path[MAX_PATH + 1];
      GetWindowsDirectoryA(windows_path, MAX_PATH);
      str = str::replace(str, "$(WINDOWSPATH)", windows_path);
      ++replaced;
    }

    if (str::contains(str.c_str(), "$(COMMONAPPDATA)")) {
      char appdata_path[MAX_PATH];
      appdata_path[0] = 0;
      SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, appdata_path);
      str = str::replace(str, "$(COMMONAPPDATA)", appdata_path);
      ++replaced;
    }

    if (str::contains(str.c_str(), "$(USERAPPDATA)")) {
      char appdata_path[MAX_PATH];
      appdata_path[0] = 0;
      SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appdata_path);
      str = str::replace(str, "$(USERAPPDATA)", appdata_path);
      ++replaced;
    }

    if (str::contains(str.c_str(), "$(DESKTOPDIR)")) {
      char desktop_path[MAX_PATH];
      desktop_path[0] = 0;
      SHGetFolderPathA(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, desktop_path);
      str = str::replace(str, "$(DESKTOPDIR)", desktop_path);
      ++replaced;
    }
#endif  // LOG_PLATFORM_WINDOWS

    if (str::contains(str.c_str(), "$(EXEDIR)")) {
      str = str::replace(str, "$(EXEDIR)", utils::get_process_file_path());
      ++replaced;
    }

    if (str::contains(str.c_str(), "$(EXEFILENAME)")) {
      str = str::replace(str, "$(EXEFILENAME)", utils::get_process_file_name());
      ++replaced;
    }

    if (str::contains(str.c_str(), "$(EXEFULLFILENAME)")) {
      str = str::replace(str, "$(EXEFULLFILENAME)", utils::get_process_full_file_name());
      ++replaced;
    }

#if LOG_USE_MODULEDEFINITION
    std::string module_path =
      module_definition::module_name_by_addr((void*)&process_config_macro);

    size_t last_delim = module_path.find_last_of("\\/");
    std::string module_dir = module_path.substr(0, last_delim);
    if (last_delim != std::string::npos) last_delim++;

    std::string module_full_file_name = module_path.substr(last_delim, std::string::npos);
    size_t count = module_path.find_last_of(".");
    if (count != std::string::npos) count -= last_delim;

    std::string module_file_name = module_path.substr(last_delim, count);

    if (str::contains(str.c_str(), "$(EXEFULLFILENAME)")) {
      str = str::replace(str, "$(EXEFULLFILENAME)", module_dir);
      ++replaced;
    }

    if (str::contains(str.c_str(), "$(MODULEFULLFILENAME)")) {
      str = str::replace(str, "$(MODULEFULLFILENAME)", module_full_file_name);
      ++replaced;
    }

    if (str::contains(str.c_str(), "$(MODULEFILENAME)")) {
      str = str::replace(str, "$(MODULEFILENAME)", module_file_name);
      ++replaced;
    }
#endif  // LOG_USE_MODULEDEFINITION

    return str;
  }

private:
  std::string plugin_name_;
};

class logger_win_config_macro_plugin_factory : public logger_plugin_default_factory<logger_win_config_macro_plugin> {
public:
  logger_win_config_macro_plugin_factory()
    : logger_plugin_default_factory<logger_win_config_macro_plugin>("win_config_macro", kLogPluginTypeConfigMacro) {}
  virtual ~logger_win_config_macro_plugin_factory() {}
};


}//namespace logging

#endif /*LOGGER_WIN_CONFIG_MACRO_PLUGIN*/
