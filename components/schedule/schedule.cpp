#include "schedule.hpp"

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <string>

#include "cJSON.h"
#include "config.hpp"
#include "esp_log.h"

const char TAG[] = "SCHEDULE";

#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

esp_err_t Schedule::Load() {
    std::string file_path(SPIFFS_BASE_PATH);
    file_path.append("/").append(SCHEDULE_FILE);

    FILE* file_ptr = fopen(file_path.c_str(), "r");
    if (file_ptr == nullptr) {
        LOG_E("%s:%d | Failed to open the schedule file.", __FILE__, __LINE__);
        return ESP_ERR_NOT_FOUND;
    }

    // Retreive the file size
    fseek(file_ptr, 0L, SEEK_END);
    std::size_t file_size = ftell(file_ptr);
    fseek(file_ptr, 0L, SEEK_SET);

    // Reserve memory for the file
    std::vector<char> file_buf(file_size, '\0');

    // Copy file data into the reserved memory
    const std::size_t read_count = fread(file_buf.data(), sizeof(char), file_size, file_ptr);
    if (read_count != file_size) {
        LOG_E("%s:%d | Error reading the schedule file. Ammount of bytes read: %u", __FILE__,
              __LINE__, read_count);
        return ESP_ERR_NOT_FINISHED;
    }

    return FromJson(std::string_view(file_buf.data(), read_count));
}

esp_err_t Schedule::Save() {
    std::string file_path(SPIFFS_BASE_PATH);
    file_path.append("/").append(SCHEDULE_FILE);

    FILE* file_ptr = fopen(file_path.c_str(), "w");
    if (file_ptr == nullptr) {
        LOG_E("%s:%d | Failed to open the schedule file.", __FILE__, __LINE__);
        return ESP_ERR_NOT_FOUND;
    }

    std::string json_data = ToJson();

    const std::size_t written =
        fwrite(json_data.c_str(), sizeof(char), json_data.length(), file_ptr);
    if (written != json_data.length()) {
        LOG_E("%s:%d | Failed to completely save the schedule file.", __FILE__, __LINE__);
    }

    fclose(file_ptr);
    return written == json_data.length() ? ESP_OK : ESP_FAIL;
}

