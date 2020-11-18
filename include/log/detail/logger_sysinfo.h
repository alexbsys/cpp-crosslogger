
#ifndef LOGGER_SYSINFO_HEADER
#define LOGGER_SYSINFO_HEADER

#include <log/logger_config.h>
#include <log/logger_pdetect.h>
#include <log/logger_pdefs.h>
#include <log/logger_sysinclib.h>
#include <log/detail/logger_strutils.h>
#include <log/detail/logger_utils.h>

#ifdef LOG_PLATFORM_WINDOWS
#include <log/detail/logger_win_registry.h>
#endif /*LOG_PLATFORM_WINDOWS*/

#if LOG_USE_MODULEDEFINITION
#include <log/detail/logger_moddef.h>
#endif /*LOG_USE_MODULEDEFINITION*/

namespace logging {
namespace detail {
#ifdef LOG_PLATFORM_WINDOWS
/** System information helpers for Windows */
namespace sys_info_win {
  /**
  * \brief    Get processor architecture (Windows)
  * \return   Processor architecture information string
  */
  static std::string get_processor_arch() {
    using namespace logging::detail::str;

    SYSTEM_INFO si;
    ::GetSystemInfo(&si);

    std::string processor_info;

    switch (si.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_INTEL:
      processor_info.append("IA-32");
      break;
    case PROCESSOR_ARCHITECTURE_AMD64:
      processor_info.append("AMD64");
      break;
    case PROCESSOR_ARCHITECTURE_IA64:
      processor_info.append("IA64");
      break;
    case PROCESSOR_ARCHITECTURE_UNKNOWN:
    default:
      processor_info.append("Unknown");
    }

    processor_info.append(stringformat(", count: %d", si.dwNumberOfProcessors));
    return processor_info;
  }

  /**
  * \brief    Check is WOW64 active (Windows)
  * \return   true - active, false - not active
  */
  static bool is_wow64_used() {
    BOOL isWow;
    ::IsWow64Process(::GetCurrentProcess(), &isWow);
    return isWow ? true : false;
  }

#ifndef VER_SUITE_WH_SERVER
#define VER_SUITE_WH_SERVER 0x00008000
#endif

  /**
  * \brief    Get OS version information string (Windows). Query OS info from API calls
  * \param    os_version     Output string for OS version info
  * \return   true - OS information obtained, false - error
  */
  static bool get_os_version_display_string(std::string& os_version) {
    typedef void(WINAPI* GetNativeSystemInfo_t)(LPSYSTEM_INFO);
    typedef BOOL(WINAPI* GetProductInfo_t)(DWORD, DWORD, DWORD, DWORD, PDWORD);

    OSVERSIONINFOEXA osvi;
    SYSTEM_INFO si;
    DWORD type = PRODUCT_UNDEFINED;

    os_version = std::string();
    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);

    if (!GetVersionExA((OSVERSIONINFOA*)&osvi)) return false;

