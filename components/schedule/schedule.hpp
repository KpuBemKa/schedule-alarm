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

    esp_err_t Load();
    esp_err_t Save();

    std::expected<std::vector<uint8_t>, esp_err_t> Serialize() const;
    esp_err_t Serialize(std::span<uint8_t> output) const;
    esp_err_t Deserialize(const std::span<const uint8_t> raw_data);

  private:
    static constexpr const std::string_view SCHEDULE_FILE = "schedule.bin";

    mutable QueueHandle_t xScheduleMutex;
    std::unique_ptr<ISchedule> mSchedule;
};

} // namespace schd
