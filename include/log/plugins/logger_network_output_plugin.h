#ifndef LOGGER_NETWORK_OUTPUT_PLUGIN_HEADER
#define LOGGER_NETWORK_OUTPUT_PLUGIN_HEADER

#include <log/logger_config.h>
#include <log/logger_interfaces.h>
#include <log/logger_pdefs.h>
#include <log/logger_pdetect.h>
#include <log/logger_sysinclib.h>
#include <log/logger_verbose.h>

#include <log/detail/logger_cfgfn.h>
#include <log/detail/logger_utils.h>

#include <log/detail/logger_mt.h>
#include <log/detail/logger_utils.h>
#include <log/detail/logger_sysinfo.h>

#include <log/plugins/network_output_plugin/ds/c_string.h>
#include <log/plugins/network_output_plugin/ds/c_vector.h>
#include <log/plugins/network_output_plugin/net/tcp_client.h>
#include <log/plugins/network_output_plugin/net/tls_client.h>
#include <log/plugins/network_output_plugin/net/uri.h>
#include <log/plugins/network_output_plugin/net/web_client.h>

#include <log/plugins/network_output_plugin/alg/z_compress.h>

#include <log/detail/logger_machine_id.h>
#include <log/detail/logger_utils.h>

#define LOGGER_HAVE_NETWORK_OUTPUT_PLUGIN 1

namespace logging {

class logger_network_output_plugin : public logger_output_plugin_interface {
 public:
  virtual ~logger_network_output_plugin() LOG_METHOD_OVERRIDE {
    release_plugin_instance();

    str_free(&host_name_);
    detail::mt::close_event(&write_event_);
    detail::mt::close_event(&buffer_devastated_);

    LOG_MT_MUTEX_DESTROY(&mt_buffer_lock_);

    net_release();
  }

  logger_network_output_plugin(const char* output_name = NULL)
      : net_cli_(nullptr),
        name_(output_name == NULL ? std::string() : output_name) {
    plugin_initialized_ = false;
    is_stop_ = false;
    bytes_in_buffer_ = 0;
    recreate_network_connection_ = 0;

    config_initialize();
    net_init();

    LOG_MT_MUTEX_INIT(&mt_buffer_lock_, NULL);

    str_init(&host_name_);

    detail::mt::create_event(&write_event_);
    detail::mt::create_event(&buffer_devastated_);
  }

  const char* type() const LOG_METHOD_OVERRIDE { return "network_output"; }

  const char* name() const LOG_METHOD_OVERRIDE { return name_.c_str(); }

