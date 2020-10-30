// Extremely simplest example
// No dependencies at all, no additional CPP files, only logger.h

#include <log/logger_win_config_macro_plugin.h>
#include <log/logger_ini_config_plugin.h>
#include <log/logger_scroll_file_output_plugin.h>
#include <log/logger_binary_command_plugin.h>
#include <log/logger_modules_command_plugin.h>
#include <log/logger_stacktrace_command_plugin.h>
#include <log/logger_crashhandler_command_plugin.h>
#include <log/logger_objmon_command_plugin.h>

#include <log/logger.h>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>

#ifdef LOG_PLATFORM_WINDOWS
#include <windows.h>
#else //LOG_PLATFORM_WINDOWS
#include <pthread.h>
#include <unistd.h>
#endif //LOG_PLATFORM_WINDOWS

// Shared log example will be used
#define LOG_SHARED 1


/// !!!! Need to define once in one of cpp file (if logger.cpp was not used)
//DEFINE_LOGGER();
logging::logger_singleton_interface<logging::logger_interface>* logging::_logger(
  new logging::singleton<logging::logger_interface, logging::detail::logger>(\
    &logging::logger_interface::ref, &logging::logger_interface::deref, \
    &logging::logger_interface::ref_counter, \
    (logging::logger_interface*)logging::detail::shared_obj::try_find_shared_object(0), \
    false));
//void* logging::unhandled_exceptions_processor::prev_exception_filter_ = NULL;



void stacktrace_example_deep1();
void stacktrace_example_deep2();
void stacktrace_example_deep3();
void stacktrace_example_variable_deep(int count);

void stacktrace_example() {
	stacktrace_example_deep1();
}

void stacktrace_example_deep1() {
	stacktrace_example_deep2();
}

void stacktrace_example_deep2() {
	stacktrace_example_deep3();
}

void stacktrace_example_deep3() {
	stacktrace_example_variable_deep(10);
}

void stacktrace_example_variable_deep(int deep) {
  if (deep) {
    stacktrace_example_variable_deep(deep - 1);
  }
	else {
		LOG_STACKTRACE_DEBUG();
	}
}


void function_with_exception_example()
{
	LOG_DEBUG("$(function) Enter");

	printf("Hello from test_function!\n");
	LOG_STACKTRACE_DEBUG();
	throw std::runtime_error("This is test exception");
}

void example_log_sourcefile_linenumbers() {
  LOG_DEBUG("$(function) Enter");

  try {
    LOG_DEBUG("Calling TestFunction() at $(srcfile), line $(line)");
		function_with_exception_example();
	}
	catch(std::runtime_error &e) {
		LOG_EXCEPTION_DEBUG(&e);

		LOG_DEBUG("Rethrowing exception at $(srcfile):$(line)");
		throw;
	}
}


#if LOG_SHARED
void example_shared_log_feature(int rounds)
{
	logging::singleton<logging::logger_interface, logging::detail::logger> _logger1( 
		&logging::logger_interface::ref, &logging::logger_interface::deref, &logging::logger_interface::ref_counter, 
		(logging::logger_interface*)logging::detail::shared_obj::try_find_shared_object(0), false); 

	_logger1->log(LOGGER_VERBOSE_INFO,(void*)"1234","5678","src", rounds,"test by other logger instance. round %d", rounds);

	if (rounds)
		example_shared_log_feature(rounds-1);
}
#endif //LOG_SHARED



struct thread_data {
	int thread_num;
};

unsigned long 
#ifdef _WIN32
WINAPI 
#endif //_WIN32
thread_fn(void* ptr) {
	thread_data* td = (thread_data*)ptr;

  LOG_INFO("Thread %d execution", td->thread_num);

	for (int i=0; i<10; i++) {
		LOG_MODULES_DEBUG();
	}

	for (int i=0; i<500; i++) {
		stacktrace_example();
	}

	free(td);
	return 0;
}

void multithread_test() {
	int i;
#ifdef _WIN32
  HANDLE threads[10];
#else //_WIN32
  pthread_t threads[10];
#endif //_WIN32

	for (i=0; i<10; i++) {
		thread_data* td = (thread_data*) malloc(sizeof(thread_data));
		td->thread_num = i;

		LOG_DEBUG("Create thread %d", i);

#ifdef LOG_PLATFORM_WINDOWS
		DWORD thread_id;
		threads[i] = CreateThread(NULL, 0, &thread_fn, td, 0, &thread_id);
#else //LOG_PLATFORM_WINDOWS
		pthread_t thread_id;
		pthread_create(&thread_id, NULL, (void*(*)(void*))&thread_fn, td);
    threads[i] = thread_id;
#endif //LOG_PLATFORM_WINDOWS
	}

	LOG_DEBUG("All threads started");
#ifdef LOG_PLATFORM_WINDOWS
	Sleep(10000);
#else
	sleep(10);
#endif

  for (int i = 0; i < 10; i++) {
#ifdef _WIN32
    WaitForSingleObject(threads[i], INFINITE);
#else //_WIN32
    pthread_join(threads[i], NULL);
#endif //_WIN32
  }
}

class Test {
public:
  Test() {
    LOG_OBJMON_REGISTER_INSTANCE();
  }

