#pragma once

#include <ctime>
#include <memory>

#include "esp_log.h"

#include "i_schedule.hpp"

namespace schd {

const char TAG[] = "SCHEDULE";

#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

struct SchedulePointDaily
{
    std::size_t day_second;
    Action fire_action;
};

class ScheduleDaily : public ISchedule
{
  public:
    ScheduleDaily(std::vector<SchedulePointDaily> input) { mSchedule = input; }

    bool IsEmpty() const override { return mSchedule.empty(); };
    std::size_t GetPointsCount() const override { return mSchedule.size(); };

    Action GetAction() const override { return mSchedule.at(mScheduleIndex).fire_action; }
    bool IsFired() const override
    {
        // check if current time is past the firing point
        return GetLocalDaySecond() >= mSchedule.at(mScheduleIndex).day_second;
    }

    void AdvanceSchedule() override {}
    void ReindexSchedule() override {}

    std::vector<uint8_t> Serialize() const override;
    esp_err_t Serialize(std::span<uint8_t> output) const override;
    esp_err_t Deserialize(const std::span<const uint8_t> raw_data) override;

    SchedulePointDaily GetPoint(const std::size_t index) const { return mSchedule.at(index); }

  private:
    static std::size_t GetLocalDaySecond();

  private:
    std::size_t mScheduleIndex = 0;
    std::vector<SchedulePointDaily> mSchedule;
};

std::size_t
ScheduleDaily::GetLocalDaySecond()
{
    const std::time_t raw_time = std::time(nullptr);
    const tm* const tl = std::localtime(&raw_time);
    return static_cast<std::size_t>(tl->tm_hour * 3600 + tl->tm_min * 60 + tl->tm_sec);
}

std::vector<uint8_t>
ScheduleDaily::Serialize() const
{
    const std::size_t schedule_bytes = mSchedule.size() * sizeof(mSchedule[0]);

    std::vector<uint8_t> result;
    result.reserve(schedule_bytes + sizeof(std::size_t));

    const uint32_t items_count = mSchedule.size();
    result.insert(result.end(),
                  reinterpret_cast<const uint8_t*>(&items_count),
                  reinterpret_cast<const uint8_t*>(&items_count) + sizeof(items_count));

    const uint8_t* schedule_list_ptr = reinterpret_cast<const uint8_t*>(mSchedule.data());
    result.insert(result.end(), schedule_list_ptr, schedule_list_ptr + schedule_bytes);

    return result;
}

esp_err_t
ScheduleDaily::Serialize(std::span<uint8_t> output) const
{
    return -1;
}

esp_err_t
ScheduleDaily::Deserialize(const std::span<const uint8_t> raw_data)
{
    const std::size_t schedule_items = *reinterpret_cast<const std::size_t*>(raw_data.data());
    const std::size_t schedule_bytes = schedule_items * sizeof(SchedulePointDaily);

    LOG_I("schedule_bytes: %d", schedule_bytes);

    if (raw_data.size() - sizeof(std::size_t) != schedule_bytes) {
        return ESP_ERR_INVALID_ARG;
    }

    mSchedule.clear();
    mSchedule.resize(schedule_items);

    std::copy_n(raw_data.data() + sizeof(std::size_t), schedule_bytes, reinterpret_cast<uint8_t*>(mSchedule.data()));

    ReindexSchedule();

    return ESP_OK;
}

} // namespace schd