  void config_updated(const logging::cfg::KeyValueTypeList& config)
      LOG_METHOD_OVERRIDE {
    using namespace detail;

    std::string web_addr =
        detail::cfg::get_logcfg_string(config, type(), name(), "WebAddr", config_.web_address_);

    bool reconnect = false;

    if (web_addr != config_.web_address_) {
      config_.web_address_ = web_addr;
      reconnect = true;
    }

    std::string product_code =
        detail::cfg::get_logcfg_string(config, type(), name(), "ProductCode", config_.product_code_);

    if (product_code != config_.product_code_) {
      config_.product_code_ = product_code;
      reconnect = true;
    }

    std::string machine_id =
        detail::cfg::get_logcfg_string(config, type(), name(), "MachineId", config_.machine_id_);

    if (machine_id != config_.machine_id_) {
      config_.machine_id_ = machine_id;
      reconnect = true;
    }

    int encryption_policy =
        detail::cfg::get_logcfg_int(config, type(), name(), "EncryptionPolicy",
                                    kConnectionEncryptionPolicy_Autodetect);

    if (encryption_policy != config_.connection_encryption_policy_) {
      config_.connection_encryption_policy_ = encryption_policy;
      reconnect = true;
    }

    config_.log_send_webaddr_ = detail::cfg::get_logcfg_string(
          config, type(), name(), "LogSendWebAddress", config_.log_send_webaddr_);

    config_.log_check_webaddr_ = detail::cfg::get_logcfg_string(
          config, type(), name(), "LogCheckWebAddress", config_.log_check_webaddr_);

    config_.flush_wait_ms_ = detail::cfg::get_logcfg_int(
        config, type(), name(), "FlushWaitMs", config_.flush_wait_ms_);
    config_.max_buffered_bytes_ =
        detail::cfg::get_logcfg_int(config, type(), name(), "MaxBufferedBytes",
                                    config_.max_buffered_bytes_);
    config_.use_compression_ =
        detail::cfg::get_logcfg_int(config, type(), name(), "UseCompression",
                                    config_.use_compression_)
            ? true
            : false;

    config_.verbose_level_ = detail::cfg::get_logcfg_int(
        config, type(), name(), "VerboseLevel", config_.verbose_level_);

    config_.max_buffered_messages_ = detail::cfg::get_logcfg_int(
        config, type(), name(), "MaxBufferedMessages",
        config_.max_buffered_messages_);

    config_.send_buffered_time_ms_ = detail::cfg::get_logcfg_int(
        config, type(), name(), "SendBufferedTimeMs",
        config_.send_buffered_time_ms_);

    config_.skip_messages_threshold_ = detail::cfg::get_logcfg_int(
        config, type(), name(), "SkipMessagesThreshold",
        config_.skip_messages_threshold_);

    config_.log_check_by_dns_addresses_ = detail::cfg::get_logcfg_string(
          config, type(), name(), "LogCheckByDnsAddresses",
          config_.log_check_by_dns_addresses_);

    config_.log_check_by_dns_ipv4_value_ = detail::cfg::get_logcfg_string(
          config, type(), name(), "LogCheckByDnsIPv4Value",
          config_.log_check_by_dns_ipv4_value_);

    config_.log_check_by_dns_ipv6_value_ = detail::cfg::get_logcfg_string(
          config, type(), name(), "LogCheckByDnsIPv6Value",
          config_.log_check_by_dns_ipv6_value_);

    if (reconnect) {
      config_.send_check_passed_ = false;
      config_.send_check_value_ = false;
      config_.send_check_retries_ = kDefaultSendCheckRetries;

      mt::atomic_increment(&recreate_network_connection_);
      mt::fire_event(&write_event_);
    }
  }

  bool attach(logger_interface* logger) LOG_METHOD_OVERRIDE {
    (void)logger;

    release_plugin_instance();
    config_initialize();
    network_thread_handle_ = detail::mt::thread_start(&network_thread_fn, this);

    return true;
  }

  void detach(logger_interface* logger) LOG_METHOD_OVERRIDE {
    (void)logger;
    release_plugin_instance();
  }

  /**
   * \brief    write method. Called every write to file operation
   */
  virtual void write(int verb_level, const std::string& hdr,
                     const std::string& what) LOG_METHOD_OVERRIDE {
    if ((verb_level & config_.verbose_level_) == 0) return;

    std::string str = hdr;
    if (hdr.size()) str.append(" ");
    str += what;

    detail::mt::mutex_scope_lock lock(&mt_buffer_lock_);
    msgs_.push_back(str);

    long bytes_in_buffer = bytes_in_buffer_;
    detail::mt::atomic_exchange(
        &bytes_in_buffer_, bytes_in_buffer + static_cast<long>(str.size()));

    // time to send?
    if (static_cast<int>(msgs_.size()) > config_.max_buffered_messages_ ||
        bytes_in_buffer_ > config_.max_buffered_bytes_) {
      detail::mt::fire_event(&write_event_);
    }
  }

  void flush(bool wait_ack) LOG_METHOD_OVERRIDE {
    detail::mt::fire_event(&write_event_);
    detail::mt::mutex_scope_lock lock(&mt_buffer_lock_);

    if (wait_ack && config_.flush_wait_ms_ != 0)
      detail::mt::wait_event(&buffer_devastated_, &mt_buffer_lock_, true,
                             config_.flush_wait_ms_);
  }

