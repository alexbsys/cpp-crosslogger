
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

/// !!!! Need to define once in one of cpp file (if logger.cpp was not used)
DEFINE_LOGGER(NULL);



void LogFn()
{
	LOG_INFO("HELLO HELLO HELLO HELLO HELLO ");
}

void stacktrace_example_deep1();
void stacktrace_example_deep2();
void stacktrace_example_deep3();
void stacktrace_example_variable_deep(int count);

void stacktrace_example()
{
	stacktrace_example_deep1();
}

void stacktrace_example_deep1()
{
	stacktrace_example_deep2();
}

void stacktrace_example_deep2()
{
	stacktrace_example_deep3();
}

void stacktrace_example_deep3()
{
	stacktrace_example_variable_deep(10);
}

void stacktrace_example_variable_deep(int count)
{
	if (count)
		stacktrace_example_variable_deep(count-1);
	else
	{
		LOG_STACKTRACE_DEBUG();
	}
}

void function_with_exception_example() {
	LOG_DEBUG("$(function) Enter");

	printf("Hello world!\n");
	LOG_STACKTRACE_DEBUG();
	throw std::runtime_error("This is test exception");
}

void example_log_sourcefile_linenumbers() {
	LOG_DEBUG("$(function) Enter");

	try {
		LOG_DEBUG("Calling TestFunction() at $(srcfile), line $(line)");
		function_with_exception_example();
	}
	catch(std::runtime_error &e)
	{
		LOG_EXCEPTION_DEBUG(&e);

		LOG_DEBUG("Rethrowing exception at line $(line)");
		throw;
	}
}


#if LOG_SHARED
void example_shared_log_feature(int rounds)
{
	logging::singleton<logging::logger_interface, logging::detail::logger> _logger1( 
    NULL,
		&logging::logger_interface::ref, &logging::logger_interface::deref, &logging::logger_interface::ref_counter, 
		(logging::logger_interface*)logging::detail::shared_obj::try_find_shared_object(0), false); 

	_logger1->log(LOGGER_VERBOSE_INFO,(void*)"1234","5678","src", rounds,"test by other logger instance. round %d", rounds);

	if (rounds)
		example_shared_log_feature(rounds-1);
}
#endif //LOG_SHARED

extern "C"
int logger_test_c();


struct thread_data
{
//	HANDLE event;
	int thread_num;
};

unsigned long 
#ifdef LOG_PLATFORM_WINDOWS
WINAPI 
#endif //LOG_PLATFORM_WINDOWS
thread_fn(void* ptr)
{
	thread_data* td = (thread_data*)ptr;

	for (int i=0; i<10; i++)
	{
		LOG_MODULES_DEBUG();
	}

	for (int i=0; i<500; i++)
	{
		stacktrace_example();
	}


	free(td);

	return 0;
}

void multithread_test()
{
	int i;

	for (i=0; i<10; i++)
	{
		thread_data* td = (thread_data*) malloc(sizeof(thread_data));
		td->thread_num = i;

		LOG_DEBUG("Create thread %d", i);

#ifdef LOG_PLATFORM_WINDOWS
		DWORD thread_id;
		CreateThread(NULL, 0, &thread_fn, td, 0, &thread_id);
#else //LOG_PLATFORM_WINDOWS
		pthread_t thread_id;
		pthread_create(&thread_id, NULL, (void*(*)(void*))&thread_fn, td);
#endif //LOG_PLATFORM_WINDOWS
	}

	LOG_DEBUG("Start!!!!!");
//	SetEvent(event);
#ifdef LOG_PLATFORM_WINDOWS
	Sleep(10000);
#else
	sleep(10);
#endif
}

int main(int argc, char* argv[])
{
	unsigned long start_ms;
	// enable all logger messgaes
//	LOG_SET_VERBOSE_LEVEL(LOGGER_VERBOSE_ALL);

	// enable to take configuration from registry
//	LOG_SET_REG_CONFIG_PATH("HKCU\\Software\\Logtest");
//	Logging::Configurator.SetLogFileName(logging::Utils::GetApplicationFileName());

#ifdef LOG_PLATFORM_WINDOWS
	start_ms = GetTickCount();
#endif //LOG_PLATFORM_WINDOWS

	LOG_INFO("Hello world! Ten: %d, string: %s!!",10,"ABCDEFG");
	LOG_INFO("WSTRING: %ws", L"test wstring");
	LOG_WARNING("Testing warning at $(srcfile), line $(line)");

	LOG_DEBUG("============ Stack trace test ============");
	stacktrace_example();


	LOG_DEBUG("============ Exceptions test ============");
	try 
	{
		example_log_sourcefile_linenumbers();
	}
	catch(std::runtime_error &e)
	{
		LOG_EXCEPTION_DEBUG(&e);
	}

	LOG_DEBUG("============ Modules list test ============");
	LOG_MODULES_DEBUG();

	LOG_DEBUG("============ Binary log test ============");
	char t[128];
	for (int i=0; i<128; i++) t[i] = i;
	LOG_BINARY_ERROR(t,sizeof(t));

#if LOG_SHARED
	LOG_DEBUG("============ Log sharing test =========== ");
	example_shared_log_feature(20);
#endif //LOG_SHARED

	LOG_DEBUG("============ Log from C test =========== ");
	logger_test_c();

	LOG_DEBUG("============ SCROLL TEST ===========");
	for (int i=0; i<8192*10; i++)
	{
		LOG_DEBUG("TEST TEST TEST TEST TEST TEST TEST");
		LogFn();
	}

	LOG_DEBUG("============= Multithread test ============");
	multithread_test();

//	LOG_DEBUG("============ Crash dump test ============");
//	BYTE* asdf = (BYTE*)0x0;
//	*asdf = 23;

//	getch();

#ifdef LOG_PLATFORM_WINDOWS
	printf("Time elapsed: %d ms\n", GetTickCount() - start_ms);
#endif //LOG_PLATFORM_WINDOWS

	return 0;
}
