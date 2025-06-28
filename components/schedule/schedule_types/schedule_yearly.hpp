#pragma once

#include <array>

#include "schedule_monthly.hpp"

namespace schd {

class ScheduleYearly : public ISchedule
{
  public:
    constexpr virtual ScheduleType GetScheduleType() const override { return ScheduleType::Yearly; }

    bool IsEmpty() const override { return mSchedule.empty(); };
    std::size_t GetPointsCount() const override { return mSchedule.size(); };

    Action GetAction() const override { return mSchedule.at(GetMonthIndex()).GetAction(); }
    bool IsFired() const override { return mSchedule.at(GetMonthIndex()).IsFired(); }

    void AdvanceSchedule() override;
    void ReindexSchedule() override;

    std::vector<uint8_t> Serialize() const override;
    esp_err_t Serialize(std::span<uint8_t> output) const override;
    std::expected<uint32_t, esp_err_t> Deserialize(const std::span<const uint8_t> raw_data) override;

  private:
    static std::size_t GetMonthIndex();

  private:
    std::array<ScheduleMonthly, 12> mSchedule;
};

} // namespace schd