  void close() LOG_METHOD_OVERRIDE {}

 private:
  const int kDefaultSendCheckRetries = 3;

  /* Build log upload web request with parameters: os,cpu,product,id,run */
  static std::string build_web_request(
      const std::string& web_addr_base,
      const std::string& page,
      const std::string& os_type,
      const std::string& cpu_type,
      const std::string& run_id,
      const std::string& machine_id,
      const std::string& product_id) {

    std::stringstream ss;
    std::string webaddr = web_addr_base;
    webaddr = detail::str::trimr(webaddr, '/');

    ss << webaddr;
    ss << (detail::str::starts_with(page,"/") ? std::string("") : std::string("/")) << page;
    ss << "?id=" << machine_id << "&cpu=" << cpu_type << "&run=" << run_id
       << "&os=" << os_type << "&product=" << product_id;

    return ss.str();
  }

  /* Get RunID - unique run number. Based on process ID and current time */
  static unsigned long get_run_id() {
    int time_milliseconds;
    auto time_val = logging::detail::utils::get_time(time_milliseconds);

    unsigned long run_id =
        (logging::detail::utils::get_process_id() + time_val.tm_year +
         (time_val.tm_yday * 10) + (time_val.tm_hour * 50) +
         (time_val.tm_min * 100) + (time_val.tm_sec * 200)) ^
        time_milliseconds;

    return run_id;
  }

  /* Initialize configuration structure with default parameters */
  void config_initialize() {
    config_.send_check_retries_ = kDefaultSendCheckRetries;
    config_.web_address_ = std::string();
    config_.log_send_webaddr_ = "/logdata";
    config_.log_check_webaddr_ = "/logcheck";
    config_.use_compression_ = false;
    config_.flush_wait_ms_ = 5000;
    config_.verbose_level_ = 255; /* all messages */
    config_.max_buffered_messages_ = 100;
    config_.max_buffered_bytes_ = 1024 * 1024 * 1; /* 1 MB */
    config_.send_buffered_time_ms_ = 2000;
    config_.skip_messages_threshold_ = 600;
    config_.connection_encryption_policy_ =
        kConnectionEncryptionPolicy_Autodetect;

    config_.log_check_by_dns_addresses_ = "";
    config_.log_check_by_dns_ipv4_value_ = "8.8.8.8";
    config_.log_check_by_dns_ipv6_value_ = "2001:4860:4860::8888";

    config_.force_send_ = false;
    config_.send_check_passed_ = false;
    config_.send_check_value_ = false;

    config_.product_code_ = "000000";
    config_.machine_id_ = "0";

    config_.run_id_ = detail::str::stringformat("%u", get_run_id());
    config_.machine_id_ =
        detail::str::stringformat("%u", logging::detail::get_machine_id());
  }

  // prepare full request from page, e.g: page=/logcheck    result: https://192.168.60.6:10003/logcheck?id=13245&product=1324&cpu=amd64&run=679&os=windows
  std::string format_request_data(const std::string& page_base) {
    std::string check_req = build_web_request(
      config_.web_address_, page_base, detail::query_os_type(),
      detail::query_cpu_type(), config_.run_id_,
      config_.machine_id_, config_.product_code_);

    str_t full_req;
    str_init(&full_req);
    web_addr_parse(check_req.c_str(), nullptr, nullptr, nullptr, &full_req);

    std::string result = str_buf(&full_req);
    str_free(&full_req);

    return result;
  }

