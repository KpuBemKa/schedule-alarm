#pragma once

#include <memory>
#include <string_view>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "schedules/i_schedule.hpp"

namespace schd {

class Schedule
{
  public:
    Schedule()
    {
        xScheduleMutex = xSemaphoreCreateBinary();
        xSemaphoreGive(xScheduleMutex);
    }

    bool IsEmpty() const;

    bool IsFired(const bool auto_advance_schedule = true);
    Action GetAction() const;

    void AdvanceSchedule();
    void ReindexSchedule();

    esp_err_t Load() { return ESP_ERR_NOT_SUPPORTED; }
    esp_err_t Save() { return ESP_ERR_NOT_SUPPORTED; }

    std::expected<std::vector<uint8_t>, esp_err_t> Serialize() const;
    esp_err_t Serialize(std::span<uint8_t> output) const;
    esp_err_t Deserialize(const std::span<const uint8_t> source) const;

  private:
    static constexpr const std::string_view SCHEDULE_FILE = "system_schedule.json";

    mutable QueueHandle_t xScheduleMutex;
    std::unique_ptr<ISchedule> mSchedule;
};

} // namespace schd
