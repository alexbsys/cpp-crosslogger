
#ifndef LOGGER_WIN_REGISTRY_HEADER
#define LOGGER_WIN_REGISTRY_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_sysinclib.h"

namespace logging {
namespace detail {

struct log_registry_helper {
  static HKEY reg_base_key_by_string(const std::string& storage) {
    if (storage == "HKLM" || storage == "HKEY_LOCAL_MACHINE") return HKEY_LOCAL_MACHINE;
    if (storage == "HKCU" || storage == "HKEY_CURRENT_USER") return HKEY_CURRENT_USER;
    if (storage == "HKCR" || storage == "HKEY_CLASSES_ROOT") return HKEY_CLASSES_ROOT;
    if (storage == "HKU" || storage == "HKEY_USERS") return HKEY_USERS;
    return HKEY_CURRENT_USER;
  }

  static bool get_reg_dword_value(HKEY baseKey, const std::string& regPath,
    const std::string& valueName, unsigned long& value) {
    HKEY hkey;
    LONG ret = RegOpenKeyExA(baseKey, regPath.c_str(), 0, KEY_QUERY_VALUE, &hkey);
    if (ret != ERROR_SUCCESS) return false;

    DWORD size = 0;
    DWORD type;
    ret = -1;
    RegQueryValueExA(hkey, valueName.c_str(), 0, &type, NULL, &size);

    if (type == REG_DWORD) {
      size = sizeof(long);
      ret = RegQueryValueExA(hkey, valueName.c_str(), 0, &type, (BYTE*)&value, &size);
    }

    RegCloseKey(hkey);
    return ret == ERROR_SUCCESS;
  }

  static bool get_reg_sz_value(HKEY baseKey, const std::string& regPath,
    const std::string& valueName, std::string& result) {
    HKEY hkey;
    LONG ret = RegOpenKeyExA(baseKey, regPath.c_str(), 0, KEY_QUERY_VALUE, &hkey);
    if (ret != ERROR_SUCCESS) return false;

    DWORD size = 0;
    DWORD type;
    ret = -1;
    RegQueryValueExA(hkey, valueName.c_str(), 0, &type, NULL, &size);

    if (type == REG_SZ) {
      size_t bufSize = 512;

      std::string value;
      value.resize(bufSize);

      DWORD size = static_cast<DWORD>(value.length()) + 1;
      ret =
        RegQueryValueExA(hkey, valueName.c_str(), 0, &type, (BYTE*)value.data(), &size);

      while (ret == ERROR_MORE_DATA) {
        bufSize += 256;
        value.resize(bufSize);

        size = static_cast<DWORD>(value.length()) + 1;
        ret = RegQueryValueExA(hkey, valueName.c_str(), NULL, &type, (BYTE*)value.data(),
          &size);
      }

      if (ret == ERROR_SUCCESS) result = value.c_str();
    }

    RegCloseKey(hkey);
    return ret == ERROR_SUCCESS;
  }
}; //struct log_registry_helper


}//namespace detail
}//namespace logging

#endif /*LOGGER_WIN_REGISTRY_HEADER*/