  /* Check that server is ready to receive logs */
  int process_check_send(bool& out_send_check_passed, bool& out_send_check_value) {

    bool enabled_by_dns = true;

    if (config_.log_check_by_dns_addresses_.size()) {
      enabled_by_dns = false;

      std::vector<std::string> dns_addrs;
      detail::str::split(config_.log_check_by_dns_addresses_, dns_addrs, ';');

      for (size_t i = 0; i < dns_addrs.size(); i++) {
        std::string addr = dns_addrs[i];
        if (!addr.size()) continue;

        struct ipaddr_text_type ipaddrs[6];
        int ip_count = net_resolve_dns_name(addr.c_str(), ipaddrs, 5);

        if (ip_count <= 0)
          continue;

         for (int j=0; j<ip_count; j++) {
           if (ipaddrs[j].is_ipv6 == 0 && config_.log_check_by_dns_ipv4_value_ == ipaddrs[j].addr_text) {
             enabled_by_dns = true;
             break;
           }

           if (ipaddrs[j].is_ipv6 == 1 && config_.log_check_by_dns_ipv6_value_ == detail::str::tolower(ipaddrs[j].addr_text)) {
             enabled_by_dns = true;
             break;
           }
         }

         if (enabled_by_dns)
           break;
       }
    }

    if (!enabled_by_dns)
      return 0;

    if (config_.log_check_webaddr_.size() == 0) // no need to check by logcheck
      return 1;

    int ret = network_connection_setup();
    if (ret < 0)
      return 0; // cannot setup network connection

    NetworkConnectionCheckSendResult r = network_connection_check_send_availability(
          net_cli_,
          str_buf(&host_name_),
          format_request_data(config_.log_check_webaddr_).c_str());

    out_send_check_passed = false;
    out_send_check_value = false;

    if (r == kLogSend_NetworkConnectionError) {
      network_connection_release();
      return 0;
    }

    if (r == kLogSend_HttpResponseError || r == kLogSend_TextResponseError)
      return -1; 

    if (r == kLogSend_DoNotSendLogs) {
      out_send_check_passed = true;
      out_send_check_value = false;
    }

    if (r == kLogSend_SendLogs) {
      out_send_check_passed = true;
      out_send_check_value = true;
//      printf("send logs enabled\n");
    } 

    return 1;
  }

  int process_send_log_data(const std::string& buffer_str, bool use_compression) {
    int ret = network_connection_setup();
    if (ret < 0)
      return ret;

    if (ret >= 0) {
      ret = network_connection_send_data(net_cli_, str_buf(&host_name_), format_request_data(config_.log_send_webaddr_).c_str(),
            buffer_str.c_str(), static_cast<int>(buffer_str.size()),
            use_compression);
    }

    if (ret < 0) {
      network_connection_release();
    }

    return ret;
  }

