#pragma once

#include <stdint.h>
#include <functional>
#include <string>
#include <string_view>

#include "esp_err.h"

#include "esp_http_server.h"

namespace srvr {

class HttpController {
   public:
    esp_err_t StartServer();
    esp_err_t StopServer();

    bool IsStarted() { return mIsStarted; }

   private:
    /* -=< Esp-Idf HTTP handlers */

    /// @brief Handler to redirect incoming GET request from `/index.html` to
    /// `/` This can be overridden by uploading file with same name
    static esp_err_t IndexHtmlHandlerGET(httpd_req_t* req);
    /// @brief Handler to handle `/` path
    static esp_err_t EmptyHandlerGET(httpd_req_t* req);
    /// @brief Handle to fetch the site favicon
    static esp_err_t FaviconHandlerGET(httpd_req_t* req);

    static esp_err_t ScheduleUploadHandler(httpd_req_t* req);
    static esp_err_t ScheduleRetreiveHandler(httpd_req_t* req);

    static esp_err_t SettingsUploadHandler(httpd_req_t* req);
    static esp_err_t SettingsRetreiveHandler(httpd_req_t* req);

   private:
    static esp_err_t SendFile(httpd_req_t* req,
                              const std::string_view file_name,
                              const std::string_view response_type);

   private:
    httpd_handle_t mServerHandle;

    bool mIsStarted = false;
};

}  // namespace srvr
