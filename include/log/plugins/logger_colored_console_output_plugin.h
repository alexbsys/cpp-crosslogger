#ifndef LOGGER_COLORED_CONSOLE_OUTPUT_PLUGIN_HEADER
#define LOGGER_COLORED_CONSOLE_OUTPUT_PLUGIN_HEADER

#include <log/logger_config.h>
#include <log/logger_pdetect.h>
#include <log/logger_pdefs.h>
#include <log/logger_interfaces.h>
#include <log/logger_sysinclib.h>
#include <log/logger_verbose.h>

#include <log/detail/logger_utils.h>
#include <log/detail/logger_cfgfn.h>

#define LOGGER_HAVE_COLORED_CONSOLE_OUTPUT_PLUGIN  1

namespace logging {

class logger_colored_console_output_plugin : public logger_output_plugin_interface {
public:
  virtual ~logger_colored_console_output_plugin() LOG_METHOD_OVERRIDE {}

  logger_colored_console_output_plugin(const char* output_name = NULL)
    : name_(output_name == NULL ? std::string() : output_name) {
  }

  const char* type() const LOG_METHOD_OVERRIDE {
    return "console_output";
  }

  const char* name() const LOG_METHOD_OVERRIDE {
    return name_.c_str();
  }

  void config_updated(const logging::cfg::KeyValueTypeList& config) LOG_METHOD_OVERRIDE {
    (void)config;
    using namespace detail;
  }
  
  /**
  * \brief    write method. Called every write to file operation
  */
  virtual void write(int verb_level, const std::string& hdr, const std::string& what) LOG_METHOD_OVERRIDE {
    int default_color = get_console_text_color();
    int color = default_color;

    switch (verb_level) {
    case LOGGER_VERBOSE_INFO:  color = 15; break;
    case LOGGER_VERBOSE_DEBUG: color = 2; break;
    case LOGGER_VERBOSE_WARNING: color = 14; break;
    case LOGGER_VERBOSE_ERROR: color = 4; break;
    case LOGGER_VERBOSE_FATAL: color = 12; break;
    }

    set_console_text_color(color);

    std::string str = hdr;
    if (hdr.size()) str.append(" ");
    str += what + std::string("\n");

    printf("%s", str.c_str());
    set_console_text_color(default_color);
  }

  void flush() LOG_METHOD_OVERRIDE {
  }

  void close() LOG_METHOD_OVERRIDE {
  }

private:
  int get_console_text_color() {
#ifdef LOG_PLATFORM_WINDOWS
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;

    /* Save current attributes */
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;

    return saved_attributes;
#else /*LOG_PLATFORM_WINDOWS*/
    return 0;
#endif /*LOG_PLATFORM_WINDOWS*/
  }

  void set_console_text_color(int color) {
    (void)color;
#ifdef LOG_PLATFORM_WINDOWS
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
#endif /*LOG_PLATFORM_WINDOWS*/
  }

protected:

  struct console_output_config {
    bool flush_every_write_;

    console_output_config() :
      flush_every_write_(false) {}
  };

  console_output_config config_;

private:
  std::string name_;
};

class logger_colored_console_output_plugin_factory : public logger_plugin_default_factory<logger_colored_console_output_plugin> {
public:
  logger_colored_console_output_plugin_factory()
    : logger_plugin_default_factory<logger_colored_console_output_plugin>("console_output", kLogPluginTypeOutput) {}
  virtual ~logger_colored_console_output_plugin_factory() LOG_METHOD_OVERRIDE {}
};

}//namespace logging

#endif /*LOGGER_COLORED_CONSOLE_OUTPUT_PLUGIN_HEADER*/
