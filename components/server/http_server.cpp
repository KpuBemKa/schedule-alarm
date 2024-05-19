#include "http_server.hpp"

#include <cstdio>

#include "cJSON.h"
#include "esp_check.h"
#include "esp_chip_info.h"
#include "esp_log.h"
#include "esp_random.h"

#include "settings.hpp"
#include "uri_encoder/uri_encoder.hpp"

const char TAG[] = "HTTP_SERVER";

#if DEBUG_HTTP
#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#else
#define LOG_I(...)
#endif

#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN (64)

namespace server {

esp_err_t
HttpServer::StartServer(std::string_view base_path)
{
  constexpr httpd_uri_t uri_handler_redirect = {
    .uri = "/index.html", .method = HTTP_GET, .handler = HttpServer::GetIndexHtmlHandler, .user_ctx = nullptr
  };
  constexpr httpd_uri_t uri_handler_empty = {
    .uri = "/", .method = HTTP_GET, .handler = HttpServer::GetEmptyHandler, .user_ctx = nullptr
  };
  constexpr httpd_uri_t uri_handler_favicon = {
    .uri = "/favicon.ico", .method = HTTP_GET, .handler = HttpServer::GetFaviconHandler, .user_ctx = nullptr
  };

  constexpr httpd_config_t http_config = { .task_priority = tskIDLE_PRIORITY + 5,
                                           .stack_size = 4096,
                                           .core_id = tskNO_AFFINITY,
                                           .server_port = 80,
                                           .ctrl_port = ESP_HTTPD_DEF_CTRL_PORT,
                                           .max_open_sockets = 7,
                                           .max_uri_handlers = 8,
                                           .max_resp_headers = 8,
                                           .backlog_conn = 5,
                                           .lru_purge_enable = true,
                                           .recv_wait_timeout = 5,
                                           .send_wait_timeout = 5,
                                           .global_user_ctx = NULL,
                                           .global_user_ctx_free_fn = NULL,
                                           .global_transport_ctx = NULL,
                                           .global_transport_ctx_free_fn = NULL,
                                           .enable_so_linger = false,
                                           .linger_timeout = 0,
                                           .keep_alive_enable = false,
                                           .keep_alive_idle = 0,
                                           .keep_alive_interval = 0,
                                           .keep_alive_count = 0,
                                           .open_fn = NULL,
                                           .close_fn = NULL,
                                           .uri_match_fn = NULL };

  mRestContext.base_path = base_path;

  // Start the httpd server
  LOG_I("Starting server on port: '%d'", http_config.server_port);
  esp_err_t esp_result = httpd_start(&mServerHandle, &http_config);
  if (esp_result != ESP_OK) {
    LOG_E("%s:%d | Could not start the HTTP server: %s", __FILE__, __LINE__, esp_err_to_name(esp_result));
    return esp_result;
  }

  // Set URI handlers
  LOG_I("Registering URI handlers...");
  bool full_success = true;

  esp_result = httpd_register_uri_handler(mServerHandle, &uri_handler_redirect);
  if (esp_result != ESP_OK) {
    LOG_E("%s:%d | Handler for '%s' could not be registered: %s",
          __FILE__,
          __LINE__,
          uri_handler_redirect.uri,
          esp_err_to_name(esp_result));
    full_success = false;
  }

  esp_result = httpd_register_uri_handler(mServerHandle, &uri_handler_empty);
  if (esp_result != ESP_OK) {
    LOG_E("%s:%d | Handler for '%s' could not be registered: %s",
          __FILE__,
          __LINE__,
          uri_handler_empty.uri,
          esp_err_to_name(esp_result));
    full_success = false;
  }

  esp_result = httpd_register_uri_handler(mServerHandle, &uri_handler_favicon);
  if (esp_result != ESP_OK) {
    LOG_E("%s:%d | Handler for '%s' could not be registered: %s",
          __FILE__,
          __LINE__,
          uri_handler_favicon.uri,
          esp_err_to_name(esp_result));
    full_success = false;
  }

  // httpd_register_uri_handler(mServerHandle, &echo);
  // httpd_register_uri_handler(mServerHandle, &ctrl);
  // httpd_register_uri_handler(mServerHandle, &any);

  return full_success ? ESP_OK : ESP_FAIL;
}

esp_err_t
HttpServer::GetIndexHtmlHandler(httpd_req_t* req)
{
  httpd_resp_set_status(req, "307 Temporary Redirect");
  httpd_resp_set_hdr(req, "Location", "/");
  httpd_resp_send(req, NULL, 0); // Response body can be empty
  return ESP_OK;
}

esp_err_t
HttpServer::GetEmptyHandler(httpd_req_t* req)
{
  // char* buf;
  // size_t buf_len;

  /* Get header value string length and allocate memory for length + 1,
   * extra byte for null termination */
  // buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
  // if (buf_len > 1) {
  //   buf = reinterpret_cast<char*>(malloc(buf_len));
  //   ESP_RETURN_ON_FALSE(buf, ESP_ERR_NO_MEM, TAG, "buffer alloc failed");
  //   /* Copy null terminated value string into buffer */
  //   if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
  //     LOG_I("Found header => Host: %s", buf);
  //   }
  //   free(buf);
  // }

  // buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1;
  // if (buf_len > 1) {
  //   buf = reinterpret_cast<char*>(malloc(buf_len));
  //   ESP_RETURN_ON_FALSE(buf, ESP_ERR_NO_MEM, TAG, "buffer alloc failed");
  //   if (httpd_req_get_hdr_value_str(req, "Test-Header-2", buf, buf_len) == ESP_OK) {
  //     LOG_I("Found header => Test-Header-2: %s", buf);
  //   }
  //   free(buf);
  // }

  // buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1;
  // if (buf_len > 1) {
  //   buf = reinterpret_cast<char*>(malloc(buf_len));
  //   ESP_RETURN_ON_FALSE(buf, ESP_ERR_NO_MEM, TAG, "buffer alloc failed");
  //   if (httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK) {
  //     LOG_I("Found header => Test-Header-1: %s", buf);
  //   }
  //   free(buf);
  // }

  /* Read URL query string length and allocate memory for length + 1,
   * extra byte for null termination */
  // buf_len = httpd_req_get_url_query_len(req) + 1;
  // if (buf_len > 1) {
  //   buf = reinterpret_cast<char*>(malloc(buf_len));
  //   ESP_RETURN_ON_FALSE(buf, ESP_ERR_NO_MEM, TAG, "buffer alloc failed");

  //   if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
  //     LOG_I("Found URL query => %s", buf);

  //     char param[EXAMPLE_HTTP_QUERY_KEY_MAX_LEN] /* , dec_param[EXAMPLE_HTTP_QUERY_KEY_MAX_LEN] = { 0 } */;
  //     std::string decoded_param;

  //     /* Get value of expected key from query string */
  //     if (httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK) {
  //       LOG_I("Found URL query parameter => query1=%s", param);
  //       decoded_param = util::uri_decode(std::string_view(param, strnlen(param, EXAMPLE_HTTP_QUERY_KEY_MAX_LEN)));
  //       LOG_I("Decoded query parameter => %s", decoded_param.c_str());
  //     }

  //     if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK) {
  //       LOG_I("Found URL query parameter => query3=%s", param);
  //       decoded_param = util::uri_decode(std::string_view(param, strnlen(param, EXAMPLE_HTTP_QUERY_KEY_MAX_LEN)));
  //       LOG_I("Decoded query parameter => %s", decoded_param.c_str());
  //     }

  //     if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK) {
  //       LOG_I("Found URL query parameter => query2=%s", param);
  //       decoded_param = util::uri_decode(std::string_view(param, strnlen(param, EXAMPLE_HTTP_QUERY_KEY_MAX_LEN)));
  //       LOG_I("Decoded query parameter => %s", decoded_param.c_str());
  //     }
  //   }
  //   free(buf);
  // }

  /* Set some custom headers */
  // httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
  // httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

  /* Send response with custom headers and body set as the
   * string passed in user context*/
  // const char* resp_str = (const char*)req->user_ctx;

  return SendFile(req, "/spiffs/index.html", "text/html");

  // const char resp_str[] = "Hello, World!";
  // httpd_resp_send(req, resp_str, sizeof(resp_str));

  // /* After sending the HTTP response the old HTTP request
  //  * headers are lost. Check if HTTP request headers can be read now. */
  // if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
  //   LOG_I("Request headers lost");
  // }

  // return ESP_OK;
}

esp_err_t
HttpServer::GetFaviconHandler(httpd_req_t* req)
{
  return SendFile(req, "/spiffs/favicon.ico", "image/x-icon");
}

esp_err_t
HttpServer::SendFile(httpd_req_t* req, const std::string_view file_name, const std::string_view response_type)
{
  // Open the file
  FILE* file_ptr = fopen(file_name.data(), "r");
  if (file_ptr == nullptr) {
    LOG_E("%s:%d | Error opening requested file '%s'.", __FILE__, __LINE__, file_name.data());
    return ESP_ERR_NOT_FOUND;
  }

  // Retreive the file size
  fseek(file_ptr, 0L, SEEK_END);
  std::size_t file_size = ftell(file_ptr);
  fseek(file_ptr, 0L, SEEK_SET);

  // Reserve memory for the file
  char* file_buf = new (std::nothrow) char[file_size];
  if (file_buf == nullptr) {
    LOG_E("%s:%d | Error reserving memory for file '%s'.", __FILE__, __LINE__, file_name.data());
    return ESP_ERR_NO_MEM;
  }

  // Copy file data into the reserved memory
  const std::size_t read_count = fread(file_buf, sizeof(char), file_size, file_ptr);
  if (read_count != file_size) {
    LOG_E("%s:%d | Error reading the entire file '%s'. Ammount of bytes read: %u",
          __FILE__,
          __LINE__,
          file_name.data(),
          read_count);
    return ESP_ERR_NOT_FINISHED;
  }

  // Send the copied file
  esp_err_t result = httpd_resp_set_type(req, response_type.data());
  result |= httpd_resp_send(req, file_buf, file_size);
  if (read_count != file_size) {
    LOG_E("%s:%d | Error sending file '%s': %s", __FILE__, __LINE__, file_name.data(), esp_err_to_name(result));
    return result;
  }

  // Close the file
  if (fclose(file_ptr) != 0) {
    LOG_E("%s:%d | Error closing file '%s'.", __FILE__, __LINE__, file_name.data());
    return result;
  }

  delete[] file_buf;
  return ESP_OK;
}

} // namespace server
