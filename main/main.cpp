#include <string.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "esp_wifi.h"
// #include "file_serving_example_common.h"
#include "driver/gpio.h"
#include "esp_spiffs.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "nvs_flash.h"

#include "config.hpp"
#include "http_controller.hpp"
#include "schedule.hpp"
#include "settings.hpp"
#include "sleep.hpp"
#include "wifi_controller.hpp"

static const char TAG[] = "MAIN";

#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

bool s_button_pressed_flag = false;

settings::Settings& s_settings = settings::Settings::GetInstance();
Schedule s_schedule;
wifi::WifiController s_wifi_controller(s_settings);
srvr::HttpController s_http_controller(s_schedule, s_settings);
SleepController s_sleep_controller(s_wifi_controller, s_http_controller, s_schedule);

TaskHandle_t s_schedule_task_handle;
TaskHandle_t s_server_task_handle;

esp_err_t MountSpiffsStorage(const char* base_path);

void ScheduleTask(void* args);
void HttpServerTask(void* args);

void RingAlarm(const AlarmType alarm_type);

extern "C" void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());

    /* Initialize file storage */
    ESP_ERROR_CHECK(MountSpiffsStorage(SPIFFS_BASE_PATH.data()));

    s_schedule.Load();

    s_settings.UpdateSettings(settings::Changable{.wifi_ssid = "School Alarm",
                                                  .wifi_password = "elimul nou",
                                                  //   .wifi_password = "",
                                                  .remote_wifi_ssid = "Penzari-2",
                                                  .remote_wifi_password = "?068882210!_"});

    // std::array schedule = {SchedulePoint{.day_minute = 0, .alarm_type = AlarmType::Single},
    //                        SchedulePoint{.day_minute = 65, .alarm_type = AlarmType::Double},
    //                        SchedulePoint{.day_minute = 120, .alarm_type = AlarmType::Triple}};
    // s_schedule.SetSchedule(schedule);
    s_sleep_controller.SetSleepTimeout(pdMS_TO_TICKS(60'000));

    // xTaskCreate(ScheduleTask, "SCHEDULE", 8192, nullptr, 8,
    //             &s_schedule_task_handle);
    xTaskCreatePinnedToCore(HttpServerTask, "SERVER", 16384, nullptr, 8, &s_schedule_task_handle,
                            0);
    // xTaskCreate(HttpServerTask, "SERVER", 16384, nullptr, 8, &s_schedule_task_handle);

    // while (true) {
    //     vTaskDelay(pdMS_TO_TICKS(10'000));
    // }
}

void ScheduleTask(void* args) {
    while (true) {
        const auto schedule_point = s_schedule.GetSchedulePoint();
        if (s_schedule.GetSystemMinute() != schedule_point.day_minute)
            continue;

        s_sleep_controller.ResetSleepTimeout();
        RingAlarm(schedule_point.alarm_type);
        s_schedule.AdvanceSchedule();

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void HttpServerTask(void* args) {
    while (true) {
        // if (!s_button_pressed_flag)
        //     continue;

        s_button_pressed_flag = false;

        if (!s_wifi_controller.IsStarted()) {
            ESP_ERROR_CHECK(s_wifi_controller.Start());
            ESP_ERROR_CHECK(s_http_controller.StartServer());
        }

        while (s_wifi_controller.GetConnectionsCount() > 0) {
            s_sleep_controller.ResetSleepTimeout();
            vTaskDelay(pdMS_TO_TICKS(1'000));
        }

        vTaskDelay(pdMS_TO_TICKS(1'000));
    }
}

esp_err_t MountSpiffsStorage(const char* base_path) {
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {.base_path = base_path,
                                  .partition_label = NULL,
                                  .max_files = 5,  // This sets the maximum number of files
                                                   // that can be open at the same time
                                  .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}

void RingAlarm(const AlarmType alarm_type) {
    LOG_I("Ringing the alarm...");
    std::size_t cycles = static_cast<uint16_t>(alarm_type);

    for (std::size_t i = 0; i < cycles; ++i) {
        gpio_set_level(pins::RELAY, 1);
        vTaskDelay(pdMS_TO_TICKS(1'000));
        gpio_set_level(pins::RELAY, 0);
        vTaskDelay(pdMS_TO_TICKS(1'000));
    }
}