    GetNativeSystemInfo_t get_native_system_info_fn = reinterpret_cast<GetNativeSystemInfo_t>(
      GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetNativeSystemInfo"));

    if (get_native_system_info_fn != NULL) {
      get_native_system_info_fn(&si);
    }
    else {
      GetSystemInfo(&si);
    }

    if (VER_PLATFORM_WIN32_NT != osvi.dwPlatformId || osvi.dwMajorVersion <= 4)
      return false;

    os_version.append("Microsoft ");

    if (osvi.dwMajorVersion == 6) {
      if (osvi.dwMinorVersion == 0) {
        if (osvi.wProductType == VER_NT_WORKSTATION)
          os_version.append("Windows Vista ");
        else
          os_version.append("Windows Server 2008 ");
      }

      if (osvi.dwMinorVersion == 1) {
        if (osvi.wProductType == VER_NT_WORKSTATION)
          os_version.append("Windows 7 ");
        else
          os_version.append("Windows Server 2008 R2 ");
      }

      if (osvi.dwMinorVersion == 2) {
        if (osvi.wProductType == VER_NT_WORKSTATION)
          os_version.append("Windows 8 ");
        else
          os_version.append("Windows Server 2012 ");
      }

      if (osvi.dwMinorVersion == 3) {
        if (osvi.wProductType == VER_NT_WORKSTATION)
          os_version.append("Windows 8.1 ");
        else
          os_version.append("Windows Server 2012 R2 ");
      }

      GetProductInfo_t get_product_info_fn = reinterpret_cast<GetProductInfo_t>(
        GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetProductInfo"));

      if (get_product_info_fn) {
        get_product_info_fn(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &type);
      }

      switch (type) {
      case 0x0000 /*PRODUCT_UNDEFINED*/:               os_version.append("(Product undefined)"); break;
      case 0x0001 /*PRODUCT_ULTIMATE*/:                os_version.append("Ultimate");         break;
      case 0x0002 /*PRODUCT_HOME_BASIC*/:              os_version.append("Home Basic");       break;
      case 0x0003 /*PRODUCT_HOME_PREMIUM*/:            os_version.append("Home Premium");     break;
      case 0x0004 /*PRODUCT_ENTERPRISE*/:              os_version.append("Enterprise");       break;
      case 0x0005 /*PRODUCT_HOME_BASIC_N*/:            os_version.append("Home Basic N");     break;
      case 0x0006 /*PRODUCT_BUSINESS*/:                os_version.append("Business");         break;
      case 0x0007 /*PRODUCT_STANDARD_SERVER*/:         os_version.append("Standard Server");  break;
      case 0x0008 /*PRODUCT_DATACENTER_SERVER*/:       os_version.append("Datacenter");       break;
      case 0x0009 /*PRODUCT_SMALLBUSINESS_SERVER*/:    os_version.append("Small Business Server"); break;
      case 0x000A /*PRODUCT_ENTERPRISE_SERVER*/:       os_version.append("Enterprise");       break;
      case 0x000B /*PRODUCT_STARTER*/:                 os_version.append("Starter");          break;
      case 0x000C /*PRODUCT_DATACENTER_SERVER_CORE*/:  os_version.append("Datacenter Edition (core installation)");       break;
      case 0x000D /*PRODUCT_STANDARD_SERVER_CORE*/:    os_version.append("Standard Edition (core installation)");         break;
      case 0x000E /*PRODUCT_ENTERPRISE_SERVER_CORE*/:  os_version.append("Enterprise Edition (core installation)");       break;
      case 0x000F /*PRODUCT_ENTERPRISE_SERVER_IA64*/:  os_version.append("Enterprise Edition for Itanium-based Systems"); break;
      case 0x0010 /*PRODUCT_BUSINESS_N*/:              os_version.append("Business N");       break;
      case 0x0011 /*PRODUCT_WEB_SERVER*/:              os_version.append("Web Server");       break;
      case 0x0012 /*PRODUCT_CLUSTER_SERVER*/:          os_version.append("Cluster Server");   break;
      case 0x0013 /*PRODUCT_HOME_SERVER*/:             os_version.append("Home Server");      break;
      case 0x0014 /*PRODUCT_STORAGE_EXPRESS_SERVER*/:  os_version.append("Storage Express Server");      break;
      case 0x0015 /*PRODUCT_STORAGE_STANDARD_SERVER*/: os_version.append("Storage Standard Server");     break;
      case 0x0016 /*PRODUCT_STORAGE_WORKGROUP_SERVER*/:os_version.append("Storage Workgroup Server");    break;
      case 0x0017 /*PRODUCT_STORAGE_ENTERPRISE_SERVER*/: os_version.append("Storage Enterprise Server"); break;
      case 0x0018 /*PRODUCT_SERVER_FOR_SMALLBUSINESS*/:os_version.append("Server for Small Business");   break;
      case 0x0019 /*PRODUCT_SMALLBUSINESS_SERVER_PREMIUM*/: os_version.append("Small Business Server Premium");  break;
      case 0x001A /*PRODUCT_HOME_PREMIUM_N*/:          os_version.append("Home Premium N"); break;
      case 0x001B /*PRODUCT_ENTERPRISE_N*/:            os_version.append("Enterprise N");   break;
      case 0x001C /*PRODUCT_ULTIMATE_N*/:              os_version.append("Ultimate N");     break;
      case 0x001D /*PRODUCT_WEB_SERVER_CORE*/:         os_version.append("Web Server (core installation)"); break;
      case 0x0021 /*PRODUCT_SERVER_FOUNDATION*/:       os_version.append("Server Foundation");   break;
      case 0x0022 /*PRODUCT_HOME_PREMIUM_SERVER*/:     os_version.append("Home Premium Server"); break;
      case 0x0023 /*PRODUCT_SERVER_FOR_SMALLBUSINESS_V*/: os_version.append("Server for Small Business V"); break;
      case 0x0024 /*PRODUCT_STANDARD_SERVER_V*/:       os_version.append("Standard Server V");   break;
      case 0x0025 /*PRODUCT_DATACENTER_SERVER_V*/:     os_version.append("Datacenter V");        break;
      case 0x0026 /*PRODUCT_ENTERPRISE_SERVER_V*/:     os_version.append("Enterprise V");        break;
      case 0x0027 /*PRODUCT_DATACENTER_SERVER_CORE_V*/:os_version.append("Datacenter V (core installation)");        break;
      case 0x0028 /*PRODUCT_STANDARD_SERVER_CORE_V*/:  os_version.append("Standard Server V (core installation)");   break;
      case 0x0029 /*PRODUCT_ENTERPRISE_SERVER_CORE_V*/:os_version.append("Enterprise Server V (core installation)"); break;
      case 0x002A /*PRODUCT_HYPERV*/:                  os_version.append("HyperV");           break;
      case 0x002F /*PRODUCT_STARTER_N*/:               os_version.append("Starter N");        break;
      case 0x0030 /*PRODUCT_PROFESSIONAL*/:            os_version.append("Professional");     break;
      case 0x0031 /*PRODUCT_PROFESSIONAL_N*/:          os_version.append("Professional N");   break;
      case 0x0040 /*PRODUCT_CLUSTER_SERVER_V*/:        os_version.append("Cluster Server V"); break;
      case 0x0065 /*PRODUCT_CORE*/:                    os_version.append("Core");             break;
      case 0x006D /*PRODUCT_CORE_SERVER*/:             os_version.append("Core Server");      break;
      case 0xABCDABCD /*PRODUCT_UNLICENSED*/:          os_version.append("(Unlicensed)");     break;
      default:   os_version.append(str::stringformat("(Not decoded: %.4X)", type));   break;
      }
    }

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) {
      if (GetSystemMetrics(SM_SERVERR2))
        os_version.append("Windows Server 2003 R2, ");
      else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER)
        os_version.append("Windows Storage Server 2003");
      else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER)
        os_version.append("Windows Home Server");
      else if (osvi.wProductType == VER_NT_WORKSTATION &&
        si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
        os_version.append("Windows XP Professional x64 Edition");
      }
      else {
        os_version.append("Windows Server 2003, ");
      }

