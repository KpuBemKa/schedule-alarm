#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "http_controller.hpp"
#include "schedule.hpp"
#include "wifi_controller.hpp"

class SleepController {
   public:
    SleepController(wifi::WifiController& wifi_controller,
                    srvr::HttpController& http_controller,
                    Schedule& schedule)
        : m_wifi_controller(wifi_controller),
          m_http_controller(http_controller),
          mSchedule(schedule) {}

    void SetSleepTimeout(const TickType_t timeout_ticks) {
        m_timeout_ticks = timeout_ticks;
    }
    void ResetSleepTimeout() {
        m_sleep_stamp = xTaskGetTickCount() + m_timeout_ticks;
    }

    void TaskRunner(void* args) {
        while (true) {
            if (xTaskGetTickCount() == m_sleep_stamp) {
                EnterSleep();

                ResetSleepTimeout();
            }
        }
    }

   private:
    esp_err_t PrepareForSleep();
    esp_err_t ConfigureWakeup();
    esp_err_t EnterSleep();

    uint64_t GetSleepTimeUs();

   private:
    TickType_t m_sleep_stamp;  // Enter sleep when at this point
    TickType_t m_timeout_ticks;

    wifi::WifiController& m_wifi_controller;
    srvr::HttpController& m_http_controller;
    Schedule& mSchedule;
};