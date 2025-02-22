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
    return m_http_controller.StopServer() | m_wifi_controller.Stop();
}

esp_err_t
SleepController::EnterSleep()
{
    PrepareForSleep();
    ConfigureWakeup();

    LOG_I("Entering sleep...\n");

    const esp_err_t esp_result = esp_light_sleep_start();
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Failed to enter sleep mode: %s\n", __FILE__, __LINE__, esp_err_to_name(esp_result));
        return esp_result;
    }

    return ESP_OK;
}

esp_err_t
SleepController::ConfigureWakeup()
{
    gpio_wakeup_enable(static_cast<gpio_num_t>(pins::BUTTON), gpio_int_type_t::GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();
    esp_sleep_enable_timer_wakeup(GetSleepTimeUs());

    return ESP_OK;
}

uint64_t
SleepController::GetSleepTimeUs()
{
    const auto minute_difference = mSchedule.GetSchedulePoint().day_minute - mSchedule.GetSystemMinute();

    return minute_difference * 60 * 1'000 * 1'000; // microseconds
}