      // Test for the server type.
      if (osvi.wProductType != VER_NT_WORKSTATION) {
        if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
          if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
            os_version.append("Datacenter Edition for Itanium-based Systems");
          else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
            os_version.append("Enterprise Edition for Itanium-based Systems");
        }

        else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
          if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
            os_version.append("Datacenter x64 Edition");
          else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
            os_version.append("Enterprise x64 Edition");
          else
            os_version.append("Standard x64 Edition");
        }
        else {
          if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
            os_version.append("Compute Cluster Edition");
          else if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
            os_version.append("Datacenter Edition");
          else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
            os_version.append("Enterprise Edition");
          else if (osvi.wSuiteMask & VER_SUITE_BLADE)
            os_version.append("Web Edition");
          else
            os_version.append("Standard Edition");
        }
      }
    }

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
      os_version += "Windows XP ";
      if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
        os_version.append("Home Edition");
      else
        os_version.append("Professional");
    }

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) {
      os_version.append("Windows 2000 ");

      if (osvi.wProductType == VER_NT_WORKSTATION) {
        os_version.append("Professional");
      }
      else {
        if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
          os_version.append("Datacenter Server");
        else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
          os_version.append("Advanced Server");
        else
          os_version.append("Server");
      }
    }

    if (strlen(osvi.szCSDVersion) > 0) {
      os_version.append(" ");
      os_version.append(osvi.szCSDVersion);
    }

    os_version.append(str::stringformat(" (build %d)", osvi.dwBuildNumber));

    if (osvi.dwMajorVersion >= 6) {
      if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        os_version.append(", 64-bit");
      else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
        os_version.append(", 32-bit");
    }

    return true;
  }

  /**
  * \brief    Get OS version information from registry (Windows)
  * \param    os_version    Output string for OS information
  * \return   true or false
  */
  static bool get_os_version_display_string2(std::string& os_version) {
    
    bool result;
    os_version = std::string();

    std::string reg_product_name;
    std::string reg_edition_id;
    std::string reg_current_version;
    std::string reg_current_build_number;

    result = log_registry_helper::get_reg_sz_value(
      HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
      "ProductName", reg_product_name);
    result &= log_registry_helper::get_reg_sz_value(
      HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
      "CurrentVersion", reg_current_version);
    result &= log_registry_helper::get_reg_sz_value(
      HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
      "CurrentBuildNumber", reg_current_build_number);
    log_registry_helper::get_reg_sz_value(HKEY_LOCAL_MACHINE,
      "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
      "EditionID", reg_edition_id);
    if (result) {
      os_version = reg_product_name + std::string(" (") + reg_edition_id +
        std::string(") / Version ") + reg_current_version +
        std::string(" / Build ") + reg_current_build_number;
    }

    return result;
  }

  /**
  * \brief    Get host name (Windows)
  * \param    name    Output string for host name
  * \return   true or false
  */
  static bool get_host_name(std::string& name) {
    name = std::string();
    char buffer[MAX_PATH];
    DWORD size = sizeof(buffer);

    if (!::GetComputerNameExA(ComputerNamePhysicalDnsFullyQualified, buffer, &size)) {
      if (!::GetComputerNameA(buffer, &size)) return false;
    }

    name = buffer;
    return true;
  }

  /**
  * \brief    Get OS memory status (Windows)
  */
  static std::string get_memory_status() {
    std::string mem_info;
    MEMORYSTATUSEX mem_status;
    memset(&mem_status, 0, sizeof(MEMORYSTATUSEX));
    mem_status.dwLength = sizeof(MEMORYSTATUSEX);

    if (!GlobalMemoryStatusEx(&mem_status)) return "";

    mem_info =
      str::stringformat("Total physical " LOG_FMT_I64 ", Total virtual " LOG_FMT_I64,
        mem_status.ullTotalPhys, mem_status.ullTotalVirtual);

    mem_info += str::stringformat("\nAvailable physical " LOG_FMT_I64
      ", available virtual " LOG_FMT_I64 ", Load %d%%",
      mem_status.ullAvailPhys, mem_status.ullAvailVirtual,
      static_cast<int>(mem_status.dwMemoryLoad));

    mem_info += str::stringformat(
      "\nPage file size: " LOG_FMT_I64 ", page memory available " LOG_FMT_I64,
      mem_status.ullTotalPageFile, mem_status.ullAvailPageFile);
    return mem_info;
  }

  /**
  * \brief    Get user name information (Windows)
  */
  static std::string get_current_user_name() {
    char buf[MAX_PATH];
    unsigned long bufferSize = sizeof(buf);
    GetUserNameA(buf, &bufferSize);
    return buf;
  }

  /**
  * \brief    Get user SID information (Windows)
  */
  static std::string get_current_user_sid() {
    HANDLE token_handle;
    OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token_handle);
    if (token_handle == INVALID_HANDLE_VALUE)
      return std::string();

    TOKEN_USER* user_obj = NULL;
    char* sid = NULL;
    std::string result;

    do {
      DWORD len = 0;
      if (!GetTokenInformation(token_handle, TokenUser, 0, 0, &len))
        break;

      TOKEN_USER* user_obj = reinterpret_cast<TOKEN_USER*>(malloc(len));
      if (!user_obj)
        break;

      if (!GetTokenInformation(token_handle, TokenUser, user_obj, len, &len))
        break;

      if (ConvertSidToStringSidA(user_obj->User.Sid, &sid))
        result = sid;
    } while (0);

    CloseHandle(token_handle);

    if (sid) {
      LocalFree(sid);
      sid = NULL;
    }

    if (user_obj) {
      free(user_obj);
      user_obj = NULL;
    }

    return result;
  }
}//namespace sys_info_win
#endif /*LOG_PLATFORM_WINDOWS*/

