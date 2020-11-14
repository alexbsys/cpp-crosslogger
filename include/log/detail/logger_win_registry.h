
#ifndef LOGGER_WIN_REGISTRY_HEADER
#define LOGGER_WIN_REGISTRY_HEADER

#include <log/logger_config.h>
#include <log/logger_pdetect.h>
#include <log/logger_sysinclib.h>

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

  static bool reg_query_subkeys_values(HKEY base_key, const std::string& reg_path, 
    std::vector<std::string>& subkeys, std::vector<std::string>& values) {
    const int kMaxKeyLength = 255;
    const int kMaxValueNameLength = 16383;
    char key_name[kMaxKeyLength];
    DWORD name_length;
    char class_name[MAX_PATH] = "";
    DWORD class_name_length = MAX_PATH;
    DWORD subkeys_count = 0;
    DWORD longest_subkey_name_length;
    DWORD longest_class_name_length;
    DWORD values_count = 0;
    DWORD longest_value_length;
    DWORD longest_value_data_size;
    DWORD security_descriptor_size;
    FILETIME last_write_time;
    char  value_name[kMaxValueNameLength];
    DWORD value_name_length = kMaxValueNameLength;
    HKEY hkey;

    subkeys.clear();
    values.clear();

    LONG ret = RegOpenKeyExA(base_key, reg_path.c_str(), 0, KEY_READ, &hkey);
    if (ret != ERROR_SUCCESS) return false;

    // Get the class name and the value count. 
    ret = RegQueryInfoKeyA(
      hkey,                 // key handle 
      class_name,               // buffer for class name 
      &class_name_length,       // size of class string 
      NULL,                     // reserved 
      &subkeys_count,           // number of subkeys 
      &longest_subkey_name_length, // longest subkey size 
      &longest_class_name_length,  // longest class string 
      &values_count,               // number of values for this key 
      &longest_value_length,       // longest value name 
      &longest_value_data_size,    // longest value data 
      &security_descriptor_size,   // security descriptor 
      &last_write_time);        // last write time 

    if (ret != ERROR_SUCCESS) {
      RegCloseKey(hkey);
      return false;
    }

    // Enumerate the subkeys, until RegEnumKeyEx fails.
    if (subkeys_count) {
      for (DWORD i = 0; i<subkeys_count; i++) {
        name_length = kMaxKeyLength;
        ret = RegEnumKeyExA(hkey, i,
          key_name,
          &name_length,
          NULL,
          NULL,
          NULL,
          &last_write_time);
        if (ret == ERROR_SUCCESS) 
          subkeys.push_back(key_name);
      }
    }

    // Enumerate the key values. 
    if (values_count) {
      for (DWORD i = 0, ret = ERROR_SUCCESS; i<values_count; i++) {
        value_name_length = kMaxValueNameLength;
        value_name[0] = '\0';
        ret = RegEnumValueA(hkey, i,
          value_name,
          &value_name_length,
          NULL,
          NULL,
          NULL,
          NULL);

        if (ret == ERROR_SUCCESS) 
          values.push_back(value_name);
      }
    }

    RegCloseKey(hkey);
    return true;
  }

  static bool get_reg_dword_value(HKEY baseKey, const std::string& regPath,
    const std::string& valueName, unsigned long& value) {
    HKEY hkey;
    LONG ret = RegOpenKeyExA(baseKey, regPath.c_str(), 0, KEY_QUERY_VALUE, &hkey);
    if (ret != ERROR_SUCCESS) return false;

    DWORD size = 0;
    DWORD type = REG_NONE;
    ret = -1;
    RegQueryValueExA(hkey, valueName.c_str(), 0, &type, NULL, &size);

    if (type == REG_DWORD) {
      size = sizeof(long);
      ret = RegQueryValueExA(hkey, valueName.c_str(), 0, &type, (BYTE*)&value, &size);
    }

    RegCloseKey(hkey);
    return ret == ERROR_SUCCESS;
  }

  static bool get_reg_sz_value(HKEY base_key, const std::string& reg_path,
    const std::string& valueName, std::string& result) {
    HKEY hkey;
    LONG ret = RegOpenKeyExA(base_key, reg_path.c_str(), 0, KEY_QUERY_VALUE, &hkey);
    if (ret != ERROR_SUCCESS) return false;

    DWORD size = 0;
    DWORD type = REG_NONE;
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
