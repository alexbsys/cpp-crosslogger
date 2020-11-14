
#ifndef LOGGER_MODULE_DEFINITION_HEADER
#define LOGGER_MODULE_DEFINITION_HEADER

#include <log/logger_config.h>
#include <log/logger_pdetect.h>
#include <log/logger_sysinclib.h>

////////////////////  Module Definition components  ////////////////////

namespace logging {
namespace detail {

#ifdef LOG_PLATFORM_WINDOWS

class pe_image {
public:
  enum pe_signatures {
    PE_IMAGE_DOS_SIGNATURE = 0x5A4D,    // MZ
    PE_IMAGE_NT_SIGNATURE = 0x00004550  // PE00
  };

  enum pe_image_directory_entry_types {
    PE_IMAGE_DIRECTORY_ENTRY_EXPORT = 0,          // Export Directory
    PE_IMAGE_DIRECTORY_ENTRY_IMPORT = 1,          // Import Directory
    PE_IMAGE_DIRECTORY_ENTRY_RESOURCE = 2,        // Resource Directory
    PE_IMAGE_DIRECTORY_ENTRY_EXCEPTION = 3,       // Exception Directory
    PE_IMAGE_DIRECTORY_ENTRY_SECURITY = 4,        // Security Directory
    PE_IMAGE_DIRECTORY_ENTRY_BASERELOC = 5,       // Base Relocation Table
    PE_IMAGE_DIRECTORY_ENTRY_DEBUG = 6,           // Debug Directory
    PE_IMAGE_DIRECTORY_ENTRY_ARCHITECTURE = 7,    // Architecture Specific Data
    PE_IMAGE_DIRECTORY_ENTRY_GLOBALPTR = 8,       // RVA of GP
    PE_IMAGE_DIRECTORY_ENTRY_TLS = 9,             // TLS Directory
    PE_IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG = 10,    // Load Configuration Directory
    PE_IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT = 11,   // Bound Import Directory in headers
    PE_IMAGE_DIRECTORY_ENTRY_IAT = 12,            // Import Address Table
    PE_IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT = 13,   // Delay Load Import Descriptors
    PE_IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR = 14  // COM Runtime descriptor
  };

  static const int PE_IMAGE_NUMBEROF_DIRECTORY_ENTRIES = 16;

  typedef struct _pe_image_dos_header_t {  // DOS .EXE header
    unsigned short e_magic;                // Magic number
    unsigned short e_cblp;                 // Bytes on last page of file
    unsigned short e_cp;                   // Pages in file
    unsigned short e_crlc;                 // Relocations
    unsigned short e_cparhdr;              // Size of header in paragraphs
    unsigned short e_minalloc;             // Minimum extra paragraphs needed
    unsigned short e_maxalloc;             // Maximum extra paragraphs needed
    unsigned short e_ss;                   // Initial (relative) SS value
    unsigned short e_sp;                   // Initial SP value
    unsigned short e_csum;                 // Checksum
    unsigned short e_ip;                   // Initial IP value
    unsigned short e_cs;                   // Initial (relative) CS value
    unsigned short e_lfarlc;               // File address of relocation table
    unsigned short e_ovno;                 // Overlay number
    unsigned short e_res[4];               // Reserved words
    unsigned short e_oemid;                // OEM identifier (for e_oeminfo)
    unsigned short e_oeminfo;              // OEM information; e_oemid specific
    unsigned short e_res2[10];             // Reserved words
    unsigned long e_lfanew;                // File address of new exe header
  } pe_image_dos_header_t;

  typedef struct _pe_image_file_header_t {
    unsigned short Machine;
    unsigned short NumberOfSections;
    unsigned long TimeDateStamp;
    unsigned long PointerToSymbolTable;
    unsigned long NumberOfSymbols;
    unsigned short SizeOfOptionalHeader;
    unsigned short Characteristics;
  } pe_image_file_header_t;