#ifdef LOG_PLATFORM_POSIX_BASED

 /** System information for POSIX-based subsystems */
namespace sys_info_posix {
  /**
  * \brief    Get processor architecture (POSIX)
  * \return   Processor architecture information string
  */
  static std::string get_processor_arch() {
#ifdef LOG_HAVE_SYS_UTSNAME_H
    struct utsname osname;
    uname(&osname);
    return osname.machine;
#else   /*LOG_HAVE_SYS_UTSNAME_H*/
    if (sizeof(int) == 4) return "32-bit";
    if (sizeof(int) == 8) return "64-bit";
    if (sizeof(int) == 2) return "16-bit";
    if (sizeof(int) == 16) return "128-bit";
    return std::string();
#endif  /*LOG_HAVE_SYS_UTSNAME_H*/
  }


  static bool get_os_version_display_string(std::string& os_version) {
#ifdef LOG_HAVE_SYS_UTSNAME_H
    struct utsname osname;
    uname(&osname);

    os_version = osname.sysname;
    os_version += " ";
    os_version += osname.release;
    os_version += " ";
    os_version += osname.version;

#else  // LOG_HAVE_SYS_UTSNAME_H

    os_version = "Posix-based ";
#ifdef LOG_PLATFORM_UNIX
    os_version += "Unix ";
#endif  // LOG_PLATFORM_UNIX

#ifdef LOG_PLATFORM_BSD
    os_version += "BSD ";
#endif  // LOG_PLATFORM_BSD

#ifdef LOG_PLATFORM_LINUX
    os_version += "Linux ";
#endif  // LOG_PLATFORM_LINUX

#ifdef LOG_PLATFORM_CYGWIN
    os_version += "CYGWIN ";
#endif  // LOG_PLATFORM_CYGWIN

#ifdef LOG_PLATFORM_FREEBSD
    os_version += "FreeBSD ";
#endif  // LOG_PLATFORM_FREEBSD

#ifdef LOG_PLATFORM_NETBSD
    os_version += "NetBSD ";
#endif  // LOG_PLATFORM_NETBSD

#ifdef LOG_PLATFORM_DRAGONFLYBSD
    os_version += "DragonFlyBSD";
#endif  // LOG_PLATFORM_DRAGONFLYBSD

#ifdef LOG_PLATFORM_SOLARIS
    os_version += "Solaris ";
#endif  // LOG_PLATFORM_SOLARIS

#ifdef LOG_PLATFORM_MAC
    os_version += "Mac-based ";
#endif  // LOG_PLATFORM_MAC

#ifdef LOG_PLATFORM_MACOSX
    os_version += "MacOSX ";
#endif  // LOG_PLATFORM_MACOSX

#ifdef LOG_PLATFORM_IPHONE
    os_version += "IPhone ";
#endif  // LOG_PLATFORM_IPHONE

#ifdef LOG_PLATFORM_IPHONESIMULATOR
    os_version += "Simulator ";
#endif  // LOG_PLATFORM_IPHONESIMULATOR

#endif  // LOG_HAVE_SYS_UTSNAME_H

    return true;
  }

