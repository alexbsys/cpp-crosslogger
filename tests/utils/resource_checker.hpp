
#ifndef RESOURCE_CHECKER_HPP
#define RESOURCE_CHECKER_HPP

// Requires: gtest tools_common

#include <gtest/gtest.h>
#include "mem_checker.h"

#include <chrono>
#include <thread>
#include <cinttypes>

#include <sstream>
#include "cprintf.h"

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#else //_WIN32
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#endif //_WIN32

namespace testing  {
class ResourceCheckerShowUsage : public EmptyTestEventListener {
public:
  virtual void OnTestStart(const TestInfo& test_info) override {
    size_t current_usage = MemCheckerGlobalGetCurrentUsage();
    colored_printf(nullptr, "%fg ", "[ MEMORY   ]");
    printf("Mem usage before test '%s' %d bytes\n",
           test_info.name(),
           static_cast<int>(current_usage));
    start_mem_usage_ = current_usage;
    start_objects_usage_ = 0;
    //LOG_OBJMON_GET_REGISTERED_OBJECTS_COUNT(&start_objects_usage_);

    start_threads_count_ = GetThreadsCount();
    start_handles_count_ = GetHandlesCount();
  }

  virtual void OnTestEnd(const TestInfo& test_info) override {
    size_t current_usage = MemCheckerGlobalGetCurrentUsage();
    int64_t diff = static_cast<int64_t>(current_usage) - static_cast<int64_t>(start_mem_usage_);

    size_t retries = kMaxWaitTimes;

    while(diff && retries--) {
      auto kWaitDuration = std::chrono::milliseconds(kWaitTimeMs);
      std::this_thread::sleep_for(kWaitDuration);

      current_usage = MemCheckerGlobalGetCurrentUsage();
      diff = static_cast<int64_t>(current_usage) - static_cast<int64_t>(start_mem_usage_);
    }

    colored_printf(nullptr, "%fg ", "[ MEMORY   ]");

    std::stringstream ss;
    ss << static_cast<int>(current_usage);
    std::string bytes_string = ss.str();
    
    ss = {};
    ss << diff;
    std::string diff_string = ss.str();

    int current_objects_usage = 0;
    //LOG_OBJMON_GET_REGISTERED_OBJECTS_COUNT(&current_objects_usage);

    if (diff > 1024) {
      colored_printf(nullptr, "Usage after test '%fW' %fr bytes, diff %fr bytes\n", test_info.name(),
           bytes_string.c_str(), diff_string.c_str());
    } else if (diff > 0) {
      colored_printf(nullptr, "Usage after test '%fW' %fy bytes, diff %fy bytes\n", test_info.name(),
           bytes_string.c_str(), diff_string.c_str());
    } else {
      colored_printf(nullptr, "Usage after test '%fW' %fg bytes, diff %fg bytes\n", test_info.name(),
           bytes_string.c_str(), diff_string.c_str());
    }
    /*
    int objects_alive_left = current_objects_usage - start_objects_usage_;
    if (objects_alive_left == 0) {
      std::string objects_string = stringformat("No left alive (%d on start)", objects_alive_left, start_objects_usage_);
      colored_printf(nullptr, "%fg %fw\n", "[ OBJECTS  ]", objects_string.c_str());
    } else {
      std::string objects_string = stringformat("%d objects left alive (%d on start, %d on finish)", objects_alive_left, start_objects_usage_, current_objects_usage);
      colored_printf(nullptr, "%fy %fy\n", "[ OBJECTS  ]", objects_string.c_str());
      LOG_OBJMON_DUMP_CONSOLE();
    }*/

    size_t current_threads = GetThreadsCount();
    size_t current_handles = GetHandlesCount();
    int threads_alive_left = static_cast<int>(current_threads) - static_cast<int>(start_threads_count_);
    int handles_alive_left = static_cast<int>(current_handles) - static_cast<int>(start_handles_count_);

    ss = {};
    ss << threads_alive_left << " threads alive (" << static_cast<int>(start_threads_count_) << "->" << static_cast<int>(current_threads) << "), ";
    ss << handles_alive_left << " handles alive (" << static_cast<int>(start_handles_count_) << "->" << static_cast<int>(current_handles) << ")";

    std::string res_string = ss.str();
    colored_printf(nullptr, "%fg %fw\n", "[ RESOURCE ]", res_string.c_str());
  }

private:
  size_t GetThreadsCount() {
#ifdef _WIN32
    size_t threads_count = 0;

    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (h == INVALID_HANDLE_VALUE)
      return 0;

    DWORD our_process_id = GetCurrentProcessId();

    THREADENTRY32 te;
    te.dwSize = sizeof(te);
    if (Thread32First(h, &te)) {
      do {
        if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
            sizeof(te.th32OwnerProcessID)) {
          if (te.th32OwnerProcessID == our_process_id)
            threads_count++;
        }
        te.dwSize = sizeof(te);
      } while (Thread32Next(h, &te));
    }
    CloseHandle(h);

    return threads_count;
#else // _WIN32
    size_t threads_count = 0;

    DIR *proc_dir;
    {
        char dirname[100];
        snprintf(dirname, sizeof dirname, "/proc/%d/task", getpid());
        proc_dir = opendir(dirname);
    }

    if (proc_dir)
    {
        /* /proc available, iterate through tasks... */
        struct dirent *entry;
        while ((entry = readdir(proc_dir)) != NULL)
        {
            if(entry->d_name[0] == '.')
                continue;

            threads_count++;
//            int tid = atoi(entry->d_name);

        }

        closedir(proc_dir);
    }

    return threads_count;
#endif //_WIN32
  }

  size_t GetHandlesCount() {
#ifdef _WIN32
    auto process = GetCurrentProcess();
    DWORD handles = 0;
    if (GetProcessHandleCount(process, &handles)) {
      return static_cast<size_t>(handles);
    }
    return 0;
#else // _WIN32
    int fd_count;
    char buf[64];
    struct dirent *dp;

    snprintf(buf, 64, "/proc/%i/fd/", getpid());

    fd_count = 0;
    DIR *dir = opendir(buf);
    while (dir && (dp = readdir(dir)) != NULL) {
         fd_count++;
    }
    if (dir)
      closedir(dir);
    return static_cast<size_t>(fd_count);
#endif //_WIN32
  }

private:
  const size_t kMaxWaitTimes = 20;
  const size_t kWaitTimeMs = 50;
  size_t start_mem_usage_ = 0;
  int start_objects_usage_ = 0;
  size_t start_threads_count_ = 0;
  size_t start_handles_count_ = 0;
};
} //namespace testing


#endif //RESOURCE_CHECKER_HPP
