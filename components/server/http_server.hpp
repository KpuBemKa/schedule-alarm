#pragma once

#include <stdint.h>
#include <string>
#include <string_view>

#include "esp_err.h"

#include "esp_http_server.h"

namespace server {

struct RestServerContext
{
  std::string base_path;
  std::string scratch;
};

class HttpServer
{
public:
  esp_err_t StartServer(std::string_view base_path);

private:
  /* Handler to redirect incoming GET request for `/index.html` to `/`
   * This can be overridden by uploading file with same name */
  static esp_err_t GetIndexHtmlHandler(httpd_req_t* req);

  static esp_err_t GetEmptyHandler(httpd_req_t* req);
  static esp_err_t GetFaviconHandler(httpd_req_t* req);

  static esp_err_t SendFile(httpd_req_t* req, const std::string_view file_name, const std::string_view response_type);

private:
  RestServerContext mRestContext;
  httpd_handle_t mServerHandle;
};

} // namespace server