  /* Main network processing thread */
#ifdef LOG_PLATFORM_WINDOWS
  static unsigned long __stdcall
#else   /*LOG_PLATFORM_WINDOWS*/
  static void*
#endif  // LOG_PLATFORM_WINDOWS
      network_thread_fn(void* data) {
    detail::utils::set_current_thread_name("logger_network_thread");
    logger_network_output_plugin* this_ptr =
        reinterpret_cast<logger_network_output_plugin*>(data);

    bool process_messages = true;
    bool use_compression = false;

    while (process_messages) {
      std::stringstream ss;
      long checkout_messages = 0;
      long checkout_bytes = 0;

      /* Log sending check was not passed? */
      if (!this_ptr->config_.send_check_passed_ && this_ptr->config_.send_check_retries_) {
        int ret = this_ptr->process_check_send(this_ptr->config_.send_check_passed_, this_ptr->config_.send_check_value_);
        if (ret < 0) this_ptr->config_.send_check_retries_--;
      }

      {
        detail::mt::mutex_scope_lock lock(&this_ptr->mt_buffer_lock_);
        detail::mt::wait_event(&this_ptr->write_event_,
                               &this_ptr->mt_buffer_lock_, true,
                               this_ptr->config_.send_buffered_time_ms_);

        process_messages = !this_ptr->is_stop_;

        if (!this_ptr->msgs_.size()) continue;

        // send logs disabled
        if ((this_ptr->config_.send_check_passed_ && !this_ptr->config_.send_check_value_) || this_ptr->config_.send_check_retries_ == 0) {
          this_ptr->msgs_.clear();
          detail::mt::atomic_exchange(&this_ptr->bytes_in_buffer_, 0);

          /* signal that buffer has been processed */
          detail::mt::fire_event(&this_ptr->buffer_devastated_);
          continue;
        }

        int skipped_lines = 0;

        long bytes_in_buffer = this_ptr->bytes_in_buffer_;

        while (this_ptr->msgs_.size() >
               this_ptr->config_.skip_messages_threshold_) {
          bytes_in_buffer -= static_cast<long>(this_ptr->msgs_.front().size());
          this_ptr->msgs_.pop_front();
          ++skipped_lines;
        }

        if (skipped_lines) {
          ss << "!!!!!! SKIPPED LINES: " << skipped_lines << std::endl;
          detail::mt::atomic_exchange(&this_ptr->bytes_in_buffer_,
                                      bytes_in_buffer);
        }

        if (!this_ptr->config_.send_check_passed_ && this_ptr->config_.send_check_retries_) {
          continue;   // wait send check passed, do not remove messages
        }

        for (const std::string& msg : this_ptr->msgs_) {
          ss << msg << std::endl;
          ++checkout_messages;
          checkout_bytes += static_cast<long>(msg.size());
        }

        use_compression = this_ptr->config_.use_compression_;
      }

      const std::string& buffer_str = ss.str();

      int ret = this_ptr->process_send_log_data(buffer_str, use_compression);
      if (ret >= 0) {
        detail::mt::mutex_scope_lock lock(&this_ptr->mt_buffer_lock_);
        while (checkout_messages-- && this_ptr->msgs_.size()) {
          this_ptr->msgs_.pop_front();
        }

        long bytes_in_buffer = this_ptr->bytes_in_buffer_ - checkout_bytes;

        detail::mt::atomic_exchange(&this_ptr->bytes_in_buffer_,
                                    bytes_in_buffer);

        if (this_ptr->msgs_.size() == 0) {
          detail::mt::atomic_exchange(&this_ptr->bytes_in_buffer_, 0);
        }

        /* signal that buffer has been processed */
        detail::mt::fire_event(&this_ptr->buffer_devastated_);
      }
    }
    
    this_ptr->network_connection_release();

    {
      detail::mt::mutex_scope_lock lock(&this_ptr->mt_buffer_lock_);
      this_ptr->msgs_.clear();
      detail::mt::atomic_exchange(&this_ptr->bytes_in_buffer_, 0);
      detail::mt::fire_event(&this_ptr->buffer_devastated_);
    }

    return 0;
  }

  /* release network connection. Can be called only from network thread */
  int network_connection_release() {
    struct net_client_interface* net_cli = nullptr;
    net_cli = net_cli_;
    net_cli_ = nullptr;

    if (net_cli) {
      net_cli->close(net_cli);
      net_cli->destroy(net_cli);
      net_cli = nullptr;
    }

    return 0;
  }

  static int network_connection_init(
      const char* web_addr,
      int encryption_policy,
      str_t* inout_host_name,
      struct net_client_interface** out_net_cli) {

    int use_ssl = 0;
    int port;

    *out_net_cli = nullptr;

    int ret = web_addr_parse(web_addr, &use_ssl, &port, inout_host_name,
                             nullptr);
    if (ret < 0) {
      return -1;
    }

    if (encryption_policy == kConnectionEncryptionPolicy_ForceTls) use_ssl = 1;

    if (encryption_policy == kConnectionEncryptionPolicy_ForceUnencrypted)
      use_ssl = 0;


    struct net_client_interface* net_cli;
    struct net_client_interface* tcp_cli;