  /**
  * \brief    Get host name (POSIX)
  */
  static bool get_host_name(std::string& name) {
    name = std::string();

#ifdef LOG_HAVE_SYS_UTSNAME_H
    struct utsname osname;
    uname(&osname);
    name = osname.nodename;
#endif  // LOG_HAVE_SYS_UTSNAME_H
    return true;
  }

  /**
  * \brief    Get OS memory status (POSIX)
  */
  static std::string get_memory_status() {
    std::string memoryString;

#ifdef LOG_HAVE_SYS_SYSINFO_H
    struct sysinfo info;
    sysinfo(&info);

    memoryString = str::stringformat("Total avail RAM %d, total swap %d, total high %d",
      info.totalram, info.totalswap, info.totalhigh);
    memoryString += str::stringformat("\nFree RAM %d, free swap %d, free high %d", info.freeram,
      info.freeswap, info.freehigh);
    memoryString += str::stringformat("\nMem used by buffers %d, shared RAM %d", info.bufferram,
      info.sharedram);
    memoryString += str::stringformat("\nMemory unit in bytes %d, process count: %d",
      info.mem_unit, info.procs);
    memoryString += str::stringformat("\nAvg load 1 min : 5 min : 15 min  --  %d:%d:%d",
      info.loads[0], info.loads[1], info.loads[2]);
#endif  // LOG_HAVE_SYS_SYSINFO_H
    return memoryString;
  }

  /**
  * \brief    Get user name information
  */
  static std::string get_current_user_name() {
#ifdef LOG_HAVE_PWD_H
    struct passwd* pass;
    pass = getpwuid(getuid());

    if (!pass) return "(unknown)";

    return pass->pw_name;
#else   // LOG_HAVE_PWD_H
    return "(cannot be obtained)";
#endif  // LOG_HAVE_PWD_H
  }

  static std::string get_current_user_sid() {
#ifdef LOG_HAVE_PWD_H
    std::string result = str::stringformat("%d", getuid());

    struct passwd* pass;
    pass = getpwuid(getuid());
    if (pass) {
      result += std::string(":");
      result += str::stringformat("%d", pass->pw_gid);
    }

    return result;
#else   // LOG_HAVE_PWD_H
    return std::string();
#endif  // LOG_HAVE_PWD_H
  }
} //namespace sys_info_posix
#endif /*LOG_PLATFORM_POSIX_BASED*/


