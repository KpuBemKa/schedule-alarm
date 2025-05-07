#pragma once

#include <ctime>
#include <memory>

#include "esp_log.h"

#include "i_schedule.hpp"

namespace schd {

#pragma pack(1)
struct SchedulePointDaily
{
    uint32_t day_second;
    Action fire_action;
};

class ScheduleDaily : public ISchedule
{
  public:
    constexpr ScheduleType GetScheduleType() const override { return ScheduleType::Daily; }

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
    std::expected<uint32_t, esp_err_t> Deserialize(const std::span<const uint8_t> raw_data) override;

    SchedulePointDaily GetPoint(const std::size_t index) const { return mSchedule.at(index); }

  private:
    static std::size_t GetLocalDaySecond();

  private:
    std::size_t mScheduleIndex = 0;
    std::vector<SchedulePointDaily> mSchedule;
};

} // namespace schd
