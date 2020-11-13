#ifndef LOGGER_UTILS_HEADER
#define LOGGER_UTILS_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_sysinclib.h"

namespace logging {
namespace detail {

class utils {
public:
  static void set_thread_name(unsigned long thread_id, const char* thread_name) {
#ifdef LOG_PLATFORM_WINDOWS
    const unsigned long MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push, 8)
    typedef struct tagTHREADNAME_INFO {
      DWORD dwType;      // Must be 0x1000.
      LPCSTR szName;     // Pointer to name (in user addr space).
      DWORD dwThreadID;  // Thread ID (-1=caller thread).
      DWORD dwFlags;     // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = thread_name;
    info.dwThreadID = thread_id;
    info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable : 6320 6322)
    __try {
      RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR),
        reinterpret_cast<ULONG_PTR*>(&info));
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
#pragma warning(pop)

#endif /*LOG_PLATFORM_WINDOWS*/
  }

  static void set_current_thread_name(const char* thread_name) {
#ifdef LOG_PLATFORM_WINDOWS
    set_thread_name(GetCurrentThreadId(), thread_name);
#endif /*LOG_PLATFORM_WINDOWS*/
  }

  static struct tm get_time(int& millisec) {
    struct tm newtime;

#ifdef LOG_PLATFORM_WINDOWS

#ifdef LOG_COMPILER_MSVC
    struct __timeb64 tstruct;
    _ftime64(&tstruct);
    _localtime64_s(&newtime, &tstruct.time);
#else   // LOG_COMPILER_MSVC

    struct timeb tstruct;
    ftime(&tstruct);
    newtime = *localtime(&tstruct.time);
#endif  // LOG_COMPILER_MSVC

    millisec = tstruct.millitm;

#else   // LOG_PLATFORM_WINDOWS
    time_t tim = time(NULL);
    struct tm* loc_time = localtime(&tim);

    memcpy(&newtime, loc_time, sizeof(struct tm));

    struct timeval tv;
    gettimeofday(&tv, NULL);
    millisec = tv.tv_usec / 1000;
#endif  // LOG_PLATFORM_WINDOWS

    return newtime;
  }

  static void create_directory_path(const std::string& path) {
#ifdef LOG_PLATFORM_WINDOWS
    SHCreateDirectoryExA(NULL, path.c_str(), NULL);
#else   // LOG_PLATFORM_WINDOWS
    mkpath(path.c_str(), 0777);
#endif  // LOG_PLATFORM_WINDOWS
  }


  static void delete_file(const std::string& file_path_for_delete) {
#ifdef LOG_PLATFORM_WINDOWS
    DeleteFileA(file_path_for_delete.c_str());
#else  // LOG_PLATFORM_WINDOWS

#ifdef LOG_HAVE_UNISTD_H
    unlink(file_path_for_delete.c_str());
#else   // LOG_HAVE_UNISTD_H
    std::remove(file_path_for_delete.c_str());
#endif  // LOG_HAVE_UNISTD_H
#endif /*LOG_PLATFORM_WINDOWS*/
  }

  static void move_file(const std::string& src_path, const std::string& dest_path) {
#ifdef LOG_PLATFORM_WINDOWS
    MoveFileExA(src_path.c_str(), dest_path.c_str(),
      MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING);
#else   // LOG_PLATFORM_WINDOWS
    rename(src_path.c_str(), dest_path.c_str());
#endif  // LOG_PLATFORM_WINDOWS
  }

#ifndef LOG_PLATFORM_WINDOWS

  static std::string do_readlink(std::string const& path) {
    char buff[1024];

#ifdef LOG_HAVE_UNISTD_H
    ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff) - 1);
#else   // LOG_HAVE_UNISTD_H
    ssize_t len = -1;
#endif  // LOG_HAVE_UNISTD_H

    if (len != -1) {
      buff[len] = '\0';
      return std::string(buff);
    }

    return std::string();
  }


  static int do_mkdir(const char* path, mode_t mode) {
    struct stat st;
    int status = 0;

    if (stat(path, &st) != 0) {
      /* Directory does not exist. EEXIST for race condition */
      if (mkdir(path, mode) != 0 && errno != EEXIST) status = -1;
    }
    else if (!S_ISDIR(st.st_mode)) {
      errno = ENOTDIR;
      status = -1;
    }

    return (status);
  }

  static int mkpath(const char* path, mode_t mode) {
    char* pp;
    char* sp;
    int status;
    char* copypath = strdup(path);

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, '/')) != 0) {
      if (sp != pp) {
        /* Neither root nor double slash in path */
        *sp = '\0';
        status = do_mkdir(copypath, mode);
        *sp = '/';
      }
      pp = sp + 1;
    }
    if (status == 0) status = do_mkdir(path, mode);
    free(copypath);
    return (status);
  }