static std::string query_system_info() {
  std::stringstream system_info;
  system_info << "*********** SYSTEM INFORMATION *************" << std::endl;

  std::string os_info;

#ifdef LOG_PLATFORM_WINDOWS
  namespace sys_info = sys_info_win;

  if (sys_info::get_os_version_display_string(os_info))
    system_info << "OS: " << os_info << std::endl;

  if (sys_info_win::get_os_version_display_string2(os_info))
    system_info << "OS from registry: " << os_info << std::endl;

  system_info << "Processor architecture: " << sys_info::get_processor_arch() << std::endl;
  if (sys_info_win::is_wow64_used()) system_info << "Used WOW64" << std::endl;

  system_info << str::stringformat("Current tick count: %d", GetTickCount()) << std::endl;

#endif /*LOG_PLATFORM_WINDOWS*/

#ifdef LOG_PLATFORM_POSIX_BASED
  namespace sys_info = sys_info_posix;

  if (sys_info_posix::get_os_version_display_string(os_info))
    system_info << "OS: " << os_info << std::endl;

  system_info << "Processor architecture: " << sys_info::get_processor_arch() << std::endl;

#ifdef LOG_HAVE_SYS_SYSINFO_H
  struct sysinfo info;
  sysinfo(&info);
  system_info << str::stringformat("Uptime, seconds: %d", info.uptime) << std::endl;
#endif  // LOG_HAVE_SYS_SYSINFO_H

#endif /*LOG_PLATFORM_POSIX_BASED*/

  std::string host_name;
  if (sys_info::get_host_name(host_name)) system_info << "Host Name: " << host_name << std::endl;

  system_info << "Current user SID: " << sys_info::get_current_user_sid() << std::endl;
  system_info << "Current user name: " << sys_info::get_current_user_name() << std::endl;

  system_info << "--- MEMORY ---" << std::endl;
  system_info << sys_info::get_memory_status() << std::endl;

  system_info << "--- CURRENT MODULE ---" << std::endl;

  std::string module_name = utils::get_process_file_name();

#if LOG_USE_MODULEDEFINITION
  std::string detected_name =
    module_definition::module_name_by_addr((void*)&query_system_info);
  if (detected_name.length()) module_name = detected_name;
#endif  // LOG_USE_MODULEDEFINITION

  system_info << "Module Name: " << module_name << std::endl;

#if LOG_USE_MODULEDEFINITION && defined(LOG_PLATFORM_WINDOWS)
  HMODULE module = reinterpret_cast<HMODULE>(pe_image::api_find_module_entry_point(
    reinterpret_cast<intptr_t>(logging_get_caller_address())));

  std::string file_version, file_description;
  std::string product_version, company_name;
  unsigned long lang_id = 0;

  if (module_definition::module_query_version_info(module, file_version, file_description,
    product_version, company_name,
    lang_id)) {
    if (file_version.size())
      system_info << "Module file version: " << file_version << std::endl;

    if (file_description.size())
      system_info << "Module file description: " << file_description << std::endl;

    if (product_version.size())
      system_info << "Product version: " << product_version << std::endl;

    if (company_name.size())
      system_info << "Module company name: " << company_name << std::endl;

    if (lang_id)
      system_info << "Module language ID: " << str::stringformat("%.X", lang_id) << std::endl;
  }
#endif  // LOG_USE_MODULEDEFINITION && defined(LOG_PLATFORM_WINDOWS)

#ifdef LOG_PLATFORM_WINDOWS
  system_info << "--- SYSTEM PATHS ---" << std::endl;

  char buffer[MAX_PATH + 1];
  GetWindowsDirectoryA(buffer, MAX_PATH);
  system_info << "Windows dir: " << buffer << std::endl;

  GetSystemDirectoryA(buffer, MAX_PATH);
  system_info << "System dir: " << buffer << std::endl;

  GetTempPathA(MAX_PATH, buffer);
  system_info << "Temp dir: " << buffer << std::endl;
#endif  // LOG_PLATFORM_WINDOWS

  system_info << "********************************************" << std::endl;
  return system_info.str();
}

}//namespace detail
}//namespace logging

#endif /*LOGGER_SYSINFO_HEADER*/
