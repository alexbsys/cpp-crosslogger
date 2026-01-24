
#ifndef LOGGER_FILE_UPLOAD_HEADER
#define LOGGER_FILE_UPLOAD_HEADER

#include <log/logger_config.h>
#include <log/logger_pdetect.h>
#include <log/logger_pdefs.h>
#include <log/logger_sysinclib.h>
#include <log/logger_interfaces.h>
#include <log/logger_userdefs.h>
#include <log/logger_verbose.h>
#include <log/detail/logger_strutils.h>
#include <log/detail/logger_runtime_debugging.h>


namespace logging {


  class logger_file_upload_command_plugin : public logger_command_plugin_interface {
  public:
    const int kFileUploadSendFiles = 0x1050;

    // for internal use, user should not call it directly
    const int kCrashExecutePrivateCommandId = 0x100C;

    logger_file_upload_command_plugin(const char* name = NULL) : plugin_name_(name ? name : ""), logger_(NULL) {}
    virtual ~logger_file_upload_command_plugin() LOG_METHOD_OVERRIDE {}

    const char* type() const LOG_METHOD_OVERRIDE { return "fileupload_cmd"; }

    const char* name() const LOG_METHOD_OVERRIDE { return plugin_name_.c_str(); }

    void get_cmd_ids(int* out_cmd_ids, int max_cmds) const LOG_METHOD_OVERRIDE {
      if (max_cmds < 1)
        return;
      out_cmd_ids[0] = kFileUploadSendFiles;
    }

    bool cmd(std::string& out_result, int cmd_id, int verb_level, void* addr, const void* vparam, int iparam) LOG_METHOD_OVERRIDE {
      (void)iparam;
      (void)vparam;
      (void)addr;
      (void)verb_level;

      std::stringstream sstream;
      bool result = false;

      if (cmd_id == kFileUploadSendFiles) {
        result = true;
//        init_unhandled_exceptions_handler(logger_);
      }

      if (result)
        out_result = sstream.str();

      return result;
    }

    virtual bool attach(logger_interface* logger) LOG_METHOD_OVERRIDE { logger_ = logger; return true; }
    virtual void detach(logger_interface* logger) LOG_METHOD_OVERRIDE { (void)logger; logger_ = NULL; }

  private:
    std::string plugin_name_;
    logger_interface* logger_;
  };

  class logger_file_upload_command_plugin_factory : public logger_plugin_default_factory<logger_file_upload_command_plugin> {
  public:
    logger_file_upload_command_plugin_factory()
      : logger_plugin_default_factory<logger_file_upload_command_plugin>("fileupload_cmd", kLogPluginTypeCommand) {}
    virtual ~logger_file_upload_command_plugin_factory() LOG_METHOD_OVERRIDE {}
  };

}//namespace logging

#endif /*LOGGER_FILE_UPLOAD_HEADER*/