  typedef struct _pe_image_data_directory_t {
    unsigned long VirtualAddress;
    unsigned long Size;
  } pe_image_data_directory_t;

  typedef struct _pe_image_optional_header64_t {
    unsigned short Magic;
    unsigned char MajorLinkerVersion;
    unsigned char MinorLinkerVersion;
    unsigned long SizeOfCode;
    unsigned long SizeOfInitializedData;
    unsigned long SizeOfUninitializedData;
    unsigned long AddressOfEntryPoint;
    unsigned long BaseOfCode;
    uint64_t ImageBase;
    unsigned long SectionAlignment;
    unsigned long FileAlignment;
    unsigned short MajorOperatingSystemVersion;
    unsigned short MinorOperatingSystemVersion;
    unsigned short MajorImageVersion;
    unsigned short MinorImageVersion;
    unsigned short MajorSubsystemVersion;
    unsigned short MinorSubsystemVersion;
    unsigned long Win32VersionValue;
    unsigned long SizeOfImage;
    unsigned long SizeOfHeaders;
    unsigned long CheckSum;
    unsigned short Subsystem;
    unsigned short DllCharacteristics;
    uint64_t SizeOfStackReserve;
    uint64_t SizeOfStackCommit;
    uint64_t SizeOfHeapReserve;
    uint64_t SizeOfHeapCommit;
    unsigned long LoaderFlags;
    unsigned long NumberOfRvaAndSizes;
    pe_image_data_directory_t DataDirectory[PE_IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
  } pe_image_optional_header64_t;

  typedef struct _pe_image_optional_header32_t {
    unsigned short Magic;
    unsigned char MajorLinkerVersion;
    unsigned char MinorLinkerVersion;
    unsigned long SizeOfCode;
    unsigned long SizeOfInitializedData;
    unsigned long SizeOfUninitializedData;
    unsigned long AddressOfEntryPoint;
    unsigned long BaseOfCode;
    unsigned long BaseOfData;
    unsigned long ImageBase;
    unsigned long SectionAlignment;
    unsigned long FileAlignment;
    unsigned short MajorOperatingSystemVersion;
    unsigned short MinorOperatingSystemVersion;
    unsigned short MajorImageVersion;
    unsigned short MinorImageVersion;
    unsigned short MajorSubsystemVersion;
    unsigned short MinorSubsystemVersion;
    unsigned long Win32VersionValue;
    unsigned long SizeOfImage;
    unsigned long SizeOfHeaders;
    unsigned long CheckSum;
    unsigned short Subsystem;
    unsigned short DllCharacteristics;
    unsigned long SizeOfStackReserve;
    unsigned long SizeOfStackCommit;
    unsigned long SizeOfHeapReserve;
    unsigned long SizeOfHeapCommit;
    unsigned long LoaderFlags;
    unsigned long NumberOfRvaAndSizes;
    pe_image_data_directory_t DataDirectory[PE_IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
  } pe_image_optional_header32_t;

  typedef struct _pe_image_nt_headers64_t {
    unsigned long Signature;
    pe_image_file_header_t FileHeader;
    pe_image_optional_header64_t OptionalHeader;
  } pe_image_nt_headers64_t;

  typedef struct _pe_image_nt_headers32_t {
    unsigned long Signature;
    pe_image_file_header_t FileHeader;
    pe_image_optional_header32_t OptionalHeader;
  } pe_image_nt_headers32_t;

#ifdef _WIN64
  typedef pe_image_nt_headers64_t pe_image_nt_headers_t;
#else
  typedef pe_image_nt_headers32_t pe_image_nt_headers_t;
#endif

public:
  static void* api_find_module_entry_point(intptr_t some_addr_in_module) {
#if !LOG_USE_SEH
    HMODULE h;
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
      (LPCTSTR)(void*)(some_addr_in_module), &h))
      return reinterpret_cast<void*>(h);

    return NULL;
#else  // LOG_USE_SEH
    const int max_cycles_find = 0x4000;
    const int max_exceptions_per_try = 0x100;

    int cycles = 0;
    int exceptions = 0;

#ifdef _AMD64_
    some_addr_in_module &= 0xFFFFFFFFFFFF0000;
#else   //_AMD64_
    some_addr_in_module &= 0xFFFF0000;
#endif  //_AMD64_

    while (1) {
      const pe_image_dos_header_t* doshdr = (pe_image_dos_header_t*)some_addr_in_module;

      __try {
        if (doshdr->e_magic == PE_IMAGE_DOS_SIGNATURE) {
          if (doshdr->e_lfanew < 0x100000) {
            const pe_image_nt_headers_t* nthdr =
              (pe_image_nt_headers_t*)(some_addr_in_module + doshdr->e_lfanew);
            if (nthdr->Signature == PE_IMAGE_NT_SIGNATURE) break;
          }
        }

        exceptions = 0;
      }
      __except (1) {
        ++exceptions;
      }

      ++cycles;
      some_addr_in_module -= 0x1000;

      if (cycles > max_cycles_find) return NULL;
      if (exceptions > max_exceptions_per_try) return NULL;
    }

    return reinterpret_cast<void*>(some_addr_in_module);
#endif  // LOG_USE_SEH
  }
}; //class pe_image
#endif  // LOG_PLATFORM_WINDOWS

struct module_entry_t {
  std::string image_name;
  std::string module_name;
  intptr_t base_address;
  unsigned long size;
  std::string file_version;
  std::string product_version;
  std::string file_description;
  std::string company_name;
  unsigned long lang_id;

