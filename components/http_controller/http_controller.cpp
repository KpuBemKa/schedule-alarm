#include "http_controller.hpp"

#include <cstdio>
#include <cstring>
#include <string>

#include "esp_check.h"
#include "esp_chip_info.h"
#include "esp_log.h"
#include "esp_random.h"

#include "config.hpp"
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

namespace srvr {

constexpr httpd_config_t DEFAULT_HTTP_CONFIG = {.task_priority = tskIDLE_PRIORITY + 5,
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
                                                .uri_match_fn = NULL};

esp_err_t HttpController::StartServer() {
    constexpr httpd_uri_t uri_handler_redirect = {.uri = "/index.html",
                                                  .method = HTTP_GET,
                                                  .handler = HttpController::IndexHtmlHandlerGET,
                                                  .user_ctx = nullptr};
    constexpr httpd_uri_t uri_handler_get_empty = {.uri = "/",
                                                   .method = HTTP_GET,
                                                   .handler = HttpController::EmptyHandlerGET,
                                                   .user_ctx = nullptr};
    constexpr httpd_uri_t uri_handler_get_favicon = {.uri = "/favicon.ico",
                                                     .method = HTTP_GET,
                                                     .handler = HttpController::FaviconHandlerGET,
                                                     .user_ctx = nullptr};
    httpd_uri_t uri_handler_get_schedule = {.uri = "/get_schedule",
                                            .method = HTTP_GET,
                                            .handler = HttpController::ScheduleGET,
                                            .user_ctx = this};
    httpd_uri_t uri_handler_post_schedule = {.uri = "/update_schedule",
                                             .method = HTTP_POST,
                                             .handler = HttpController::SchedulePOST,
                                             .user_ctx = this};

    // Start the httpd server
    LOG_I("Starting server on port %d...", DEFAULT_HTTP_CONFIG.server_port);
    // esp_err_t esp_result = httpd_start(&mServerHandle, &DEFAULT_HTTP_CONFIG);
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    esp_err_t esp_result = httpd_start(&mServerHandle, &config);
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Could not start the HTTP server: %s", __FILE__, __LINE__,
              esp_err_to_name(esp_result));
        return esp_result;
    }

    mIsStarted = true;

    // Set the URI handlers
    LOG_I("Server has been started. Registering URI handlers...");
    bool full_success = true;

    esp_result = httpd_register_uri_handler(mServerHandle, &uri_handler_redirect);
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Handler for '%s' could not be registered: %s", __FILE__, __LINE__,
              uri_handler_redirect.uri, esp_err_to_name(esp_result));
        full_success = false;
    }

    esp_result = httpd_register_uri_handler(mServerHandle, &uri_handler_get_empty);
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Handler for '%s' could not be registered: %s", __FILE__, __LINE__,
              uri_handler_get_empty.uri, esp_err_to_name(esp_result));
        full_success = false;
    }

    esp_result = httpd_register_uri_handler(mServerHandle, &uri_handler_get_favicon);
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Handler for '%s' could not be registered: %s", __FILE__, __LINE__,
              uri_handler_get_favicon.uri, esp_err_to_name(esp_result));
        full_success = false;
    }

    esp_result = httpd_register_uri_handler(mServerHandle, &uri_handler_get_schedule);
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Handler for '%s' could not be registered: %s", __FILE__, __LINE__,
              uri_handler_get_schedule.uri, esp_err_to_name(esp_result));
        full_success = false;
    }

    esp_result = httpd_register_uri_handler(mServerHandle, &uri_handler_post_schedule);
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Handler for '%s' could not be registered: %s", __FILE__, __LINE__,
              uri_handler_post_schedule.uri, esp_err_to_name(esp_result));
        full_success = false;
    }

    LOG_I("URI handlers have been registered.");

    return full_success ? ESP_OK : ESP_FAIL;
}

esp_err_t HttpController::StopServer() {
    const esp_err_t esp_error = httpd_stop(mServerHandle);

    if (esp_error != ESP_OK) {
        LOG_E("%s:%d | Error stopping the HTTP server: %s", __FILE__, __LINE__,
              esp_err_to_name(esp_error));
        return esp_error;
    }

    LOG_I("HTTP server has been stopped.");

    mIsStarted = false;

    return ESP_OK;
}