  ~Test() {
    LOG_OBJMON_UNREGISTER_INSTANCE();
  }
};

class LogToConsole : public virtual logging::logger_output_plugin_interface {
public:
  virtual ~LogToConsole() {}
  const char* type() const { return "console_output"; }
  virtual void write(int verb_level, const std::string& hdr, const std::string& what) {
    printf("%s %s\n", hdr.c_str(), what.c_str());
  }


};

int main(int argc, char* argv[]) {

	// enable all logger messgaes
//	LOG_SET_VERBOSE_LEVEL(LOGGER_VERBOSE_ALL);

  logging::_logger->get()->register_plugin_factory(new logging::logger_scroll_file_output_plugin_factory() );
  logging::_logger->get()->register_plugin_factory(new logging::logger_binary_command_plugin_factory());
  logging::_logger->get()->register_plugin_factory(new logging::logger_stacktrace_command_plugin_factory());
  logging::_logger->get()->register_plugin_factory(new logging::logger_modules_command_plugin_factory());
  logging::_logger->get()->register_plugin_factory(new logging::logger_win_config_macro_plugin_factory());
  logging::_logger->get()->register_plugin_factory(new logging::logger_ini_config_plugin_factory());
  logging::_logger->get()->register_plugin_factory(new logging::logger_crashhandler_command_plugin_factory());
  logging::_logger->get()->register_plugin_factory(new logging::logger_objmon_command_plugin_factory());

//	logging::configurator.set_log_file_name(logging::detail::utils::get_process_file_name());
//  logging::configurator.set_log_scroll_file_count(6);
//  logging::configurator.set_log_scroll_file_size(2 * 1024 * 1024);
//  logging::configurator.set_log_scroll_file_every_run(true);

//  logging::_logger->attach_plugin(new logging::logger_binary_command_plugin());
//  logging::_logger->attach_plugin(new logging::logger_modules_command_plugin());
//  logging::_logger->attach_plugin(new logging::logger_stacktrace_command_plugin());

//  logging::_logger->attach_plugin(new logging::logger_win_config_macro_plugin());
//  logging::_logger->attach_plugin(new logging::logger_ini_config_plugin());
  logging::_logger->get()->attach_plugin(new LogToConsole());
//  logging::_logger->attach_plugin(new logging::detail::logger_file_output());

  logging::_logger->get()->set_config_param("logger::LoadPlugins", "win_config_macro ini_config modules_cmd stacktrace_cmd binary_cmd crashhandler_cmd objmon_cmd");
  logging::_logger->get()->set_config_param("IniFilePaths", LOG_DEFAULT_INI_PATHS);


  logging::_logger->get()->reload_config();

  logging::_logger->get()->dump_state(logging::logger_verbose_info);
  logging::_logger->get()->flush();

  LOG_CMD(0x100A, logging::logger_verbose_debug, NULL, 0);

  const char* msg = "HELLO";

  LOG_CMD(0x1001, logging::logger_verbose_info, msg, 6);
  LOG_CMD(0x1002, logging::logger_verbose_info, NULL, 0);
//  logging::_logger->log_cmd(0x1001, logging::logger_verbose_info, 0, "", "", 0, msg, 6);

  {
    Test test;
    LOG_OBJMON_DUMP_INFO();
  }

  LOG_OBJMON_DUMP_INFO();

  LOG_DEBUG("%.8X", msg);

//  (*logging::_logger.get()) << "Hello world" << " test " << 12345;
  LOGS_DEBUG() << "Hello world" << " test " << 12345;
  LOGS_DEBUG() << "Next string " << std::string("this-is-stdstring");

	LOG_INFO("Formatted string example. Ten: %d, string: %s", 10, "ABCDEFG");
	LOG_INFO("WSTRING: %ws", L"test wstring");
	LOG_WARNING("Example warning at $(srcfile), line $(line)");
  LOG_ERROR("Example error");

  LOG_MODULES_INFO();

//  (*logging::_logger.get()) << std::string("Hello world") << std::string("\n");// std::endl();

	LOG_DEBUG("============ Stack trace test ============");
	stacktrace_example();

	LOG_DEBUG("============ Exceptions test ============");
	try {
		example_log_sourcefile_linenumbers();
	} catch(std::runtime_error &e) {
		LOG_EXCEPTION_DEBUG(&e);
	}

	LOG_DEBUG("============ Modules list test ============");
	LOG_MODULES_DEBUG();

	LOG_INFO("============ Binary log test ============");
	char t[128];
	for (int i=0; i<128; i++) t[i] = i;
	  LOG_BINARY_ERROR(t,sizeof(t));

#if LOG_SHARED
	LOG_INFO("============ Log sharing test =========== ");
	example_shared_log_feature(20);
#endif //LOG_SHARED

	LOG_INFO("============ SCROLL FILES TEST ===========");
	for (int i=0; i<8192; i++)	{
		LOG_DEBUG("TEST TEST TEST TEST TEST TEST TEST");
	}

//	LOG_INFO("============= Multithread test ============");
//	multithread_test();

	LOG_DEBUG("============ Crash dump test ============");
	BYTE* asdf = (BYTE*)0x0;
	*asdf = 23;

	return 0;
}
