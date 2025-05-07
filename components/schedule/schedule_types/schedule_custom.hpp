#pragma once

#include <vector>

#include "i_schedule.hpp"

namespace schd {

#pragma pack(1)
struct SchedulePointCustom
{
    uint64_t fireTime;
    Action fireAction;
};

class ScheduleCustom : public ISchedule
{
    bool IsEmpty() const override { return mSchedule.empty(); };
    std::size_t GetPointsCount() const override { return mSchedule.size(); };

    Action GetAction() const override { return mSchedule.at(mScheduleIndex).fireAction; }
    bool IsFired() const override { return mSchedule.at(mScheduleIndex).fireTime >= GetStartTimeOffset(); }

    void AdvanceSchedule() override;
    void ReindexSchedule() override;

    std::vector<uint8_t> Serialize() const override;
    esp_err_t Serialize(std::span<uint8_t> output) const override;
    std::expected<uint32_t, esp_err_t> Deserialize(const std::span<const uint8_t> raw_data) override;

  private:
    uint64_t GetTimestamp() const;
    uint64_t GetStartTimeOffset() const;

  private:
    uint64_t mStartTime;
    uint64_t mLoopRange;
    std::vector<SchedulePointCustom> mSchedule;
    std::size_t mScheduleIndex = 0;
};

} // namespace schd
