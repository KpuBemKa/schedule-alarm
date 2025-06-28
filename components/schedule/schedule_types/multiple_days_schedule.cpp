#include "multiple_days_schedule.hpp"

#include <cstring>

#include "esp_log.h"

#include "config.hpp"

const char TAG[] = "SCHEDULE";

#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

namespace schd {

void
MultipleDaysSchedule::AdvanceSchedule()
{
    const std::size_t current_day = GetDayIndex();

    if (current_day == mDayIndex)
        mDaySchedule.AdvanceSchedule();
    else
        LoadDayScheduleFromStorage(current_day);
}

void
MultipleDaysSchedule::ReindexSchedule()
{
    const std::size_t current_day = GetDayIndex();

    if (current_day == mDayIndex)
        mDaySchedule.ReindexSchedule();
    else
        LoadDayScheduleFromStorage(current_day);
}

ScheduleError
MultipleDaysSchedule::VerifySchedule() const
{
    // open the schedule file
    FILE* file_ptr = fopen(config::SCHEDULE_FILEPATH.data(), "rb");
    if (file_ptr == nullptr) {
        LOG_E("%s:%d | Failed to open the schedule file.", __FILE__, __LINE__);
        return ScheduleError::Error;
    }

    const ScheduleDaily schedule_verifier;

    // go through each day
    for (std::size_t i = 0; i < kMaxDays; ++i) {
        uint32_t schd_items_count = 0;
        std::size_t read_count = std::fread(&schd_items_count, sizeof(schd_items_count), 1, file_ptr);
        if (read_count != 1) {
            LOG_E("%s:%d | fread() returned an unexpected value: %d", __FILE__, __LINE__, read_count);
            fclose(file_ptr);
            return ScheduleError::Error;
        }

        // prepare the buffer
        std::vector<SchedulePoint> buffer(schd_items_count);

        // read & check
        std::size_t read_count = std::fread(buffer.data(), sizeof(buffer[0]), buffer.size(), file_ptr);
        if (read_count != schd_items_count) {
            LOG_E("%s:%d | fread() returned an unexpected value: %d", __FILE__, __LINE__, read_count);
            fclose(file_ptr);
            return ScheduleError::Error;
        }

        const ScheduleError schedule_errors = schedule_verifier.VerifyOtherSchedule(buffer);
        if (schedule_errors == ScheduleError::OK) {
            fclose(file_ptr);
            return schedule_errors;
        }
    }

    // no need for the file anymore
    fclose(file_ptr);
    return ScheduleError::OK;
}

std::size_t
MultipleDaysSchedule::GetDayIndex()
{
    const std::time_t raw_time = std::time(nullptr);
    const tm* const tl = localtime(&raw_time);
    return static_cast<std::size_t>(tl->tm_mday - 1);
}

esp_err_t
MultipleDaysSchedule::LoadDayScheduleFromStorage(const std::size_t day_index)
{
    if (day_index > kMaxDays)
        return ESP_ERR_INVALID_ARG;

    // open the schedule file
    FILE* file_ptr = fopen(config::SCHEDULE_FILEPATH.data(), "rb");
    if (file_ptr == nullptr) {
        LOG_E("%s:%d | Failed to open the schedule file.", __FILE__, __LINE__);
        return ESP_ERR_NOT_FOUND;
    }

    // advance to the requried day
    for (std::size_t i = 0; i < day_index; ++i) {
        uint32_t schd_items_count = 0;

        // read & check this schedule size
        const std::size_t read_count = std::fread(&schd_items_count, sizeof(schd_items_count), 1, file_ptr);
        if (read_count != 1) {
            LOG_E("%s:%d | fread() returned an unexpected value: %d", __FILE__, __LINE__, read_count);
            fclose(file_ptr);
            return ESP_ERR_INVALID_SIZE;
        }

        // seek to the next schedule
        if (fseek(file_ptr, schd_items_count * sizeof(SchedulePoint), SEEK_CUR) != 0) {
            LOG_E("%s:%d | fseek() error: %s", __FILE__, __LINE__, strerror(errno));
            fclose(file_ptr);
            return ESP_ERR_INVALID_STATE;
        }
    }

    // read & check again
    uint32_t schd_items_count = 0;
    std::size_t read_count = std::fread(&schd_items_count, sizeof(schd_items_count), 1, file_ptr);
    if (read_count != 1) {
        LOG_E("%s:%d | fread() returned an unexpected value: %d", __FILE__, __LINE__, read_count);
        fclose(file_ptr);
        return ESP_ERR_INVALID_SIZE;
    }

    // prepare the buffer
    const std::size_t schedule_bytes = schd_items_count * sizeof(SchedulePoint);
    std::vector<uint8_t> buffer(schedule_bytes);

    // read & check
    std::size_t read_count = std::fread(buffer.data(), sizeof(uint8_t), schedule_bytes, file_ptr);
    if (read_count != schedule_bytes) {
        LOG_E("%s:%d | fread() returned an unexpected value: %d", __FILE__, __LINE__, read_count);
        fclose(file_ptr);
        return ESP_ERR_INVALID_SIZE;
    }

    // no need for the file anymore
    fclose(file_ptr);

    // deserialize this needed schedule
    const std::expected<uint32_t, schd::ScheduleError> result = mDaySchedule.Deserialize(buffer);
    if (!result.has_value()) {
        LOG_E("%s:%d | DaySchedule::Deserialize() returned an unexpected value: %d",
              __FILE__,
              __LINE__,
              static_cast<int>(result.error()));
        return ESP_FAIL;
    }

    return ESP_OK;
}

} // namespace schd
