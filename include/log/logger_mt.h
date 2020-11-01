
#ifndef LOGGER_MULTITHREAD_HEADER
#define LOGGER_MULTITHREAD_HEADER

#include "logger_config.h"
#include "logger_pdetect.h"
#include "logger_pdefs.h"
#include "logger_sysinclib.h"

#ifdef LOG_CPP_X11
#include <thread>
#endif /*LOG_CPP_X11*/

#ifdef LOG_HAVE_PTHREAD
#include <pthread.h>
#endif /*LOG_HAVE_PTHREAD*/

////////// Portable atomic implementation

#define LOG_ATOMIC_IMPL_GCC 1
#define LOG_ATOMIC_IMPL_WINDOWS 2
#define LOG_ATOMIC_IMPL_CXX11 3
#define LOG_ATOMIC_IMPL_NOATOMIC 4

/* User can override value LOG_ATOMIC_IMPL by himself */
#if !defined(LOG_ATOMIC_IMPL)
#if defined (LOG_COMPILER_GCC_COMPAT)
#define LOG_ATOMIC_IMPL LOG_ATOMIC_IMPL_GCC
#elif defined(LOG_PLATFORM_WINDOWS)
#define LOG_ATOMIC_IMPL LOG_ATOMIC_IMPL_WINDOWS
#elif defined(LOG_CPP_X11)
#define LOG_ATOMIC_IMPL LOG_ATOMIC_IMPL_CXX11
#else
#define LOG_ATOMIC_IMPL LOG_ATOMIC_IMPL_NOATOMIC
#endif

#endif /*!defined(LOG_ATOMIC_IMPL)*/

#if LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_CXX11
#include <atomic>
#elif LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_WINDOWS /*end of LOG_ATOMIC_IMPL==LOG_CPP_X11*/
long _InterlockedIncrement(long volatile*);
long _InterlockedDecrement(long volatile*);
long LOG_CDECL _InterlockedExchange(long volatile*, long);
long LOG_CDECL _InterlockedCompareExchange(long volatile*, long, long);

#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedCompareExchange)

#endif  /*end of LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_WINDOWS*/

