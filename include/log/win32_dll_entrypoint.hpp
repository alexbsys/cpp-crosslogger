
#ifdef _WIN32

#include <windows.h>
#include <log/logger.h>

#include <string>

#define LOGTAG "DLLENTRY "

static std::string GetModuleFileName(HMODULE module) {
  char file_name[MAX_PATH];

  ::GetModuleFileNameA(module, file_name, sizeof(file_name));

  std::string path(file_name);
  size_t last_delimiter = path.find_last_of("\\");
  std::string name_without_path = path.substr(last_delimiter+1);
  size_t last_ext = name_without_path.find_last_of(".");
  return name_without_path.substr(0, last_ext);
}

BOOL APIENTRY DllMain( 
  HMODULE hModule,
  DWORD  ul_reason_for_call,
  LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        std::string modname = GetModuleFileName(hModule);
        LOG_SET_CONFIG_PARAM("file_output::LogFileName", std::string(modname + ".log").c_str());
        LOG_SET_CONFIG_PARAM("logger::LoadPlugins", "builtin win_config_macro ini_config modules_cmd stacktrace_cmd binary_cmd crashhandler_cmd objmon_cmd win_registry_config file_output");

        std::string ini_file_paths = std::string(LOG_DEFAULT_INI_PATHS) + std::string(";$(EXEDIR)/") + modname + std::string(".log.ini");
        LOG_SET_CONFIG_PARAM("IniFilePaths", ini_file_paths.c_str());

        LOG_RELOAD_CONFIG();
        LOG_INFO(LOGTAG "%s DLL attached (%.8X)", modname.c_str(), hModule);
        break;
      }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH: {
        std::string modname = GetModuleFileName(hModule);
        LOG_INFO(LOGTAG "%s DLL detached (%.8X)", modname.c_str(), hModule);
        break;

      }
    }
    return TRUE;
}

#endif //_WIN32
