#include "schedule_yearly.hpp"

const char TAG[] = "SCHEDULE";

#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

namespace schd {

void
ScheduleYearly::AdvanceSchedule()
{
    static std::size_t last_month = GetMonthIndex();

    if (last_month != GetMonthIndex()) {
        last_month = GetMonthIndex();
        mSchedule.at(last_month).ReindexSchedule();
        return;
    }

    mSchedule.at(last_month).AdvanceSchedule();
}

void
ScheduleYearly::ReindexSchedule()
{
    mSchedule.at(GetMonthIndex()).ReindexSchedule();
}

std::vector<uint8_t>
ScheduleYearly::Serialize() const
{
    std::vector<uint8_t> result;
    for (const ScheduleMonthly& monthly : mSchedule) {
        const auto serialized = monthly.Serialize();
        result.insert(result.end(), serialized.begin(), serialized.end());
    }

    result.shrink_to_fit();
    return result;
}

esp_err_t
ScheduleYearly::Serialize(std::span<uint8_t> output) const
{
    return (ESP_ERR_NOT_FOUND);
}

std::expected<uint32_t, esp_err_t>
ScheduleYearly::Deserialize(const std::span<const uint8_t> raw_data)
{
    uint32_t read_count = 0;
    std::vector<uint8_t> result;

    for (ScheduleMonthly& monthly : mSchedule) {
        const std::expected<uint32_t, esp_err_t> result = monthly.Deserialize(raw_data.subspan(read_count));

        if (!result.has_value()) {
            LOG_E("%s:%d | Error deserializing yearly schedule at read_count = %lu: %s",
                  __FILE__,
                  __LINE__,
                  read_count,
                  esp_err_to_name(result.error()));
            return result.error();
        }

        read_count += result.value();
    }

    ReindexSchedule();

    return read_count;
}

std::size_t
ScheduleYearly::GetMonthIndex()
{
    const std::time_t raw_time = std::time(nullptr);
    const tm* const tl = localtime(&raw_time);
    return static_cast<std::size_t>(tl->tm_mday);
}

} // namespace schd
