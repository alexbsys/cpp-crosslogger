#ifndef MEM_CHECKER_H
#define MEM_CHECKER_H

#include <cstdint>
#include <cstddef>
#include "platform_detect.h"

#ifdef PDETECT_PLATFORM_WINDOWS
#include <crtdbg.h>
#endif //PDETECT_PLATFORM_WINDOWS

size_t MemCheckerGlobalGetCurrentUsage();

class MemChecker {
public:
  MemChecker();
  ~MemChecker();
  int64_t GetCurrentUsage() const;
private:
#ifdef PDETECT_PLATFORM_WINDOWS
  struct _CrtMemState mem_state_;
#else // PDETECT_PLATFORM_WINDOWS
  int64_t total_allocated_mem_;
#endif //PDETECT_PLATFORM_WINDOWS
};

#endif // MEM_CHECKER_H
