#include "sntp.hpp"

#include <cstdlib>
#include <ctime>

#include "config.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

const char TAG[] = "SNTP";

#if DEBUG_WIFI
#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#else
#define LOG_I(...)
#endif

#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

namespace wifi {

esp_err_t
SNTP::Init(const std::size_t wait_for_sync_ms)
{
    LOG_I("Syncronizing system time...");

    const esp_sntp_config_t sntp_config{
        .smooth_sync = false,
        .server_from_dhcp = false,
        .wait_for_sync = true,
        .start = true,
        .sync_cb =
          [](timeval* tv) {
              const std::tm* dt = localtime(&tv->tv_sec);

              LOG_I("System time has been syncronized with SNTP. "
                    "Current system time: %4d.%02d.%02d %02d:%02d:%02d",
                    dt->tm_year + 1900,
                    dt->tm_mon,
                    dt->tm_mday,
                    dt->tm_hour,
                    dt->tm_min,
                    dt->tm_sec);
          },
        .renew_servers_after_new_IP = false,
        .ip_event_to_renew = IP_EVENT_STA_GOT_IP,
        .index_of_first_server = 0,
        .num_of_servers = 1,
        .servers = { "pool.ntp.org" },
    };

    esp_err_t esp_result = esp_netif_sntp_init(&sntp_config);
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Error initializing SNTP: %s", __FILE__, __LINE__, esp_err_to_name(esp_result));
        return esp_result;
    }

    if (wait_for_sync_ms > 0) {
        esp_result = esp_netif_sntp_sync_wait(pdMS_TO_TICKS(wait_for_sync_ms));
        if (esp_result != ESP_OK) {
            LOG_E("%s:%d | Error waiting for SNTP sync: %s", __FILE__, __LINE__, esp_err_to_name(esp_result));
            return esp_result;
        }
    }

    return ESP_OK;
}

void
SNTP::SetTimezone(const std::string_view timezone)
{
    setenv("TZ", timezone.data(), 1);
    tzset();
}

} // namespace wifi
