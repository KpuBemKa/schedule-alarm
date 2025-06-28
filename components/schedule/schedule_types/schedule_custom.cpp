#include "schedule_custom.hpp"

#include <algorithm>
#include <ctime>

#include "memory_utils.hpp"

const char TAG[] = "SCHEDULE";

#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

namespace schd {

ScheduleError
ScheduleCustom::SetSchedule(const std::vector<SchedulePoint>& source_schedule)
{
    const ScheduleError errors = VerifyOtherSchedule(source_schedule);
}

void
ScheduleCustom::AdvanceSchedule()
{
    if (mSchedule.size() == 0 && mSchedule.size() == 1)
        return;

    if (mSchedule.size() - 1 == mScheduleIndex)
        mScheduleIndex = 0;

    ++mScheduleIndex;
}

void
ScheduleCustom::ReindexSchedule()
{
    if (mSchedule.size() == 0 && mSchedule.size() == 1) {
        mScheduleIndex = 0;
        return;
    }

    const uint64_t current_time_offset = GetStartTimeOffset();
    // iterate through schedule points until a higher activation time is found
    // this will be the required schedule point
    for (std::size_t i = 0; i < mSchedule.size(); ++i) {
        if (mSchedule.at(i).time_offset >= current_time_offset) {
            mScheduleIndex = i;
            break;
        }

        // if at the last element
        // and no schedule point with higher activation time has been found
        if (mSchedule.size() - 1 == i)
            mScheduleIndex = i;
    }
}

ScheduleError
ScheduleCustom::VerifySchedule() const
{
    return VerifyOtherSchedule(mSchedule);
}

ScheduleError
ScheduleCustom::VerifyOtherSchedule(const std::span<const SchedulePoint> verifyable) const
{
    if (mLoopRange <= 0)
        return ScheduleError::InvalidLoopRange;

    // skip last item to avoid triggering out-of-bounds
    for (std::size_t i = 0; i < verifyable.size() - 1; ++i) {
        const auto current = verifyable[i];
        const auto next = verifyable[i + 1];

        // check if the schedule is ordered by timeOffset
        if (current.time_offset > next.time_offset)
            return ScheduleError::Unordered;

        // check for schedule points with the same day time
        if (current.time_offset == next.time_offset)
            return ScheduleError::Duplicate;

        // check for overflow beyond to loop end
        if (current.time_offset >= mLoopRange)
            return ScheduleError::LoopOverflow;
    }

    // check the last item here
    if (verifyable.size() > 0) {
        // check for overflow
        if (verifyable[verifyable.size() - 1].time_offset > mLoopRange)
            return ScheduleError::LoopOverflow;
    }

    return ScheduleError::OK;
}

std::vector<uint8_t>
ScheduleCustom::Serialize() const
{
    std::vector<uint8_t> result;

    util::AppendAsBytes(result, mStartTime);
    util::AppendAsBytes(result, mLoopRange);
    util::AppendAsBytes(result, static_cast<uint32_t>(mSchedule.size()));

    const uint8_t* schedule_list_ptr = reinterpret_cast<const uint8_t*>(mSchedule.data());
    result.insert(result.end(), schedule_list_ptr, schedule_list_ptr + mSchedule.size() * sizeof(mSchedule[0]));

    result.shrink_to_fit();
    return result;
}

esp_err_t
ScheduleCustom::Serialize(std::span<uint8_t> output) const
{
    return ESP_ERR_NOT_SUPPORTED;
}

std::expected<uint32_t, ScheduleError>
ScheduleCustom::Deserialize(const std::span<const uint8_t> raw_data)
{
    if (raw_data.size() < sizeof(mStartTime) + sizeof(mLoopRange) + sizeof(::size_t)) {
        return std::unexpected(ScheduleError::InvalidArgument);
    }

    std::size_t read_count = 0;

    const std::size_t schedule_items = util::ExtractType<std::size_t>(raw_data.subspan(read_count));
    read_count += sizeof(schedule_items);

    mStartTime = util::ExtractType<uint64_t>(raw_data);
    read_count += sizeof(mStartTime);

    mLoopRange = util::ExtractType<uint64_t>(raw_data.subspan(read_count));
    read_count += sizeof(mLoopRange);

    const std::size_t schedule_bytes = schedule_items * sizeof(mSchedule[0]);
    if (raw_data.size() - read_count < schedule_bytes) {
        return std::unexpected(ScheduleError::InvalidArgument);
    }

    // if the schedule is empty just leave it at that
    if (schedule_bytes == 0)
        return read_count;

    // copy the data directly
    mSchedule.clear();
    mSchedule.resize(schedule_items);
    std::copy_n(raw_data.data() + read_count, schedule_bytes, reinterpret_cast<uint8_t*>(mSchedule.data()));
    read_count += schedule_bytes;

    // reindex internal variables after updating the schedule
    ReindexSchedule();

    // add an item with the same action at the end of the loop to not worry in higher level schedules
    // about checking whever `mScheduleIndex` was overflown after AdvanceSchedule()
    const auto last_schedule_point = mSchedule.rend();
    if (last_schedule_point->time_offset != mLoopRange - 1) {
        mSchedule.push_back(SchedulePoint{ .time_offset = mLoopRange - 1, .action = last_schedule_point->action });
    }

    return read_count;
}

uint64_t
ScheduleCustom::GetTimestamp() const
{
    return static_cast<uint64_t>(std::time(nullptr));
}

uint64_t
ScheduleCustom::GetStartTimeOffset() const
{
    return (GetTimestamp() - mStartTime) % mLoopRange;
}

} // namespace schd
