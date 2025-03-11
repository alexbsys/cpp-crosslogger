#include "mem_checker.h"

#include <iostream>

#ifdef PDETECT_PLATFORM_WINDOWS
#include <crtdbg.h>
#else // PDETECT_PLATFORM_WINDOWS

#ifndef PDETECT_PLATFORM_APPLE
#include <malloc.h>
#else //PDETECT_PLATFORM_APPLE
#include <malloc/malloc.h>
#endif //PDETECT_PLATFORM_APPLE

#endif //PDETECT_PLATFORM_WINDOWS

MemChecker* g_mem_checker = nullptr;

size_t MemCheckerGlobalGetCurrentUsage() {
  if (!g_mem_checker)
    return 0;
  return g_mem_checker->GetCurrentUsage();
}

MemChecker::MemChecker() {
  if (g_mem_checker == nullptr)
    g_mem_checker = this;

#ifdef PDETECT_PLATFORM_WINDOWS
  _CrtMemCheckpoint(&mem_state_);
#else // PDETECT_PLATFORM_WINDOWS

#ifndef PDETECT_PLATFORM_APPLE
  struct mallinfo mi;
  mi = mallinfo();
  total_allocated_mem_ = mi.uordblks;
#else //PDETECT_PLATFORM_APPLE
  struct mstats ms = mstats();
  total_allocated_mem_ = ms.bytes_used;
#endif //PDETECT_PLATFORM_APPLE

#endif // PDETECT_PLATFORM_WINDOWS
}

int64_t MemChecker::GetCurrentUsage() const {
#ifdef PDETECT_PLATFORM_WINDOWS
  _CrtMemState state_now;
  _CrtMemState state_diff;
  _CrtMemCheckpoint(&state_now);
  int diff_result = _CrtMemDifference(&state_diff, &mem_state_, &state_now);

  if (!diff_result)
    return 0;

  return static_cast<int64_t>(state_diff.lSizes[1]);
#else // PDETECT_PLATFORM_WINDOWS

#ifndef PDETECT_PLATFORM_APPLE
  struct mallinfo mi;
  mi = mallinfo();
  return static_cast<int64_t>(mi.uordblks) - total_allocated_mem_;
#else //PDETECT_PLATFORM_APPLE
  struct mstats ms = mstats();
  return static_cast<int64_t>(ms.bytes_used) - total_allocated_mem_;
#endif //PDETECT_PLATFORM_APPLE

#endif // PDETECT_PLATFORM_WINDOWS
}

MemChecker::~MemChecker() {
  int diff_result = 0;
  int64_t allocated_mem = 0;

#ifdef PDETECT_PLATFORM_WINDOWS
  _CrtMemState state_now;
  _CrtMemState state_diff;
  _CrtMemCheckpoint(&state_now);
  diff_result = _CrtMemDifference(&state_diff, &mem_state_, &state_now);
  allocated_mem = static_cast<int64_t>(state_diff.lSizes[1]);
#else // PDETECT_PLATFORM_WINDOWS

#ifndef PDETECT_PLATFORM_APPLE
  struct mallinfo mi;
  mi = mallinfo();
  allocated_mem = static_cast<int64_t>(mi.uordblks) - total_allocated_mem_;
  diff_result = allocated_mem != total_allocated_mem_;
#else //PDETECT_PLATFORM_APPLE
  struct mstats ms = mstats();
  allocated_mem = static_cast<int64_t>(ms.bytes_used) - total_allocated_mem_;
  diff_result = allocated_mem != total_allocated_mem_;
#endif //PDETECT_PLATFORM_APPLE

#endif // PDETECT_PLATFORM_WINDOWS

  if (diff_result) {
    std::cerr << std::endl << "[MEMLEAKDETECTOR] Still allocated " << allocated_mem << " byte(s) detected." << std::endl;
    std::cerr << "  Memory can be used by global objects and STL internals, so small values do not mean real leakage" << std::endl;
  }

  if (g_mem_checker == this)
    g_mem_checker = nullptr;
}
