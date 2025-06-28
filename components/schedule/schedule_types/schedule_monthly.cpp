#include "schedule_monthly.hpp"

#include "config.hpp"

const char TAG[] = "SCHEDULE";

#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

namespace schd {

void
ScheduleMonthly::AdvanceSchedule()
{
    static std::size_t current_day = GetMonthDayIndex();

    if (current_day == mDayIndex) {
        mDaySchedule.AdvanceSchedule();
    }
}

void
ScheduleMonthly::ReindexSchedule()
{
    mSchedule.at(GetMonthDayIndex()).ReindexSchedule();
}

ScheduleError
ScheduleMonthly::VerifySchedule() const
{
    for (const auto& schedule_item : mSchedule) {
        const auto schd_error = schedule_item.VerifySchedule();

        if (schd_error != ScheduleError::OK)
            return schd_error;
    }

    return ScheduleError::OK;
}

std::size_t
ScheduleMonthly::GetMonthDayIndex()
{
    const std::time_t raw_time = std::time(nullptr);
    const tm* const tl = localtime(&raw_time);
    return static_cast<std::size_t>(tl->tm_mday - 1);
}

esp_err_t
ScheduleMonthly::ReadDayScheduleFromStorage(const std::size_t day_index)
{
    FILE* file_ptr = fopen(config::SCHEDULE_FILEPATH.data(), "rb");
    if (file_ptr == nullptr) {
        LOG_E("%s:%d | Failed to open the schedule file.", __FILE__, __LINE__);
        return ESP_ERR_NOT_FOUND;
    }

    
}

// std::vector<uint8_t>
// ScheduleMonthly::Serialize() const
// {
//     std::vector<uint8_t> result;
//     for (const ScheduleDaily& schedule : mSchedule) {
//         const auto serialized = schedule.Serialize();
//         result.insert(result.end(), serialized.begin(), serialized.end());
//     }

//     result.shrink_to_fit();
//     return result;
// }

// esp_err_t
// ScheduleMonthly::Serialize(std::span<uint8_t> output) const
// {
//     return (ESP_ERR_NOT_FOUND);
// }

// std::expected<uint32_t, ScheduleError>
// ScheduleMonthly::Deserialize(const std::span<const uint8_t> raw_data)
// {
//     uint32_t read_count = 0;
//     std::vector<uint8_t> result;
//     // std::size_t i = 0;
//     for (ScheduleDaily& schedule : mSchedule) {
//         // LOG_I("Day #%u", i);
//         const std::expected<uint32_t, esp_err_t> result = schedule.Deserialize(raw_data.subspan(read_count));

//         if (!result.has_value()) {
//             LOG_E("%s:%d | Error deserializing monthly schedule at read_count = %lu: %s",
//                   __FILE__,
//                   __LINE__,
//                   read_count,
//                   esp_err_to_name(result.error()));
//             return result.error();
//         }

//         read_count += result.value();
//     }
//     // LOG_I("---\n");

//     ReindexSchedule();

//     return read_count;
// }

} // namespace schd
