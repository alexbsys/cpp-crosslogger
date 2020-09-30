
#include "log/logger.h"


int logger_test_c()
{
	LOG_DEBUG("Hello world from C! %d %d %d %d", 1, 2, 3, 4);
	LOG_MODULES_INFO;

	LOG_STACKTRACE_WARNING;

	return 0;
}