#endif  //LOG_PLATFORM_WINDOWS

  static std::string get_path_separator() {
#ifdef LOG_PLATFORM_WINDOWS
    return std::string("\\");
#else /*LOG_PLATFORM_WINDOWS*/
    return std::string("/");
#endif /*LOG_PLATFORM_WINDOWS*/
  }

  static std::string get_process_file_path() {
#ifdef LOG_PLATFORM_WINDOWS
    char file_name[MAX_PATH];

    GetModuleFileNameA(NULL, file_name, sizeof(file_name));

    std::string path(file_name);
    size_t last_delimiter = path.find_last_of(get_path_separator());
    return path.substr(0, last_delimiter);
#else  // LOG_PLATFORM_WINDOWS

#ifndef LOG_PLATFORM_MAC
    std::string path = do_readlink(LOG_SELF_PROC_LINK);
#else //LOG_PLATFORM_MAC
    char file_path[1024];
    ::uint32_t size = sizeof(file_path);
    _NSGetExecutablePath(file_path, &size);
    std::string path = file_path;

#endif //LOG_PLATFORM_MAC
    return path.substr(0, path.find_last_of('/'));

#endif  // LOG_PLATFORM_WINDOWS
  }

  static std::string get_process_full_file_name() {
#ifdef LOG_PLATFORM_WINDOWS
    char file_name[MAX_PATH];

    GetModuleFileNameA(NULL, file_name, sizeof(file_name));
    std::string path = file_name;

    size_t last_delim = path.find_last_of(get_path_separator());
    if (last_delim != std::string::npos) last_delim++;

    return path.substr(last_delim, std::string::npos);
#else   // LOG_PLATFORM_WINDOWS

#ifndef LOG_PLATFORM_MAC
    std::string path = do_readlink(LOG_SELF_PROC_LINK);
#else //LOG_PLATFORM_MAC
    char file_path[1024];
    ::uint32_t size = sizeof(file_path);
    _NSGetExecutablePath(file_path, &size);
    std::string path = file_path;
#endif //LOG_PLATFORM_MAC

    size_t last_delim = path.find_last_of(get_path_separator());
    if (last_delim != std::string::npos)
      last_delim++;
    else
      return path;

    return path.substr(last_delim, std::string::npos);
#endif  // LOG_PLATFORM_WINDOWS
  }

  static std::string get_process_file_name() {
    std::string full_name = get_process_full_file_name();

    size_t start = full_name.find_last_of(get_path_separator());
    if (start != std::string::npos)
      start++;
    else
      start = 0;

    size_t count = full_name.find_last_of(".");
    return full_name.substr(start, count);
  }


  static unsigned long get_process_id() {
    unsigned long pid = 0;
#ifdef LOG_PLATFORM_WINDOWS
    pid = GetCurrentProcessId();
#else  // LOG_PLATFORM_WINDOWS
#  ifdef LOG_HAVE_UNISTD_H
    pid = (unsigned long)getpid();
#  endif  // LOG_HAVE_UNISTD_H
#endif  // LOG_PLATFORM_WINDOWS
    return pid;
  }

  static unsigned long get_thread_id() {
    unsigned long tid = 0;
#ifdef LOG_PLATFORM_WINDOWS
    tid = GetCurrentThreadId();
#else  // LOG_PLATFORM_WINDOWS

#ifdef gettid
    tid = (unsigned long)gettid();
#else  // gettid
#if defined(SYS_gettid) && defined(LOG_HAVE_SYS_SYSCALL_H)
    tid = (unsigned long)syscall(SYS_gettid);
#else  // defined(SYS_gettid) && defined(LOG_HAVE_SYS_SYSCALL_H)

#ifdef LOG_MULTITHREADED
    tid = (unsigned long)pthread_self();
#endif  // LOG_MULTITHREADED
#endif  // defined(SYS_gettid) && defined(LOG_HAVE_SYS_SYSCALL_H)

#endif  // gettid

#endif  // LOG_PLATFORM_WINDOWS
    return tid;
  }

}; //class utils

}//namespace detail
}//namespace logging

#endif /*LOGGER_UTILS_HEADER*/