esp_err_t Schedule::SetSchedule(const std::span<const SchedulePoint> schedule) {
    if (xSemaphoreTake(xScheduleMutex, pdMS_TO_TICKS(10'000)) != pdTRUE) {
        LOG_E("%s:%d | Failed to obtain the schedule resource in time.", __FILE__, __LINE__);
        return ESP_ERR_TIMEOUT;
    }
    mSchedule.assign(schedule.begin(), schedule.end());
    xSemaphoreGive(xScheduleMutex);

    ReindexSchedule();
    Save();

    return ESP_OK;
}

std::vector<SchedulePoint> Schedule::GetScheduleArray() {
    if (xSemaphoreTake(xScheduleMutex, pdMS_TO_TICKS(10'000)) != pdTRUE) {
        LOG_E("%s:%d | Failed to obtain the schedule resource in time.", __FILE__, __LINE__);
        return {};
    }

    auto copy = mSchedule;

    xSemaphoreGive(xScheduleMutex);

    return copy;
}

SchedulePoint Schedule::GetSchedulePoint() {
    if (xSemaphoreTake(xScheduleMutex, pdMS_TO_TICKS(10'000)) != pdTRUE) {
        LOG_E("%s:%d | Failed to obtain the schedule resource in time.", __FILE__, __LINE__);
        return SchedulePoint{.day_minute = 0, .alarm_type = AlarmType::None};
    }

    // LOG_I("Index: %d", mScheduleIndex);

    auto schedule_point = mSchedule.at(mScheduleIndex);

    xSemaphoreGive(xScheduleMutex);

    return schedule_point;
}

uint16_t Schedule::GetSystemMinute() {
    time_t rawtime;
    time(&rawtime);
    const tm* const tl = localtime(&rawtime);

    return static_cast<uint16_t>(tl->tm_hour * 60 + tl->tm_min);
}

void Schedule::AdvanceSchedule() {
    if (xSemaphoreTake(xScheduleMutex, pdMS_TO_TICKS(10'000)) != pdTRUE) {
        LOG_E("%s:%d | Failed to obtain the schedule resource in time.", __FILE__, __LINE__);
        return;
    }

    if (mScheduleIndex == mSchedule.size() - 1) {
        mScheduleIndex = 0;
    } else {
        ++mScheduleIndex;
    }

    xSemaphoreGive(xScheduleMutex);
}

void Schedule::ReindexSchedule() {
    if (xSemaphoreTake(xScheduleMutex, pdMS_TO_TICKS(10'000)) != pdTRUE) {
        LOG_E("%s:%d | Failed to obtain the schedule resource in time.", __FILE__, __LINE__);
        return;
    }

    // sort the array so that day minutes would be in ascending order
    std::sort(
        mSchedule.begin(), mSchedule.end(),
        [](const SchedulePoint& a, const SchedulePoint& b) { return a.day_minute < b.day_minute; });

    const uint16_t system_minute = GetSystemMinute();

    // find the point when the next alarm should be fired
    for (std::size_t i = 0; i < mSchedule.size(); ++i) {
        if (mSchedule.at(i).day_minute >= system_minute) {
            mScheduleIndex = i;
            break;
        }
    }

    xSemaphoreGive(xScheduleMutex);
}

std::string Schedule::ToJson() {
    // create the root object
    cJSON* schedule_root = cJSON_CreateObject();
    if (schedule_root == nullptr) {
        cJSON_Delete(schedule_root);
        return "";
    }

    // cJSON_AddItemToObject(schedule, "schedule_items", schedule_items);
    cJSON* schedule_items = cJSON_AddArrayToObject(schedule_root, "schedule_items");
    if (schedule_items == nullptr) {
        cJSON_Delete(schedule_root);
        return "";
    }

    const auto schedule_copy = GetScheduleArray();

    for (const SchedulePoint& item : schedule_copy) {
        // create & add a schedule point
        cJSON* schedule_point = cJSON_CreateObject();
        if (schedule_point == nullptr) {
            cJSON_Delete(schedule_root);
            return "";
        }
        cJSON_AddItemToArray(schedule_items, schedule_point);

        // create & add schedule point's time
        if (cJSON_AddNumberToObject(schedule_point, "time", item.day_minute) == nullptr) {
            cJSON_Delete(schedule_root);
            return "";
        }

        // create & add schedule point's alarm type
        if (cJSON_AddNumberToObject(schedule_point, "type",
                                    static_cast<uint16_t>(item.alarm_type)) == nullptr) {
            cJSON_Delete(schedule_root);
            return "";
        }
    }

    return std::string(cJSON_Print(schedule_root));
}

esp_err_t Schedule::FromJson(const std::string_view raw_json) {
    LOG_I("New JSON data: %.*s", raw_json.length(), raw_json.data());

    const char* parse_error_ptr = nullptr;
    cJSON* schedule_json =
        cJSON_ParseWithLengthOpts(raw_json.data(), raw_json.length(), &parse_error_ptr, false);

    if (schedule_json == nullptr) {
        if (parse_error_ptr != nullptr) {
            LOG_E("%s:%d | Error parsing JSON with schedule. Error before: %s", __FILE__, __LINE__,
                  parse_error_ptr);
        }

        cJSON_Delete(schedule_json);
        return ESP_ERR_INVALID_ARG;
    }

    cJSON* schedule_items = cJSON_GetObjectItemCaseSensitive(schedule_json, "schedule_items");
    if (schedule_items == nullptr) {
        LOG_E("%s:%d | Error parsing JSON with schedule.", __FILE__, __LINE__);
        cJSON_Delete(schedule_json);
        return ESP_ERR_INVALID_ARG;
    }

    std::vector<SchedulePoint> result_schedule;
    cJSON* schedule_point;
    cJSON_ArrayForEach(schedule_point, schedule_items) {
        cJSON* time = cJSON_GetObjectItemCaseSensitive(schedule_point, "time");
        cJSON* type = cJSON_GetObjectItemCaseSensitive(schedule_point, "type");

        if (!cJSON_IsNumber(time) || !cJSON_IsNumber(type)) {
            LOG_E("%s:%d | Error parsing JSON with schedule.", __FILE__, __LINE__);
            cJSON_Delete(schedule_json);
            return ESP_ERR_INVALID_ARG;
        }

        result_schedule.push_back(
            SchedulePoint{.day_minute = static_cast<uint16_t>(time->valuedouble),
                          .alarm_type = static_cast<AlarmType>(type->valuedouble)});
    }

    cJSON_Delete(schedule_json);

    return SetSchedule(result_schedule);
}