    tcp_cli = create_tcp_net_client();
    if (!tcp_cli) {
      return -1;
    }

    if (use_ssl) {
      net_cli = create_tls_net_client(tcp_cli);
      if (!net_cli) {
        return -1;
      }
    } else {
      net_cli = tcp_cli;
    }

    char port_str[128];
    snprintf(port_str, sizeof(port_str) - 1, "%d", port);
    ret = net_cli->connect(net_cli, str_buf(inout_host_name), port_str, 0);

    if (ret != 0) {
      if (net_cli != tcp_cli) {
        net_cli->close(net_cli);
        net_cli->destroy(net_cli);
        net_cli = nullptr;
        tcp_cli = nullptr;
      } else {
        tcp_cli->close(tcp_cli);
        tcp_cli->destroy(tcp_cli);
        tcp_cli = nullptr;
      }

      return ret;
    }

    *out_net_cli = net_cli;

    return 0;
  }

  int network_connection_setup() {
    if (recreate_network_connection_ || (net_cli_ && !net_cli_->is_connected(net_cli_))) {
//      printf("recreate_network_connection\n");
      detail::mt::atomic_exchange(&recreate_network_connection_, 0);
      network_connection_release();
    }

    std::string web_addr;
    int encryption_policy = 0;

    if (!net_cli_) {
      detail::mt::mutex_scope_lock lock(&mt_buffer_lock_);
      web_addr = config_.web_address_;
      encryption_policy = config_.connection_encryption_policy_;
    }

    if (!net_cli_ && web_addr.size()) {
      if (network_connection_init(web_addr.c_str(), encryption_policy, &host_name_,
                                  &net_cli_) != 0) {
        str_free(&host_name_);
        return -1;
      }      
    }

    if (net_cli_)
      return 0;
    else
      return -1;
  }

  enum NetworkConnectionCheckSendResult {
    kLogSend_NetworkConnectionError = -1,
    kLogSend_HttpResponseError = -2,
    kLogSend_TextResponseError = -3,
    kLogSend_DoNotSendLogs = 0,
    kLogSend_SendLogs = 1
  };

  static NetworkConnectionCheckSendResult network_connection_check_send_availability(
      struct net_client_interface* net_cli,
      const char* host_name,
      const char* page) {
    NetworkConnectionCheckSendResult op_result = kLogSend_DoNotSendLogs;
    
    str_t query;
    str_init(&query);

    build_http_get_query(host_name, page, true, &query);

    web_response_t* resp = nullptr;
    int ret = http_client_transaction(net_cli, str_buf(&query),
                                      str_length(&query), nullptr, 0, 0, &resp);

    str_free(&query);

    if (resp) {
      if (resp->code == 200)
        op_result = kLogSend_SendLogs;

      if (resp->code != 200) 
        op_result = kLogSend_DoNotSendLogs;

      web_response_free(resp);
    }

    if (ret < 0) {
      op_result = kLogSend_NetworkConnectionError;
    }

    return op_result;
  }

  /* Send data to Web server. Result: R<0 - connection or transaction error, other value - HTTP response code  */
  static int network_connection_send_data(
      struct net_client_interface* net_cli,
      const char* host_name,
      const char* page,
      const char* payload,
      int size,
      bool use_compression) {

    char* compressed_payload = nullptr;
    int compressed_payload_size = 0;

    str_t query;
    str_init(&query);

    if (use_compression) {
      // compress payload
      int ret = z_compress_deflate(payload, size, &compressed_payload,
                                   &compressed_payload_size, -1);

      if (ret != 0) use_compression = false;
      else {
        build_http_put_keepalive_query(host_name, page, compressed_payload_size,
                                     1, &query);      
      }
    }

    if (!use_compression) {
      build_http_post_keepalive_query(host_name, page, size,
                                      &query);
    }

    web_response_t* resp = nullptr;
    int ret = http_client_transaction(
        net_cli, str_buf(&query), str_length(&query),
        use_compression ? compressed_payload : payload,
        use_compression ? compressed_payload_size : size, 0, &resp);

    str_free(&query);

#ifdef _DEBUG
    if (ret < 0) {
//      printf("************ HTTP send ERROR %d. tried to send compressed bytes: %d, uncompressed: %d\n", ret, compressed_payload_size, size);
    }
//    if (resp) printf("******* log data sent, compressed bytes: %d, uncompressed: %d, result: %d\n", compressed_payload_size, size, resp->code);
#endif /*_DEBUG*/

    if (ret >= 0 && resp)
      ret = resp->code;

    if (resp) web_response_free(resp);

    if (compressed_payload) {
      z_compress_free_buffer(compressed_payload);
      compressed_payload = nullptr;
    }

    return ret;
  }

