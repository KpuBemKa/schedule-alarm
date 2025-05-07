#include "schedule_daily.hpp"

#include "memory_utils.hpp"

const char TAG[] = "SCHEDULE";

#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

namespace schd {

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
    const uint32_t schedule_bytes = static_cast<uint32_t>(mSchedule.size()) * sizeof(mSchedule[0]);

    std::vector<uint8_t> result;
    result.reserve(schedule_bytes + sizeof(uint32_t));

    util::AppendAsBytes(result, mSchedule.size());

    const uint8_t* schedule_list_ptr = reinterpret_cast<const uint8_t*>(mSchedule.data());
    result.insert(result.end(), schedule_list_ptr, schedule_list_ptr + schedule_bytes);

    return result;
}

esp_err_t
ScheduleDaily::Serialize(std::span<uint8_t> output) const
{
    return -1;
}

std::expected<uint32_t, esp_err_t>
ScheduleDaily::Deserialize(const std::span<const uint8_t> raw_data)
{
    const uint32_t schedule_items = *reinterpret_cast<const uint32_t*>(raw_data.data());
    const uint32_t schedule_bytes = schedule_items * sizeof(SchedulePointDaily);

    // LOG_I("Schedule items: %lu", schedule_items);

    if (raw_data.size() - sizeof(uint32_t) < schedule_bytes) {
        return std::unexpected(ESP_ERR_INVALID_ARG);
    }

    mSchedule.clear();
    mSchedule.resize(schedule_items);
    std::copy_n(raw_data.data() + sizeof(uint32_t), schedule_bytes, reinterpret_cast<uint8_t*>(mSchedule.data()));

    // for (const auto& item : mSchedule) {
    //     LOG_I("Day second: %lu | Action: %lu", item.day_second, static_cast<uint32_t>(item.fire_action));
    // }

    ReindexSchedule();

    return schedule_bytes + sizeof(uint32_t);
}

} // namespace schd