  module_entry_t() : base_address(0), size(0), lang_id(0) {}
};

class module_definition {
public:

#ifdef LOG_PLATFORM_WINDOWS
  static std::string module_name_by_addr(void* function_address) {
    HMODULE module = reinterpret_cast<HMODULE>(pe_image::api_find_module_entry_point(
      reinterpret_cast<intptr_t>(function_address)));

    if (!module) return std::string();

    char file_name[MAX_PATH];
    GetModuleFileNameA(module, file_name, sizeof(file_name));
    return file_name;
  }
#else   // LOG_PLATFORM_WINDOWS
  static std::string module_name_by_addr(void* function_address) {
    Dl_info info;
    if (dladdr(functionAddress, &info)) {
      return info.dli_fname;
    }
    return std::string();
  }
#endif  // LOG_PLATFORM_WINDOWS

#ifdef LOG_PLATFORM_WINDOWS
  static bool module_query_version_info(HMODULE module, std::string& file_version,
    std::string& file_description,
    std::string& product_version,
    std::string& company_name,
    unsigned long& lang_id) {
    char szFilename[MAX_PATH + 1];

    if (GetModuleFileNameA(module, szFilename, MAX_PATH) == 0) {
      return false;
    }

    // allocate a block of memory for the version info
    DWORD dummy;
    DWORD obj_size = GetFileVersionInfoSizeA(szFilename, &dummy);
    if (obj_size == 0) {
      return false;
    }

    uint8_t* data = (uint8_t*)malloc(obj_size);
    if (!data) return false;

    // load the version info
    if (!GetFileVersionInfoA(szFilename, 0, obj_size, data)) {
      free(data);
      return false;
    }

    DWORD v_len, lang_d;
    BOOL ret_val;

    LPVOID retbuf = NULL;

    static char file_entry[256];

    sprintf(file_entry, "\\VarFileInfo\\Translation");
    ret_val = VerQueryValueA(data, file_entry, &retbuf, (UINT*)&v_len);
    if (ret_val && v_len == 4) {
      static char current_entry[256];

      memcpy(&lang_d, retbuf, 4);

      lang_id = lang_d;

      sprintf(file_entry, "\\StringFileInfo\\%02X%02X%02X%02X\\", (lang_d & 0xff00) >> 8,
        lang_d & 0xff, (lang_d & 0xff000000) >> 24, (lang_d & 0xff0000) >> 16);

      strcpy(current_entry, file_entry);
      strcat(current_entry, "FileVersion");

      if (VerQueryValueA(data, current_entry, &retbuf, (UINT*)&v_len))
        file_version = (char*)retbuf;

      strcpy(current_entry, file_entry);
      strcat(current_entry, "FileDescription");

      if (VerQueryValueA(data, current_entry, &retbuf, (UINT*)&v_len))
        file_description = (char*)retbuf;

      strcpy(current_entry, file_entry);
      strcat(current_entry, "ProductVersion");

      if (VerQueryValueA(data, current_entry, &retbuf, (UINT*)&v_len))
        product_version = (char*)retbuf;

      strcpy(current_entry, file_entry);
      strcat(current_entry, "CompanyName");

      if (VerQueryValueA(data, current_entry, &retbuf, (UINT*)&v_len))
        company_name = (char*)retbuf;
    }

    free(data);

    return true;
  }

#endif  // LOG_PLATFORM_WINDOWS

#if !defined(LOG_PLATFORM_WINDOWS)
  static int module_callback(struct dl_phdr_info* info, size_t size, void* data) {
    std::list<module_entry_t>* modules = (std::list<module_entry_t>*)data;

    if (!info || size < sizeof(struct dl_phdr_info)) return 0;

    if (!strlen(info->dlpi_name)) return 0;

    module_entry_t e;
    e.base_address = info->dlpi_addr;

    e.image_name = info->dlpi_name;
    e.module_name = info->dlpi_name;

    size_t last_delim = e.module_name.find_last_of('/');
    if (last_delim == std::string::npos)
      last_delim = 0;
    else
      last_delim++;

    e.module_name = e.module_name.substr(last_delim);

    e.size = 0;

    for (int j = 0; j < info->dlpi_phnum; j++) {
      uintptr_t ptr =
        info->dlpi_phdr[j].p_vaddr + info->dlpi_phdr[j].p_memsz;  // - info->dlpi_addr;

      if (e.size < ptr) e.size = ptr;
    }

    modules->push_back(e);
    return 0;
  }
#endif

