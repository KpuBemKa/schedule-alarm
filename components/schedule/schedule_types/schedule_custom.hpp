#pragma once

#include <span>
#include <vector>

#include "i_schedule.hpp"

namespace schd {

#pragma pack(1)
struct SchedulePoint
{
    uint64_t time_offset;
    Action action;
};

class ScheduleCustom : public ISchedule
{
  public:
    constexpr virtual ScheduleType GetScheduleType() const override { return ScheduleType::Custom; }

    bool IsEmpty() const override { return mSchedule.empty(); };
    std::size_t GetPointsCount() const override { return mSchedule.size(); };

    Action GetAction() const override { return mSchedule.at(mScheduleIndex).action; }
    bool IsFired() const override { return mSchedule.at(mScheduleIndex).time_offset >= GetStartTimeOffset(); }

    ScheduleError SetSchedule(const std::vector<SchedulePoint>& source_schedule);

    void AdvanceSchedule() override;
    void ReindexSchedule() override;
    ScheduleError VerifySchedule() const override;
    ScheduleError VerifyOtherSchedule(const std::span<const SchedulePoint> verifyable) const;

    std::vector<uint8_t> Serialize() const override;
    esp_err_t Serialize(std::span<uint8_t> output) const override;
    std::expected<uint32_t, ScheduleError> Deserialize(const std::span<const uint8_t> raw_data) override;

  protected:
    uint64_t GetTimestamp() const;
    uint64_t GetStartTimeOffset() const;

  protected:
    uint64_t mStartTime;
    uint64_t mLoopRange;
    std::vector<SchedulePoint> mSchedule;
    std::size_t mScheduleIndex = 0;
};

} // namespace schd
