#include "sleep.hpp"

#include <ctime>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_sleep.h"

#include "config.hpp"

const char TAG[] = "SLEEP";

#if DEBUG_SLEEP
#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#else
#define LOG_I(...)
#endif

#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

esp_err_t
SleepController::PrepareForSleep()
{
    esp_err_t esp_result = m_http_controller.StopServer();
    if (esp_result != ESP_OK && esp_result != ESP_ERR_INVALID_STATE) {
        LOG_E("%s:%d | Failed to stop HTTP server: %s\n", __FILE__, __LINE__, esp_err_to_name(esp_result));
        return esp_result;
    }

    esp_result = m_wifi_controller.Stop();
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Failed to stop Wifi: %s\n", __FILE__, __LINE__, esp_err_to_name(esp_result));
    }

    return esp_result;
}

esp_err_t
SleepController::EnterSleep()
{
    esp_err_t esp_result = PrepareForSleep();
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Prepare for sleep failed: %s\n", __FILE__, __LINE__, esp_err_to_name(esp_result));
        return esp_result;
    }

    esp_result = ConfigureWakeup();
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Wakeup configuration failed: %s\n", __FILE__, __LINE__, esp_err_to_name(esp_result));
        return esp_result;
    }

    LOG_I("Entering sleep...\n");

    vTaskDelay(pdMS_TO_TICKS(500));

    esp_result = esp_light_sleep_start();
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Failed to enter sleep mode: %s\n", __FILE__, __LINE__, esp_err_to_name(esp_result));
        return esp_result;
    }

    return ESP_OK;
}

esp_err_t
SleepController::ConfigureWakeup()
{
    esp_err_t esp_result =
      gpio_wakeup_enable(static_cast<gpio_num_t>(pins::BUTTON), gpio_int_type_t::GPIO_INTR_LOW_LEVEL);
    esp_result |= esp_sleep_enable_gpio_wakeup();
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Failed to configure wakeup GPIO: %s\n", __FILE__, __LINE__, esp_err_to_name(esp_result));
        return esp_result;
    }

    esp_result = esp_sleep_enable_timer_wakeup(GetSleepTimeUs());
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Failed to configure wakeup timer: %s\n", __FILE__, __LINE__, esp_err_to_name(esp_result));
        return esp_result;
    }

    return ESP_OK;
}

uint64_t
SleepController::GetSleepTimeUs()
{
    return 30 * 1'000 * 1'000;
    // minutes to microseconds conversion factor
    constexpr uint64_t MINUTES_TO_US = 60 * 1'000 * 1'000;

    if (mSchedule.IsEmpty()) {
        return 60 * MINUTES_TO_US;
    }

    const auto minute_difference = mSchedule.GetSchedulePoint().day_minute - mSchedule.GetSystemMinute();

    return minute_difference * MINUTES_TO_US; // microseconds
}