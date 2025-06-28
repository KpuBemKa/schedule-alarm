#pragma once

#include <array>

#include "schedule_daily.hpp"

namespace schd {

class 

// class ScheduleMonthly : public ISchedule
// {
//   public:
//     constexpr ScheduleType GetScheduleType() const override { return ScheduleType::Monthly; }

//     bool IsEmpty() const override { return mDaySchedule.IsEmpty(); };
//     std::size_t GetPointsCount() const override { return mDaySchedule.GetPointsCount(); };

//     Action GetAction() const override { return mDaySchedule.GetAction(); }
//     bool IsFired() const override { return mDaySchedule.IsFired(); }

//     void AdvanceSchedule() override;
//     void ReindexSchedule() override;
//     ScheduleError VerifySchedule() const override;

//     // std::vector<uint8_t> Serialize() const override;
//     // esp_err_t Serialize(std::span<uint8_t> output) const override;
//     // std::expected<uint32_t, ScheduleError> Deserialize(const std::span<const uint8_t> raw_data) override;

//   private:
//     static std::size_t GetMonthDayIndex();
    
//     esp_err_t ReadDayScheduleFromStorage(const std::size_t day_index);

//   private:
//     std::size_t mDayIndex;
//     ScheduleDaily mDaySchedule;
// };



} // namespace schd
