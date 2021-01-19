
// Logger implementation part
// This file must be included from C++ part

#ifndef LOGGER_SHARED_MEMORY_HEADER
#define LOGGER_SHARED_MEMORY_HEADER

#include <log/logger_config.h>
#include <log/logger_pdetect.h>
#include <log/logger_pdefs.h>
#include <log/logger_sysinclib.h>

#include <stdint.h>

#if LOG_SHARED && !defined(LOG_PLATFORM_WINDOWS)
#include <sys/mman.h>
#define SH_MEM_READ 1
#define SH_MEM_WRITE 2
#define SH_MEM_EXEC 4
#endif  // LOG_SHARED && !defined(LOG_PLATFORM_WINDOWS)

namespace logging {
namespace detail {


// logger shared page structure
// page+0   4  signature_1  (0x12345678)
// page+4   4  signature_2  (0xa0b0c0d0)
// page+8   4  reserved
// page+12  4  reserved
// page+16  4/8  pointer to logger

/**
* \struct    shared_obj helper. Implements working with 'shared' logger in memory.
*            'shared' logger means that first instance of logger (from exe or dll) registers as 'shared'.
*            All dlls which will be loaded laterly will use only 'shared' instance of logger through logger_interface*
*            pointer
*/
struct shared_obj {
  /** Logger shared memory page start */
  static const intptr_t start_shared_page = 0x04320000;
  static const int shared_page_size = 0x100000;
  static const int shared_pages_find = 0x30;

  /** Size of page in shared memory for logger */
  static const unsigned long shared_page_mem_size = 0x100;

  /** Signature for detect shared instance of logger */
  static const unsigned long shared_mem_signature_1 = 0x12345678;

  /** Second signature for detect shared instance of logger (to prevent unexpected coincidence) */
  static const unsigned long shared_mem_signature_2 = 0xA0B0C0D0;

#ifndef LOG_PLATFORM_WINDOWS
  /**
  * \brief    Get shared memory page bits from /proc/self/maps
  * \param    addr   Address in memory
  * \param    bits   Result value which is combination of SH_MEM_READ, SH_MEM_WRITE, SH_MEM_EXEC
  */
  static inline bool get_page_bits(void* addr, int* bits) {
    // On linux, first check /proc/self/maps
    unsigned long laddr = reinterpret_cast<unsigned long>(addr);

    FILE* pF = fopen("/proc/self/maps", "r");
    if (pF) {
      // Linux /proc/self/maps -> parse
      // Format:
      // lower    upper    prot     stuff                 path
      // 08048000-0804c000 r-xp 00000000 03:03 1010107    /bin/cat
      unsigned long rlower, rupper;
      char r, w, x;
      while (fscanf(pF, "%lx-%lx %c%c%c", &rlower, &rupper, &r, &w, &x) != EOF) {
        // Check whether we're IN THERE!
        if (laddr >= rlower && laddr < rupper) {
          fclose(pF);
          *bits = 0;
          if (r == 'r') *bits |= SH_MEM_READ;
          if (w == 'w') *bits |= SH_MEM_WRITE;
          if (x == 'x') *bits |= SH_MEM_EXEC;

          return true;
        }
        // Read to end of line
        int c;
        while ((c = fgetc(pF)) != '\n') {
          if (c == EOF) break;
        }
        if (c == EOF) break;
      }
      fclose(pF);
      return false;
    }

    pF = fopen("/proc/curproc/map", "r");
    if (pF) {
      // FreeBSD /proc/curproc/map
      // 0x804800 0x805500 13 15 0xc6e18960 r-x 21 0x0 COW NC vnode
      unsigned long rlower, rupper, ignoreLong;
      int ignoreInt;
      char r, w, x;
      while (fscanf(pF, "0x%lx 0x%lx %d %d 0x%lx %c%c%c", &rlower, &rupper, &ignoreInt,
        &ignoreInt, &ignoreLong, &r, &w, &x) != EOF) {
        if (laddr >= rlower && laddr < rupper) {
          fclose(pF);
          *bits = 0;
          if (r == 'r') *bits |= SH_MEM_READ;
          if (w == 'w') *bits |= SH_MEM_WRITE;
          if (x == 'x') *bits |= SH_MEM_EXEC;
          return true;
        }

        // Read to end of line
        int c;
        while ((c = fgetc(pF)) != '\n') {
          if (c == EOF) break;
        }
        if (c == EOF) break;
      }
      fclose(pF);
      return false;
    }
    return false;
  }
#endif  // LOG_PLATFORM_WINDOWS

