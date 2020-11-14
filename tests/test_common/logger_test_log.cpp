
#include "logger_tests_log.h"

#	define LOG_ENABLED 1
#	define LOG_ONLY_DEBUG 0
#	define LOG_USE_SYSTEMINFO 1
#	define LOG_USE_MODULEDEFINITION 0
#	define LOG_AUTO_DEBUGGING 0
#	define LOG_UNHANDLED_EXCEPTIONS 0
#	define LOG_CONFIGURE_FROM_REGISTRY 0
#	define LOG_INI_CONFIGURATION 0
#	define LOG_CREATE_DIRECTORY 0
#	define LOG_RTTI_ENABLED 0
#	define LOG_SHARED 0
#	define LOG_COMPILER_WARNINGS 1
#	define LOG_USE_DLL 0
#	define LOG_MULTITHREADED 0
#	define LOG_FLUSH_FILE_EVERY_WRITE 0
#	define LOG_CHECKED 1
#	define LOG_USE_MACRO_HEADER_CACHE 1
#	define LOG_PROCESS_MACRO_IN_LOG_TEXT 1
#	define LOG_TEST_DO_NOT_WRITE_FILE 0
#	define LOG_RELEASE_ON_APP_CRASH 1

#include "log/logger.h"
#include "log/logger_register_builtin_plugin.h"

DEFINE_LOGGER(NULL);

bool get_line_skip_empty(std::ifstream& infile, std::string& line)
{
	line = "";

	while (!infile.eof() && !line.size())
		std::getline(infile,line);

	if (!line.size())
		return false;

	return true;
}

//[$(V)] $(dd).$(MM).$(yyyy) $(hh):$(mm):$(ss).$(ttt) [$(PID):$(TID)] [$(module)!$(function)]
TEST_F(logger_tests_log, check_verbose_nofilter)
{
  const std::string kTestLogFileName = "check_verbose_nofilter.log";

	logging::_logger->release();
  std::remove(kTestLogFileName.c_str() /* logging::configurator.get_full_log_file_path().c_str()*/);

  LOG_REGISTER_PLUGIN_FACTORY(new logging::logger_register_builtin_plugin_factory());
  LOG_SET_CONFIG_PARAM("logger::LoadPlugins", "builtin win_config_macro binary_cmd console_output file_output");

  LOG_SET_CONFIG_PARAM("logger::Verbose", "255");
  LOG_SET_CONFIG_PARAM("logger::LogSysInfo", "0");
  LOG_SET_CONFIG_PARAM("file_output::Verbose", "255");
  LOG_SET_CONFIG_PARAM("file_output::LogFileName", kTestLogFileName.c_str());
  LOG_SET_CONFIG_PARAM("file_output::FlushFileEveryWrite", "1");
  LOG_SET_CONFIG_PARAM("logger::HdrFormat", "[$(V)]");
  LOG_SET_CONFIG_PARAM("file_output::ScrollFileSize", "0");
  LOG_SET_CONFIG_PARAM("file_output::ScrollFileCount", "0");
  LOG_SET_CONFIG_PARAM("file_output::LogPath", "$(CURRENTDIR)");
  LOG_RELOAD_CONFIG();



	LOG_DEBUG("TEST-DEBUG");
	LOG_INFO("TEST-INFO");
	LOG_WARNING("TEST-WARNING");
	LOG_ERROR("TEST-ERROR");
	LOG_FATAL("TEST-FATAL");

  logging::_logger->release();

	std::ifstream infile(kTestLogFileName /* logging::configurator.get_full_log_file_path()*/ );
	if (!infile.is_open())
		FAIL();

	std::string line;
	ASSERT_TRUE(get_line_skip_empty(infile,line));
	ASSERT_TRUE(line == "[DEBUG] TEST-DEBUG");
	ASSERT_TRUE(get_line_skip_empty(infile,line));
	ASSERT_TRUE(line == "[INFO] TEST-INFO");
	ASSERT_TRUE(get_line_skip_empty(infile,line));
	ASSERT_TRUE(line == "[WARNING] TEST-WARNING");
	ASSERT_TRUE(get_line_skip_empty(infile,line));
	ASSERT_TRUE(line == "[ERROR] TEST-ERROR");
	ASSERT_TRUE(get_line_skip_empty(infile,line));
	ASSERT_TRUE(line == "[FATAL] TEST-FATAL");
}

