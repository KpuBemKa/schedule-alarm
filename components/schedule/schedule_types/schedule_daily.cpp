#include "schedule_daily.hpp"

#include "memory_utils.hpp"

const char TAG[] = "SCHEDULE";

#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

namespace schd {

std::vector<uint8_t>
ScheduleDaily::Serialize() const
{
    std::vector<uint8_t> result;

    util::AppendAsBytes(result, static_cast<uint32_t>(mSchedule.size()));

    const uint8_t* schedule_list_ptr = reinterpret_cast<const uint8_t*>(mSchedule.data());
    result.insert(result.end(), schedule_list_ptr, schedule_list_ptr + mSchedule.size() * sizeof(mSchedule[0]));
    result.shrink_to_fit();

    return result;
}

esp_err_t
ScheduleDaily::Serialize(std::span<uint8_t> output) const
{
    return ESP_ERR_NOT_SUPPORTED;
}

std::expected<uint32_t, ScheduleError>
ScheduleDaily::Deserialize(const std::span<const uint8_t> raw_data)
{
    // check if there is enough data to read the amount of schedule items
    if (raw_data.size() < sizeof(uint32_t)) {
        return std::unexpected(ScheduleError::InvalidArgument);
    }

    std::size_t read_count = 0;

    const uint32_t schedule_items_count = util::ExtractType<uint32_t>(raw_data.subspan(read_count));
    read_count += sizeof(schedule_items_count);

    const std::size_t schedule_bytes = schedule_items_count * sizeof(mSchedule[0]);
    // check if there is amount of data to read the schedule items
    if (raw_data.size() - read_count < schedule_bytes) {
        return std::unexpected(ScheduleError::InvalidArgument);
    }

    // if the schedule is empty just leave it at that
    if (schedule_bytes == 0)
        return read_count;

    // copy the data directly
    mSchedule.clear();
    mSchedule.resize(schedule_items_count);
    std::copy_n(raw_data.data() + read_count, schedule_bytes, reinterpret_cast<uint8_t*>(mSchedule.data()));
    read_count += schedule_bytes;

    // verify the copied data
    const ScheduleError schd_errors = VerifySchedule();
    if (schd_errors != ScheduleError::OK)
        return std::unexpected(schd_errors);

    // reindex internal variables after updating the schedule
    ReindexSchedule();

    // add an item with the same action at the end of the loop to not worry in higher level schedules
    // about checking whever `mScheduleIndex` was overflown after AdvanceSchedule()
    const auto last_schedule_point = mSchedule.rend();
    if (last_schedule_point->time_offset != mLoopRange - 1) {
        mSchedule.push_back(
          SchedulePoint{ .time_offset = mLoopRange - 1, .action = last_schedule_point->action });
    }

    return read_count;
}

// std::size_t
// ScheduleDaily::GetLocalDaySecond()
// {
//     const std::time_t raw_time = std::time(nullptr);
//     const tm* const tl = std::localtime(&raw_time);
//     return static_cast<std::size_t>(tl->tm_hour * 3600 + tl->tm_min * 60 + tl->tm_sec);
// }

// void
// ScheduleDaily::AdvanceSchedule()
// {
//     if (mSchedule.size() == 0 && mSchedule.size() == 1)
//         return;

//     if (mSchedule.size() - 1 == mScheduleIndex)
//         mScheduleIndex = 0;

//     ++mScheduleIndex;
// }

// void
// ScheduleDaily::ReindexSchedule()
// {
//     const std::size_t current_time = GetLocalDaySecond();

//     if (mSchedule.size() == 0 && mSchedule.size() == 1) {
//         mScheduleIndex = 0;
//         return;
//     }

//     // iterate through schedule points until a higher activation time is found
//     // this will be the required schedule point
//     for (std::size_t i = 0; i < mSchedule.size(); ++i) {
//         if (mSchedule.at(i).day_second >= current_time) {
//             mScheduleIndex = i;
//             break;
//         }

//         // if at the last element
//         // and no schedule point with higher activation time has been found
//         if (mSchedule.size() - 1 == i)
//             mScheduleIndex = i;
//     }
// }
// ScheduleError
// ScheduleDaily::VerifySchedule() const
// {
//     // skip last item to avoid triggering out-of-bounds
//     for (std::size_t i = 0; i < mSchedule.size() - 1; ++i) {
//         const auto current = mSchedule.at(i);
//         const auto next = mSchedule.at(i + 1);

//         // check if the schedule is ordered by timeOffset
//         if (current.day_second > next.day_second)
//             return ScheduleError::Unordered;

//         // check for schedule points with the same day time
//         if (current.day_second == next.day_second)
//             return ScheduleError::Duplicate;
//     }

//     return ScheduleError::Ok;
// }

// std::vector<uint8_t>
// ScheduleDaily::Serialize() const
// {
//     const uint32_t schedule_bytes = static_cast<uint32_t>(mSchedule.size()) * sizeof(mSchedule[0]);

//     std::vector<uint8_t> result;
//     result.reserve(schedule_bytes + sizeof(uint32_t));

//     util::AppendAsBytes(result, mSchedule.size());

//     const uint8_t* schedule_list_ptr = reinterpret_cast<const uint8_t*>(mSchedule.data());
//     result.insert(result.end(), schedule_list_ptr, schedule_list_ptr + schedule_bytes);

//     return result;
// }

// esp_err_t
// ScheduleDaily::Serialize(std::span<uint8_t> output) const
// {
//     return -1;
// }

// std::expected<uint32_t, esp_err_t>
// ScheduleDaily::Deserialize(const std::span<const uint8_t> raw_data)
// {
//     const uint32_t schedule_items = *reinterpret_cast<const uint32_t*>(raw_data.data());
//     const uint32_t schedule_bytes = schedule_items * sizeof(SchedulePointDaily);

//     if (raw_data.size() - sizeof(uint32_t) < schedule_bytes) {
//         return std::unexpected(ESP_ERR_INVALID_ARG);
//     }

//     mSchedule.clear();
//     mSchedule.resize(schedule_items);
//     std::copy_n(raw_data.data() + sizeof(uint32_t), schedule_bytes, reinterpret_cast<uint8_t*>(mSchedule.data()));

//     ReindexSchedule();

//     return schedule_bytes + sizeof(uint32_t);
// }

} // namespace schd