namespace logging {
namespace detail {
namespace mt {

#if LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_GCC
typedef volatile long atomic_long_type;
#elif LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_WINDOWS
typedef volatile long atomic_long_type;
#elif LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_CXX11
typedef std::atomic<long> atomic_long_type;
#elif LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_NOATOMIC
typedef volatile long atomic_long_type;
#else
#error "Wrong atomic implementation configuration"
#endif

#if LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_GCC
// GCC implementation
static long atomic_increment(long volatile* variable) {
  return __atomic_add_fetch(variable, 1, __ATOMIC_SEQ_CST);
}

static long atomic_decrement(long volatile* variable) {
  return __atomic_sub_fetch(variable, 1, __ATOMIC_SEQ_CST);
}

static long atomic_exchange(long volatile* variable, long new_val) {
  return __atomic_exchange_n(variable, new_val);
}

static bool atomic_compare_exchange(long volatile* variable, long new_val, long expected_val) {
  return __atomic_compare_exchange_n(variable, &expected_val, new_val, true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

#elif LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_WINDOWS
// Windows implementation
static long atomic_increment(long volatile* variable) {
  return _InterlockedIncrement(variable);
}

static long atomic_decrement(long volatile* variable) {
  return _InterlockedDecrement(variable);
}

static long atomic_exchange(long volatile* variable, long new_val) {
  return _InterlockedExchange(variable, new_val);
}

static bool atomic_compare_exchange(long volatile* variable, long new_val, long expected_val) {
  long old_val = _InterlockedCompareExchange(variable, new_val, expected_val);
  return old_val == expected_val;
}
#elif LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_CXX11 /*end of LOG_CPP_X11*/
// C++11 implementation
static long atomic_increment(std::atomic<long>* variable) {
  return (*variable)++;
}

static long atomic_decrement(std::atomic<long>* variable) {
  return (*variable)--;
}

static long atomic_exchange(std::atomic<long>* variable, long new_val) {
  return (*variable).exchange(new_val);
}

static bool atomic_compare_exchange(std::atomic<long>* variable, long new_val, long expected_val) {
  return (*variable).compare_exchange_weak(expected_val, new_val);
}

#elif LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_NOATOMIC /*end of LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_CXX11*/
/* NO ATOMIC SUPPORT, JUST EMULATE OPERATIONS */
#pragma warning("Atomic operations are not supported!")

static long atomic_increment(long volatile* variable) {
  long old_val = *variable;
  *variable++;
  return old_val;
}

static long atomic_decrement(long volatile* variable) {
  long old_val = *variable;
  *variable--;
  return old_val;
}

static long atomic_exchange(long volatile* variable, long new_val) {
  long old_val = *variable;
  return *variable;
}

static bool atomic_compare_exchange(long volatile* variable, long new_val, long expected_val) {
  long old_val = *variable;
  *variable = new_val;
  return old_val == expected_val;
}

#endif /*end of LOG_ATOMIC_IMPL==LOG_ATOMIC_IMPL_NOATOMIC*/
}//namespace mt
}//namespace detail
}//namespace logging

//////////// Multithreading defines

#ifdef LOG_PLATFORM_WINDOWS
#define LOG_MT_EVENT_TYPE HANDLE
#define LOG_MT_THREAD_HANDLE_TYPE   HANDLE

#define LOG_MT_MUTEX CRITICAL_SECTION
#define LOG_MT_MUTEX_INIT(x, y) InitializeCriticalSection(x)
#define LOG_MT_MUTEX_LOCK(x) EnterCriticalSection(x)
#define LOG_MT_MUTEX_UNLOCK(x) LeaveCriticalSection(x)
#define LOG_MT_MUTEX_DESTROY(x) DeleteCriticalSection(x)
#define LOG_MT_THREAD_EXIT(x) ExitThread(x)
#else  // LOG_PLATFORM_WINDOWS

#define LOG_MT_EVENT_TYPE pthread_cond_t
#define LOG_MT_THREAD_HANDLE_TYPE   pthread_t

#define LOG_MT_MUTEX pthread_mutex_t
#define LOG_MT_MUTEX_INIT pthread_mutex_init
#define LOG_MT_MUTEX_LOCK pthread_mutex_lock
#define LOG_MT_MUTEX_UNLOCK pthread_mutex_unlock
#define LOG_MT_MUTEX_DESTROY pthread_mutex_destroy
#define LOG_MT_THREAD_EXIT pthread_exit

#endif  // LOG_PLATFORM_WINDOWS

namespace logging {
namespace detail {
namespace mt {

static void create_event(LOG_MT_EVENT_TYPE* evt) {
#ifdef LOG_PLATFORM_WINDOWS
  *evt = CreateEvent(NULL, FALSE, TRUE, NULL);
#else   // LOG_PLATFORM_WINDOWS
  pthread_cond_init(evt, NULL);
#endif  // LOG_PLATFORM_WINDOWS
}

static void close_event(LOG_MT_EVENT_TYPE* evt) {
#ifdef LOG_PLATFORM_WINDOWS
  CloseHandle(*evt);
  *evt = NULL;
#else 
  pthread_cond_destroy(evt);
#endif
}

static void fire_event(LOG_MT_EVENT_TYPE* evt) {
#ifdef LOG_PLATFORM_WINDOWS
  SetEvent(*evt);
#else   // LOG_PLATFORM_WINDOWS
  pthread_cond_signal(evt);
#endif  // LOG_PLATFORM_WINDOWS
}

static void yield() {
#if defined(LOG_PLATFORM_WINDOWS)
  Sleep(0);
#elif defined(LOG_HAVE_PTHREAD)
  pthread_yield();
#elif defined(LOG_CPP_X11)
  std::this_thread::yield();
#else
  // ??? do nothing
#endif
}

static void thread_join(LOG_MT_THREAD_HANDLE_TYPE* handle) {
#ifdef LOG_PLATFORM_WINDOWS
  WaitForSingleObject(*handle, INFINITE);
#else   // LOG_PLATFORM_WINDOWS
  pthread_join(*handle, NULL);
#endif  // LOG_PLATFORM_WINDOWS
}

static bool wait_event(LOG_MT_EVENT_TYPE* evt, LOG_MT_MUTEX* mutex, bool is_mutex_locked, int wait_ms) {
#ifdef LOG_PLATFORM_WINDOWS
  if (is_mutex_locked) {
    LOG_MT_MUTEX_UNLOCK(mutex);
  }

  bool result = WaitForSingleObject(*evt, wait_ms) == WAIT_OBJECT_0;

  if (is_mutex_locked) {
   LOG_MT_MUTEX_LOCK(mutex);
  }

  return result;
#endif  // LOG_PLATFORM_WINDOWS

#ifndef LOG_PLATFORM_WINDOWS
  struct timeval tv;
  struct timespec ts;

  gettimeofday(&tv, NULL);
  ts.tv_sec = time(NULL) + wait_ms / 1000;
  ts.tv_nsec = tv.tv_usec * 1000 + 1000 * 1000 * (wait_ms % 1000);
  ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
  ts.tv_nsec %= (1000 * 1000 * 1000);

  if (!is_mutex_locked) {
    LOG_MT_MUTEX_LOCK(mutex);
  }

  int ret = pthread_cond_timedwait(evt, &mt_buffer_lock_, &ts);
  if (!is_mutex_locked) {
    LOG_MT_MUTEX_UNLOCK(mutex);
  }

  return ret == 0;
#endif  // LOG_PLATFORM_WINDOWS
}


class read_write_spinlock {
public:
  read_write_spinlock() : readers_(0), writer_requests_(0) {}
  ~read_write_spinlock() {}