TEST_F(logger_tests_log, check_verbose_filter_fatal_warning) {
  const std::string kTestLogFileName = "check_verbose_filter_fatal_warning.log";
  
  logging::_logger->release();
  std::remove(kTestLogFileName.c_str());

  int verb_level = LOGGER_VERBOSE_FATAL | LOGGER_VERBOSE_WARNING;
  char verb_level_str[24];
  itoa(verb_level, verb_level_str, 10);

  LOG_REGISTER_PLUGIN_FACTORY(new logging::logger_register_builtin_plugin_factory());
  LOG_SET_CONFIG_PARAM("logger::LoadPlugins", "builtin win_config_macro binary_cmd console_output file_output");

  LOG_SET_CONFIG_PARAM("logger::Verbose", verb_level_str);
  LOG_SET_CONFIG_PARAM("logger::LogSysInfo", "0");
  LOG_SET_CONFIG_PARAM("file_output::Verbose", verb_level_str);
  LOG_SET_CONFIG_PARAM("file_output::LogFileName", kTestLogFileName.c_str());
  LOG_SET_CONFIG_PARAM("logger::HdrFormat", "[$(V)]");
  LOG_SET_CONFIG_PARAM("file_output::ScrollFileSize", "0");
  LOG_SET_CONFIG_PARAM("file_output::ScrollFileCount", "0");
  LOG_SET_CONFIG_PARAM("file_output::LogPath", "$(CURRENTDIR)");


	LOG_DEBUG("TEST-DEBUG");
	LOG_INFO("TEST-INFO");
	LOG_WARNING("TEST-WARNING");
	LOG_ERROR("TEST-ERROR");
	LOG_FATAL("TEST-FATAL");

	logging::_logger->release();

	std::ifstream infile(kTestLogFileName);
	if (!infile.is_open())
		FAIL();

	std::string line;
	ASSERT_TRUE(get_line_skip_empty(infile,line));
	ASSERT_TRUE(line == "[WARNING] TEST-WARNING");
	ASSERT_TRUE(get_line_skip_empty(infile,line));
	ASSERT_TRUE(line == "[FATAL] TEST-FATAL");
}
/*
TEST_F(logger_tests_log, noheader_filter_nodebug)
{
	logging::_logger.release();

	logging::configurator.set_log_file_name("test.log");
	logging::configurator.set_hdr_format("");
	logging::configurator.set_log_scroll_file_size(0);
	logging::configurator.set_log_path("$(EXEDIR)");
	logging::configurator.set_log_scroll_file_count(0);
	logging::configurator.set_verbose_level(logging::logger_verbose_fatal_error | logging::logger_verbose_warning | logging::logger_verbose_info);
	logging::configurator.set_need_sys_info(false);

	std::remove(logging::configurator.get_full_log_file_path().c_str());

	LOG_DEBUG("TEST-DEBUG");
	LOG_INFO("TEST-INFO");
	LOG_WARNING("TEST-WARNING");
	LOG_ERROR("TEST-ERROR");
	LOG_FATAL("TEST-FATAL");

	logging::_logger.release();

	std::ifstream infile(logging::configurator.get_full_log_file_path());
	if (!infile.is_open())
		FAIL();

	std::string line;
	ASSERT_TRUE(get_line_skip_empty(infile,line));
	ASSERT_TRUE(line == "TEST-INFO");
	ASSERT_TRUE(get_line_skip_empty(infile,line));
	ASSERT_TRUE(line == "TEST-WARNING");
	ASSERT_TRUE(get_line_skip_empty(infile,line));
	ASSERT_TRUE(line == "TEST-ERROR");
	ASSERT_TRUE(get_line_skip_empty(infile,line));
	ASSERT_TRUE(line == "TEST-FATAL");
}



TEST_F(logger_tests_log, noheader_scroll)
{
	const int max_file_size = 50;
	const int max_files = 2;
  const std::string kTestFileName = "noheader_scroll.log";

	logging::_logger->release();

	logging::configurator.set_log_file_name("test.log");
	logging::configurator.set_hdr_format("");
	logging::configurator.set_log_scroll_file_size(max_file_size);
	logging::configurator.set_log_path("$(EXEDIR)");
	logging::configurator.set_log_scroll_file_count(max_files);
	logging::configurator.set_verbose_level(logging::logger_verbose_all);
	logging::configurator.set_need_sys_info(false);

	std::remove(logging::configurator.get_full_log_file_path().c_str());
	std::remove((logging::configurator.get_full_log_file_path() + ".1").c_str());
	std::remove((logging::configurator.get_full_log_file_path() + ".2").c_str());
	std::remove((logging::configurator.get_full_log_file_path() + ".3").c_str());

	LOG_DEBUG  ("TEST-DEBUG   1 12345"); // 20 bytes
	LOG_INFO   ("TEST-INFO    2 12345");
	LOG_WARNING("TEST-WARNING 3 12345");
	LOG_ERROR  ("TEST-ERROR   4 12345");
	LOG_FATAL  ("TEST-FATAL   5 12345");
	LOG_FATAL  ("TEST-FATAL   6 12345");
	LOG_FATAL  ("TEST-FATAL   7 12345");
	LOG_FATAL  ("TEST-FATAL   8 12345");
	LOG_FATAL  ("TEST-FATAL   9 12345");
	LOG_FATAL  ("TEST-FATAL  10 12345");
	LOG_FATAL  ("TEST-FATAL  11 12345");
	LOG_FATAL  ("TEST-FATAL  12 12345");
	LOG_FATAL  ("TEST-FATAL  13 12345");
	LOG_FATAL  ("TEST-FATAL  14 12345");
	LOG_FATAL  ("TEST-FATAL  15 12345");
	LOG_FATAL  ("TEST-FATAL  16 12345");
	LOG_FATAL  ("TEST-FATAL  17 12345");

	logging::_logger.release();

	{
		std::ifstream infile(logging::configurator.get_full_log_file_path(), std::ios::ate);
		if (!infile.is_open())
			FAIL();

		ASSERT_LE(infile.tellg(),max_file_size*2);
	}

	{
		std::ifstream infile(logging::configurator.get_full_log_file_path() + ".1", std::ios::ate);
		if (!infile.is_open())
			FAIL();

		ASSERT_LE(infile.tellg(),max_file_size*2);
	}

	{
		std::ifstream infile(logging::configurator.get_full_log_file_path() + ".2", std::ios::ate);
		if (!infile.is_open())
			FAIL();

		ASSERT_LE(infile.tellg(),max_file_size*2);
	}

	{
		std::ifstream infile(logging::configurator.get_full_log_file_path() + ".3", std::ios::ate);
		if (infile.is_open())
			FAIL();
	}
}
*/
TEST_F(logger_tests_log, check_strong_header)
{
  const std::string kTestLogFileName = "check_strong_header.log";

	logging::_logger->release();
  std::remove(kTestLogFileName.c_str());

  LOG_REGISTER_PLUGIN_FACTORY(new logging::logger_register_builtin_plugin_factory());
  LOG_SET_CONFIG_PARAM("logger::LoadPlugins", "builtin win_config_macro binary_cmd console_output file_output");

  LOG_SET_CONFIG_PARAM("logger::Verbose", "255");
  LOG_SET_CONFIG_PARAM("logger::LogSysInfo", "0");
  LOG_SET_CONFIG_PARAM("file_output::Verbose", "255");
  LOG_SET_CONFIG_PARAM("file_output::LogFileName", kTestLogFileName.c_str());
  LOG_SET_CONFIG_PARAM("logger::HdrFormat", "$(V) $(dd) $(MM) $(yyyy) $(hh) $(mm) $(ss) $(ttt) $(PID) $(TID) $(function)");
  LOG_SET_CONFIG_PARAM("file_output::ScrollFileSize", "0");
  LOG_SET_CONFIG_PARAM("file_output::ScrollFileCount", "0");
  LOG_SET_CONFIG_PARAM("file_output::LogPath", "$(CURRENTDIR)");

	
	int millisec;
	struct tm timeinfo = logging::detail::utils::get_time(millisec);

	LOG_INFO("TEST-INFO");

	logging::_logger->release();

	std::ifstream infile(kTestLogFileName /* logging::configurator.get_full_log_file_path()*/ );
	if (!infile.is_open())
		FAIL();

	std::string line;
	ASSERT_TRUE(get_line_skip_empty(infile,line));

	char verbose[256];
	char function[256];
	function[0] = 0;

	int dd, MM, yyyy, hh, mm, ss, ttt, pid, tid;
	
	// $(V)/$(dd)/$(MM)/$(yyyy)/$(hh)/$(mm)/$(ss)/$(ttt)/$(PID)/$(TID)/$(function)
	sscanf(line.c_str(), "%s %d %d %d %d %d %d %d %d %d %s", verbose, &dd, &MM, &yyyy, &hh, &mm, &ss, &ttt, &pid, &tid, function);

	ASSERT_TRUE(!strcmp(verbose,"INFO"));
	ASSERT_EQ(timeinfo.tm_mday, dd);
	ASSERT_EQ(timeinfo.tm_mon+1, MM);
	ASSERT_EQ(timeinfo.tm_year+1900, yyyy);
	ASSERT_EQ(timeinfo.tm_hour, hh);
	ASSERT_EQ(timeinfo.tm_min, mm);

	ASSERT_NE(0,pid);
	ASSERT_TRUE(strlen(function) > 0);
}