  /**
  * \brief   Create object in shared memory
  * \param   object_id       Shared object ID. Can be any value, but the same value must be used for
  *                          find shared object
  * \param   object          Pointer to object
  * \return  Pointer to created shared object
  * \todo    object_id must be 0 for now
  */
  static void* create_shared_object(unsigned char object_id, void* object) {
    intptr_t shared_page = get_start_address(object_id);
    void* result_ptr = NULL;
    void* invalid_ptr = (void*)((long)-1);

    for (intptr_t page = shared_page;
      page < shared_page + (shared_pages_find * shared_page_size);
      page += shared_page_size) {
#ifdef LOG_PLATFORM_WINDOWS
      MEMORY_BASIC_INFORMATION mem_info;
      if (!VirtualQuery((LPCVOID)page, &mem_info, sizeof(MEMORY_BASIC_INFORMATION)))
        continue;

      if (mem_info.State != MEM_FREE) continue;

      result_ptr = VirtualAlloc((LPVOID)page, shared_page_mem_size,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else   // LOG_PLATFORM_WINDOWS
      int page_bits;

#if !defined(LOG_PLATFORM_ANDROID)
      if (get_page_bits((void*)page, &page_bits)) continue;
#endif

      result_ptr =
        mmap((void*)page, shared_page_mem_size, PROT_READ | PROT_WRITE | PROT_EXEC,
          MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0);
#endif  // LOG_PLATFORM_WINDOWS

      if (result_ptr && result_ptr != invalid_ptr) {
        memset(result_ptr, 0, shared_page_mem_size);

        unsigned int* ptr = (unsigned int*)page;
        *ptr = shared_mem_signature_1;
        ptr++;

        *ptr = shared_mem_signature_2;
        *(intptr_t*)(page + (4*sizeof(unsigned int))) = (intptr_t)object;

        break;
      }
    }

    return result_ptr;
  }

  /**
  * \brief    Try find shared object page by object type value
  * \param    object_id    Shared object ID value which used in create_shared_object method
  * \return   Page ID or NULL
  * \todo     object_id actually not used for now
  */
  static intptr_t try_find_shared_object_page(unsigned short object_id) {
    (void)object_id;

    for (intptr_t page = start_shared_page;
      page < start_shared_page + (shared_pages_find * shared_page_size);
      page += shared_page_size) {

#ifdef LOG_PLATFORM_WINDOWS
      MEMORY_BASIC_INFORMATION mem_info;
      if (!VirtualQuery((LPCVOID)page, &mem_info, sizeof(MEMORY_BASIC_INFORMATION)))
        continue;

      if (mem_info.State == MEM_COMMIT && mem_info.Protect & PAGE_READWRITE)
#else   // LOG_PLATFORM_WINDOWS
      int page_bits;
      if (!get_page_bits((void*)page, &page_bits)) continue;

      if ((page_bits & SH_MEM_READ) && (page_bits & SH_MEM_WRITE))
#endif  // LOG_PLATFORM_WINDOWS
      {
        if (*((unsigned int*)page) != shared_mem_signature_1) continue;
        if (*((unsigned int*)page + 1) != shared_mem_signature_2) continue;

        return page;
      }
    }
    return (intptr_t)NULL;
  }

  /**
  * \brief    Try find shared object by ID
  * \param    object_id   shared object ID
  * \return   Pointer to found object or NULL
  */
  static void* try_find_shared_object(unsigned short object_id) {
    intptr_t page = try_find_shared_object_page(object_id);
    if (!page) return NULL;

    intptr_t* ptr = (intptr_t*)(page + (4*sizeof(unsigned int)));
    return (void*)*ptr;
  }

  /**
  * \brief    Release shared object by shared object ID
  * \param    object_id     Shared object ID
  */
  static bool free_shared_object(unsigned char object_id) {
    void* obj = (void*)try_find_shared_object_page(object_id);
    if (!obj) return false;

#ifdef LOG_PLATFORM_WINDOWS
    VirtualFree(obj, 0, MEM_DECOMMIT);
    return VirtualFree(obj, 0, MEM_RELEASE) ? true : false;
#else   // LOG_PLATFORM_WINDOWS
    return munmap(obj, shared_page_mem_size) == 0;
#endif  // LOG_PLATFORM_WINDOWS
  }

private:
  static intptr_t get_start_address(unsigned char object_id) {
    return start_shared_page + ((intptr_t)object_id << 22);
  }
};

}//namespace detail
}//namespace logging

#endif /*LOGGER_SHARED_MEMORY_HEADER*/
