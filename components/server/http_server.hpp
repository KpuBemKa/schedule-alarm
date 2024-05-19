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
  /* An HTTP GET handler */
  static esp_err_t EmptyHandler(httpd_req_t* req);

private:
  RestServerContext mRestContext;
  httpd_handle_t mServerHandle;
};

} // namespace server