  static bool query_module_list(std::list<module_entry_t>& modules) {
#ifdef LOG_PLATFORM_WINDOWS
    const int kMaxLoadedModules = 1024;
    modules.clear();

    HMODULE modules_handles[kMaxLoadedModules];
    DWORD bytes_needed;

    HANDLE process_handle = GetCurrentProcess();

    if (!EnumProcessModules(process_handle, modules_handles, kMaxLoadedModules * sizeof(HMODULE),
      &bytes_needed))
      return false;

    for (size_t i = 0; i < bytes_needed / sizeof(HMODULE); i++) {
      MODULEINFO moduleInfo;
      GetModuleInformation(process_handle, modules_handles[i], &moduleInfo, sizeof moduleInfo);

      module_entry_t e;
      e.base_address = reinterpret_cast<intptr_t>(moduleInfo.lpBaseOfDll);
      e.size = moduleInfo.SizeOfImage;

      char name[MAX_PATH];
      GetModuleFileNameExA(process_handle, modules_handles[i], name, sizeof(name));
      e.image_name = name;

      GetModuleBaseNameA(process_handle, modules_handles[i], name, sizeof(name));
      e.module_name = name;

      module_query_version_info(reinterpret_cast<HMODULE>(moduleInfo.lpBaseOfDll),
        e.file_version, e.file_description, e.product_version,
        e.company_name, e.lang_id);

      modules.push_back(e);
    }

#else   // LOG_PLATFORM_WINDOWS
    dl_iterate_phdr(&module_callback, &modules);
#endif  // LOG_PLATFORM_WINDOWS

    return true;
  }
}; //class module_definition



}//namespace detail
}//namespace logging



#endif /*LOGGER_MODULE_DEFINITION_HEADER*/
