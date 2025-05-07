#include "schedule_custom.hpp"

#include <algorithm>
#include <ctime>

#include "memory_utils.hpp"

const char TAG[] = "SCHEDULE";

#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

namespace schd {

std::vector<uint8_t>
ScheduleCustom::Serialize() const
{
    std::vector<uint8_t> result;

    util::AppendAsBytes(result, mStartTime);
    util::AppendAsBytes(result, mLoopRange);
    util::AppendAsBytes(result, mSchedule.size());

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

std::expected<uint32_t, esp_err_t>
ScheduleCustom::Deserialize(const std::span<const uint8_t> raw_data)
{

    if (raw_data.size() < sizeof(mStartTime) + sizeof(mLoopRange) + sizeof(::size_t)) {
        return std::unexpected(ESP_ERR_INVALID_ARG);
    }

    std::size_t read_count = 0;

    mStartTime = util::ExtractType<uint64_t>(raw_data);
    read_count += sizeof(mStartTime);

    mLoopRange = util::ExtractType<uint64_t>(raw_data.subspan(read_count));
    read_count += sizeof(mLoopRange);

    const std::size_t schedule_items = util::ExtractType<std::size_t>(raw_data.subspan(read_count));
    read_count += sizeof(schedule_items);

    const std::size_t schedule_bytes = schedule_items * sizeof(mSchedule[0]);
    if (raw_data.size() - read_count < schedule_bytes) {
        return std::unexpected(ESP_ERR_INVALID_ARG);
    }

    mSchedule.clear();
    mSchedule.resize(schedule_items);
    std::copy_n(raw_data.data() + read_count, schedule_bytes, reinterpret_cast<uint8_t*>(mSchedule.data()));

    ReindexSchedule();

    return schedule_bytes + sizeof(uint32_t);
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