esp_err_t HttpController::IndexHtmlHandlerGET(httpd_req_t* req) {
    httpd_resp_set_status(req, "307 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);  // Response body can be empty
    return ESP_OK;
}

esp_err_t HttpController::EmptyHandlerGET(httpd_req_t* req) {
    return SendFile(req, "/spiffs/index.html", "text/html");
}

esp_err_t HttpController::FaviconHandlerGET(httpd_req_t* req) {
    return SendFile(req, "/spiffs/favicon.ico", "image/x-icon");
}

esp_err_t HttpController::SchedulePOST(httpd_req_t* req) {
    std::string schedule_raw_json;
    int remaining = req->content_len;
    std::size_t retries_count = 0;
    auto self = reinterpret_cast<HttpController*>(req->user_ctx);

    while (remaining > 0) {
        LOG_I("Remaining to receive: %d", remaining);

        int received =
            httpd_req_recv(req, self->mReceiveBuffer.data(), self->mReceiveBuffer.size());

        if (retries_count > 5) {
            LOG_E("%s:%d | Schedule reception failed: TIMEOUT", __FILE__, __LINE__);
            return ESP_ERR_TIMEOUT;
        }

        if (received <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                ++retries_count;
                continue;
            }

            LOG_E("%s:%d | Schedule reception failed. Code: %d", __FILE__, __LINE__, received);

            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
                                "Failed to receive the schedule");
            return ESP_FAIL;
        }

        retries_count = 0;

        schedule_raw_json.append(self->mReceiveBuffer.data(), received);
        remaining -= received;
    }

    LOG_I("Received data: %.*s", schedule_raw_json.length(), schedule_raw_json.c_str());

    const esp_err_t esp_result = self->mSchedule.FromJson(schedule_raw_json);

    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Schedule parsing failed: %s", __FILE__, __LINE__,
              esp_err_to_name(esp_result));

        if (esp_result == ESP_ERR_INVALID_ARG) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Invalid JSON");
        } else {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Internal server error.");
        }

        return esp_result;
    }

    httpd_resp_sendstr(req, "Schedule has been updated.");

    LOG_I("Schedule has been updated.");

    return ESP_OK;
}

esp_err_t HttpController::ScheduleGET(httpd_req_t* req) {
    auto self = reinterpret_cast<HttpController*>(req->user_ctx);

    std::string schedule_json = self->mSchedule.ToJson();

    const esp_err_t esp_result = SendString(req, schedule_json, "application/json");
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Error sending schedule: %s", __FILE__, __LINE__,
              esp_err_to_name(esp_result));
    }

    return esp_result;
}

esp_err_t HttpController::SettingsPOST(httpd_req_t* req) {
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t HttpController::SettingsGET(httpd_req_t* req) {
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t HttpController::SendFile(httpd_req_t* req,
                                   const std::string_view file_name,
                                   const std::string_view content_type) {
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
    std::vector<char> file_buf(file_size, '\0');

    // Copy file data into the reserved memory
    const std::size_t read_count = fread(file_buf.data(), sizeof(char), file_size, file_ptr);
    if (read_count != file_size) {
        LOG_E(
            "%s:%d | Error reading the entire file '%s'. Ammount of bytes "
            "read: %u",
            __FILE__, __LINE__, file_name.data(), read_count);
        return ESP_ERR_NOT_FINISHED;
    }

    // Send the copied file
    esp_err_t result = httpd_resp_set_type(req, content_type.data());
    result |= httpd_resp_send(req, file_buf.data(), file_size);
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error sending file '%s': %s", __FILE__, __LINE__, file_name.data(),
              esp_err_to_name(result));
        return result;
    }

    // Close the file
    if (fclose(file_ptr) != 0) {
        LOG_E("%s:%d | Error closing file '%s'.", __FILE__, __LINE__, file_name.data());
        return result;
    }

    return ESP_OK;
}

esp_err_t HttpController::SendString(httpd_req_t* req,
                                     const std::string_view string,
                                     const std::string_view content_type) {
    // char* json_str = cJSON_Print(data);
    // const std::size_t json_str_size = std::strlen(json_str);

    esp_err_t result = httpd_resp_set_type(req, content_type.data());
    result |= httpd_resp_send(req, string.data(), string.size());
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error sending JSON: %s", __FILE__, __LINE__, esp_err_to_name(result));
    }

    return result;
}

}  // namespace srvr