  void read_lock() {

    while (true) {
      long readers = readers_;
      long writer_requests = writer_requests_;

      if (readers != kWriteLockActive && !writer_requests) {
        long new_readers = readers + 1;
        if (atomic_compare_exchange(&readers_, new_readers, readers)) {
          // lock acquired
          return;
        }
      }

      yield();
    }
  }

  void read_unlock() {
    while (true) {
      long readers = readers_;
      if (readers != kWriteLockActive && readers > 0) {
        long new_readers = readers - 1;
        if (atomic_compare_exchange(&readers_, new_readers, readers)) {
          // unlock successfull
          return;
        }
      }

      yield();
    }
  }

  void write_lock() {
    atomic_increment(&writer_requests_);

    while (true) {
      long readers = readers_;
      if (readers == 0 && atomic_compare_exchange(&readers_, kWriteLockActive, readers)) {
        // write lock acquired
        break;
      }

      yield();
    }

    atomic_decrement(&writer_requests_);
  }

  void write_unlock() {
    while (true) {
      long readers = readers_;
      if (readers == kWriteLockActive && atomic_compare_exchange(&readers_, 0, readers)) {
        // write lock acquired
        break;
      }

      yield();
    }
  }

private:
  const long kWriteLockActive = 0x7FFFFFFF;
  atomic_long_type readers_;
  atomic_long_type writer_requests_;
};

class reader_spinlock {
public:
  reader_spinlock(read_write_spinlock& lock) : lock_(lock) {
    lock_.read_lock();
  }

  ~reader_spinlock() {
    lock_.read_unlock();
  }
private:
  read_write_spinlock& lock_;
};

class writer_spinlock {
public:
  writer_spinlock(read_write_spinlock& lock) : lock_(lock) {
    lock_.write_lock();
  }

  ~writer_spinlock() {
    lock_.write_unlock();
  }
private:
  read_write_spinlock& lock_;
};



}//namespace mt
}//namespace detail
}//namespace logging

#endif /*LOGGER_MULTITHREAD_HEADER*/
