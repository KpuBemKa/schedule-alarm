#pragma once

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
  /* Set HTTP response content type according to file extension */
  esp_err_t set_content_type_from_file(httpd_req_t* req, const char* filepath);

  /* Send HTTP response with the contents of the requested file */
  esp_err_t rest_common_get_handler(httpd_req_t* req);

  /* Simple handler for light brightness control */
  esp_err_t light_brightness_post_handler(httpd_req_t* req);

  /* Simple handler for getting system handler */
  esp_err_t system_info_get_handler(httpd_req_t* req);

  /* Simple handler for getting temperature data */
  esp_err_t temperature_data_get_handler(httpd_req_t* req);

private:
  RestServerContext mRestContext;
  httpd_handle_t mServerHandle;
};

} // namespace server
