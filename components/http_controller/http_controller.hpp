#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <string_view>
#include <vector>

#include "cJSON.h"
#include "esp_err.h"
#include "esp_http_server.h"

#include "schedule.hpp"
#include "settings.hpp"

namespace srvr {

class HttpController
{
  public:
    HttpController(schd::Schedule& schedule, settings::Settings& settings)
      : mSchedule(schedule)
      , mSettings(settings)
    {
    }

    esp_err_t StartServer();
    esp_err_t StopServer();

    bool IsStarted() { return mIsStarted; }

    int GetConnectionsCount();

  private:
    /* -=< Esp-Idf HTTP handlers */

    /// @brief Handler to redirect incoming GET request from `/index.html` to
    /// `/` This can be overridden by uploading file with same name
    static esp_err_t IndexHtmlHandlerGET(httpd_req_t* req);
    /// @brief Handler to handle `/` path
    static esp_err_t EmptyHandlerGET(httpd_req_t* req);
    /// @brief Handle to fetch the site favicon
    static esp_err_t FaviconHandlerGET(httpd_req_t* req);
    /// @brief Handle to fetch the settings page
    static esp_err_t SettingsPageHandlerGET(httpd_req_t* req);

    /// @brief /update_schedule
    static esp_err_t SchedulePOST(httpd_req_t* req);
    /// @brief /get_schedule
    static esp_err_t ScheduleGET(httpd_req_t* req);

    /// @brief /update_settings
    static esp_err_t SettingsPOST(httpd_req_t* req);
    /// @brief /get_settings
    static esp_err_t SettingsGET(httpd_req_t* req);

    /// @brief /get_time
    static esp_err_t SystemTimeGET(httpd_req_t* req);
    /// @brief /sync_time
    static esp_err_t SyncTimePOST(httpd_req_t* req);

    static esp_err_t SendFile(httpd_req_t* req, const std::string_view file_name, const std::string_view content_type);

    static esp_err_t SendString(httpd_req_t* req, const std::string_view string, const std::string_view content_type);

  private:
    schd::Schedule& mSchedule;
    settings::Settings& mSettings;

    httpd_handle_t mServerHandle;

    std::array<char, 2048> mReceiveBuffer;

    bool mIsStarted = false;
};

} // namespace srvr
