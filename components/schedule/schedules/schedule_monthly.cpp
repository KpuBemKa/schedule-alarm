#include "schedule_monthly.hpp"

namespace schd {

std::size_t
ScheduleMonthly::GetLocalMonthDay()
{

    const std::time_t raw_time = std::time(nullptr);
    const tm* const tl = localtime(&raw_time);
    return static_cast<std::size_t>(tl->tm_mday);
}

void
ScheduleMonthly::AdvanceSchedule()
{
    mSchedule.at(GetLocalMonthDay() - 1).AdvanceSchedule();
}

void
ScheduleMonthly::ReindexSchedule()
{
    mSchedule.at(GetLocalMonthDay() - 1).ReindexSchedule();
}

std::vector<uint8_t>
ScheduleMonthly::Serialize() const
{
    std::vector<uint8_t> result;
    for (const ScheduleDaily& schedule : mSchedule) {
        const auto serialized = schedule.Serialize();
        result.insert(result.end(), serialized.begin(), serialized.end());
    }

    result.shrink_to_fit();
    return result;
}

esp_err_t
ScheduleMonthly::Serialize(std::span<uint8_t> output) const
{
    return (ESP_ERR_NOT_FOUND);
}

std::expected<uint32_t, esp_err_t>
ScheduleMonthly::Deserialize(const std::span<const uint8_t> raw_data)
{
    uint32_t read_count = 0;
    std::vector<uint8_t> result;
    // std::size_t i = 0;
    for (ScheduleDaily& schedule : mSchedule) {
        // LOG_I("Day #%u", i);
        const std::expected<uint32_t, esp_err_t> result = schedule.Deserialize(raw_data.subspan(read_count));

        if (!result.has_value()) {
            LOG_E("%s:%d | Error deserializing monthly schedule at read_count = %lu: %s", __FILE__, __LINE__, read_count, esp_err_to_name(result.error()));
            return result.error();
        }

        read_count += result.value();
    }
    // LOG_I("---\n");

    ReindexSchedule();

    return read_count;
}

} // namespace schd
