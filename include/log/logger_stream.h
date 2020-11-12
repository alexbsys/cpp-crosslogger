
// Logger public definition

#ifndef LOGGER_STREAM_HEADER
#define LOGGER_STREAM_HEADER

#include "logger_config.h"
#include "logger_interfaces.h"
#include <sstream>

namespace logging {
namespace detail {

class log_stream {
public:
  log_stream(logger_interface* logger_obj, int verb_level, void* addr,
    const char* function_name, const char* source_file, int line_number)
    : logger_obj_(logger_obj)
    , verb_level_(verb_level)
    , addr_(addr)
    , function_name_(function_name)
    , source_file_(source_file)
    , line_number_(line_number) {
    logger_obj_->ref();
  }

  log_stream(const log_stream& stream)
    : logger_obj_(stream.logger_obj_)
    , verb_level_(stream.verb_level_)
    , addr_(stream.addr_)
    , function_name_(stream.function_name_)
    , source_file_(stream.source_file_), line_number_(stream.line_number_) {
    ss_.str(stream.ss_.str());
    logger_obj_->ref();
  }

  virtual ~log_stream() {
    logger_obj_->log(verb_level_, addr_, function_name_, source_file_, line_number_, "%s", ss_.str().c_str());
    logger_obj_->deref();
  }

  template<typename T>
  log_stream& operator <<(const T& t) {
    ss_ << t;
    return *this;
  }

private:
  logger_interface* logger_obj_;
  std::stringstream ss_;
  int verb_level_;
  void* addr_;
  const char* function_name_;
  const char* source_file_;
  int line_number_;
};

}//namespace detail
}//namespace logging

#endif /*LOGGER_STREAM_HEADER*/