  void release_plugin_instance() {
    if (plugin_initialized_) {
      is_stop_ = true;
      detail::mt::fire_event(&write_event_);

      detail::mt::thread_join(&network_thread_handle_);
      plugin_initialized_ = false;
    }
  }

 private:
  LOG_MT_EVENT_TYPE write_event_;
  LOG_MT_EVENT_TYPE buffer_devastated_;
  LOG_MT_THREAD_HANDLE_TYPE network_thread_handle_;

  std::list<std::string> msgs_;

  LOG_MT_MUTEX mt_buffer_lock_;

  struct network_output_plugin_config_type {
    std::string web_address_; /* full address for connect */
    std::string log_check_webaddr_;
    std::string log_send_webaddr_;
    std::string product_code_;
    std::string machine_id_;
    std::string run_id_;

    bool send_check_passed_;
    bool send_check_value_;
    int send_check_retries_;

    bool force_send_; /* Force send even if server disallow that */

    bool use_compression_;      /* use gzip compression, HTTP PUT will be used.
                                   Otherwise - HTTP POST for uncompressed messages */
    int max_buffered_messages_; /* max messages in buffer*/
    int max_buffered_bytes_;    /* max bytes in buffer */
    int send_buffered_time_ms_; /* keep buffer for specified time before send if
                                   max messages in buffer were not reached */
    int verbose_level_;         /* verbose level filter */

    int connection_encryption_policy_; /* 0 - autodetect by http:// or https://
                                          in address, 1 - force TLS connection,
                                          2 - force non-encrypted connection */
    int flush_wait_ms_; /* wait some time on flush for messages send, when wait_ack=true. 0 - do not
                           wait, -1 - wait infinite */

    int skip_messages_threshold_; /* if messages count is greater than value,
                                     other messages will be skipped */

    std::string log_check_by_dns_addresses_;
    std::string log_check_by_dns_ipv6_value_;
    std::string log_check_by_dns_ipv4_value_;
  };

  enum connection_encryption_policy {
    kConnectionEncryptionPolicy_Autodetect = 0,
    kConnectionEncryptionPolicy_ForceTls = 1,
    kConnectionEncryptionPolicy_ForceUnencrypted = 2
  };

  network_output_plugin_config_type config_;

  struct net_client_interface* net_cli_;
  str_t host_name_;
  detail::mt::atomic_long_type bytes_in_buffer_;
  detail::mt::atomic_long_type recreate_network_connection_;
  std::string name_;
  bool is_stop_;
  bool plugin_initialized_;
};

class logger_network_output_plugin_factory
    : public logger_plugin_default_factory<logger_network_output_plugin> {
 public:
  logger_network_output_plugin_factory()
      : logger_plugin_default_factory<logger_network_output_plugin>(
            "network_output", kLogPluginTypeOutput) {}
  virtual ~logger_network_output_plugin_factory() LOG_METHOD_OVERRIDE {}
};

}  // namespace logging

#endif /*LOGGER_NETWORK_OUTPUT_PLUGIN_HEADER*/
