#pragma once

#include <ctime>
#include <memory>

#include "esp_log.h"

#include "schedule_custom.hpp"

namespace schd {

class ScheduleDaily : public ScheduleCustom
{
  public:
    ScheduleDaily()
    {
        mStartTime = 0;
        mLoopRange = 86400;
    }

    // constexpr ScheduleType GetScheduleType() const override { return ScheduleType::Daily; }

    // bool IsEmpty() const override { return mSchedule.IsEmpty(); };
    // std::size_t GetPointsCount() const override { return mSchedule.GetPointsCount(); };

    // Action GetAction() const override { return mSchedule.GetAction(); }
    // bool IsFired() const override { return mSchedule.IsFired(); }

    // void AdvanceSchedule() override { mSchedule.AdvanceSchedule(); }
    // void ReindexSchedule() override { mSchedule.ReindexSchedule(); }
    // ScheduleError VerifySchedule() const override { return mSchedule.VerifySchedule(); }

    std::vector<uint8_t> Serialize() const override;
    esp_err_t Serialize(std::span<uint8_t> output) const override;
    std::expected<uint32_t, ScheduleError> Deserialize(const std::span<const uint8_t> raw_data) override;
};

// #pragma pack(1)
// struct SchedulePointDaily
// {
//     uint32_t day_second;
//     Action fire_action;
// };

// class ScheduleDaily : public ISchedule
// {
//   public:
//     constexpr ScheduleType GetScheduleType() const override { return ScheduleType::Daily; }

//     bool IsEmpty() const override { return mSchedule.empty(); };
//     std::size_t GetPointsCount() const override { return mSchedule.size(); };

//     Action GetAction() const override { return !IsEmpty() ? mSchedule.at(mScheduleIndex).fire_action : Action::None;
//     } bool IsFired() const override
//     {
//         // check if current time is past the firing point
//         return !IsEmpty() ? GetLocalDaySecond() >= mSchedule.at(mScheduleIndex).day_second : false;
//     }

//     void AdvanceSchedule() override;
//     void ReindexSchedule() override;
//     ScheduleError VerifySchedule() const override;

//     std::vector<uint8_t> Serialize() const override;
//     esp_err_t Serialize(std::span<uint8_t> output) const override;
//     std::expected<uint32_t, esp_err_t> Deserialize(const std::span<const uint8_t> raw_data) override;

//     SchedulePointDaily GetPoint(const std::size_t index) const { return mSchedule.at(index); }

//   private:
//     static std::size_t GetLocalDaySecond();

//   private:
//     std::size_t mScheduleIndex = 0;
//     std::vector<SchedulePointDaily> mSchedule;
// };

} // namespace schd
