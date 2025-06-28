#pragma once

#include <array>

#include "schedule_daily.hpp"

namespace schd {

class MultipleDaysSchedule : public ISchedule
{
  public:
    MultipleDaysSchedule(const std::size_t max_days)
      : kMaxDays(max_days)
    {
    }

    constexpr ScheduleType GetScheduleType() const override { return ScheduleType::Monthly; }

    bool IsEmpty() const override { return mDaySchedule.IsEmpty(); };
    std::size_t GetPointsCount() const override { return mDaySchedule.GetPointsCount(); };

    Action GetAction() const override { return mDaySchedule.GetAction(); }
    bool IsFired() const override { return mDaySchedule.IsFired(); }

    void AdvanceSchedule() override;
    void ReindexSchedule() override;
    ScheduleError VerifySchedule() const override;

  private:
    static std::size_t GetDayIndex();

    esp_err_t LoadDayScheduleFromStorage(const std::size_t day_index);

  private:
    const std::size_t kMaxDays;

    std::size_t mDayIndex;
    ScheduleDaily mDaySchedule;
};

} // namespace schd
