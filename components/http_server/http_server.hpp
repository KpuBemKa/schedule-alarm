#pragma once

#include <stdint.h>
#include <string>
#include <string_view>

#include "esp_err.h"

#include "esp_http_server.h"

namespace server {

class HttpServer
{
public:
  esp_err_t StartServer();
  esp_err_t StopServer();

private:
  /* -=< Esp-Idf HTTP handlers */

  /// @brief Handler to redirect incoming GET request from `/index.html` to `/`
  /// This can be overridden by uploading file with same name
  static esp_err_t GetIndexHtmlHandler(httpd_req_t* req);
  /// @brief Handler to handle `/` path
  static esp_err_t GetEmptyHandler(httpd_req_t* req);
  /// @brief Handle to fetch the site favicon
  static esp_err_t GetFaviconHandler(httpd_req_t* req);

  static esp_err_t PostAlarmInfoUpload(httpd_req_t* req);

private:
  static esp_err_t SendFile(httpd_req_t* req, const std::string_view file_name, const std::string_view response_type);

private:
  httpd_handle_t mServerHandle;
};

} // namespace server
