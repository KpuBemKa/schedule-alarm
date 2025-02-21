#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

enum class AlarmType : uint16_t { None = 0, Single = 1, Double = 2, Triple = 3 };

struct SchedulePoint {
    // Represents a minute of the day when the alarm should fire
    uint16_t day_minute;
    AlarmType alarm_type;
};

class Schedule {
   public:
    Schedule() {
        xScheduleMutex = xSemaphoreCreateBinary();
        xSemaphoreGive(xScheduleMutex);
    }

    esp_err_t Load();
    esp_err_t Save();

    void SetIndex(const std::size_t new_index) { mScheduleIndex = new_index; }

    esp_err_t SetSchedule(const std::span<const SchedulePoint> schedule);
    std::vector<SchedulePoint> GetScheduleArray();

    SchedulePoint GetSchedulePoint();
    uint16_t GetSystemMinute();

    void AdvanceSchedule();
    void ReindexSchedule();

    std::string ToJson();
    esp_err_t FromJson(const std::string_view raw_json);

   private:
    static constexpr const std::string_view SCHEDULE_FILE = "alarm_schedule.json";

    mutable QueueHandle_t xScheduleMutex;
    std::size_t mScheduleIndex = 0;
    std::vector<SchedulePoint> mSchedule = {};
};