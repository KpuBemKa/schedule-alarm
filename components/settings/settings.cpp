#include "settings.hpp"

#include <cstring>
#include <vector>

#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"

#include "cjson_helper.hpp"
#include "config.hpp"

const char TAG[] = "SETTINGS";
#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

namespace settings {

const Changable kDefaultSettings{ .wifi_ssid = "School Alarm",
                                  .wifi_password = "",
                                  .remote_wifi_ssid = "",
                                  .remote_wifi_password = "",
                                  .timezone = "UTC0" };

esp_err_t
Settings::Load()
{
    std::string file_path(SPIFFS_BASE_PATH);
    file_path.append("/").append(SCHEDULE_FILE);

    FILE* file_ptr = fopen(file_path.c_str(), "r");
    if (file_ptr == nullptr) {
        LOG_W("%s:%d | Failed to open the settings file. Loading the default values...", __FILE__, __LINE__);
        return UpdateSettings(kDefaultSettings);
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
        LOG_E("%s:%d | Error reading the settings file. Ammount of bytes read: %u", __FILE__, __LINE__, read_count);
        LOG_W("Loading the default values...");
        return UpdateSettings(kDefaultSettings);
    }

    return FromJson(std::string_view(file_buf.data(), read_count), /* surpress_updates = */ true);
}

esp_err_t
Settings::Save()
{
    return Save(GetSettings());
}

settings::Changable
Settings::GetSettings() const
{
    taskENTER_CRITICAL(&xSettingsSpinlock);
    auto copy = mSettings;
    taskEXIT_CRITICAL(&xSettingsSpinlock);

    return copy;
}

esp_err_t
Settings::UpdateSettings(const settings::Changable& new_settings, bool surpress_updates)
{
    SetSettings(new_settings);

    if (surpress_updates)
        return ESP_OK;

    esp_err_t esp_result = Save(new_settings);
    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Error saving settings: %s", __FILE__, __LINE__, esp_err_to_name(esp_result));
        return esp_result;
    }

    NotifySettingsChanged();

    return ESP_OK;
}

std::string
Settings::ToJson()
{
    // create the root object
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        LOG_E("%s:%d | Error creating a JSON.", __FILE__, __LINE__);
        cJSON_Delete(root);
        return "";
    }

    const auto settings_copy = GetSettings();

    cJSON* wifi_ssid = cJSON_AddStringToObject(root, "wifi_ssid", settings_copy.wifi_ssid.c_str());
    if (wifi_ssid == nullptr) {
        LOG_E("%s:%d | Error creating a JSON.", __FILE__, __LINE__);
        cJSON_Delete(root);
        return "";
    }

    cJSON* wifi_password = cJSON_AddStringToObject(root, "wifi_password", settings_copy.wifi_password.c_str());
    if (wifi_password == nullptr) {
        LOG_E("%s:%d | Error creating a JSON.", __FILE__, __LINE__);
        cJSON_Delete(root);
        return "";
    }

    cJSON* remote_wifi_ssid = cJSON_AddStringToObject(root, "remote_wifi_ssid", settings_copy.remote_wifi_ssid.c_str());
    if (remote_wifi_ssid == nullptr) {
        LOG_E("%s:%d | Error creating a JSON.", __FILE__, __LINE__);
        cJSON_Delete(root);
        return "";
    }

    cJSON* remote_wifi_password =
      cJSON_AddStringToObject(root, "remote_wifi_password", settings_copy.remote_wifi_password.c_str());
    if (remote_wifi_password == nullptr) {
        LOG_E("%s:%d | Error creating a JSON.", __FILE__, __LINE__);
        cJSON_Delete(root);
        return "";
    }

    cJSON* timezone = cJSON_AddStringToObject(root, "timezone", settings_copy.timezone.c_str());
    if (timezone == nullptr) {
        LOG_E("%s:%d | Error creating a JSON.", __FILE__, __LINE__);
        cJSON_Delete(root);
        return "";
    }

    return std::string(cJSON_Print(root));
}

esp_err_t
Settings::FromJson(const std::string_view raw_json, bool surpress_updates)
{
    const char* parse_error_ptr = nullptr;
    cJSON* root_json = cJSON_ParseWithLengthOpts(raw_json.data(), raw_json.length(), &parse_error_ptr, false);

    if (root_json == nullptr) {
        if (parse_error_ptr != nullptr) {
            LOG_E("%s:%d | Error parsing JSON with schedule. Error before: %s", __FILE__, __LINE__, parse_error_ptr);
        }

        cJSON_Delete(root_json);
        return ESP_ERR_INVALID_ARG;
    }

    Changable new_settings;

    std::expected<std::string, esp_err_t> exp_result = util::ExtractStringFromJSON(root_json, "wifi_ssid");
    if (!exp_result.has_value()) {
        LOG_E("%s:%d | Error parsing JSON: %s", __FILE__, __LINE__, esp_err_to_name(exp_result.error()));
        cJSON_Delete(root_json);
        return exp_result.error();
    }
    new_settings.wifi_ssid = exp_result.value();

    exp_result = util::ExtractStringFromJSON(root_json, "wifi_password");
    if (!exp_result.has_value()) {
        LOG_E("%s:%d | Error parsing JSON: %s", __FILE__, __LINE__, esp_err_to_name(exp_result.error()));
        cJSON_Delete(root_json);
        return exp_result.error();
    }
    new_settings.wifi_password = exp_result.value();

    exp_result = util::ExtractStringFromJSON(root_json, "remote_wifi_ssid");
    if (!exp_result.has_value()) {
        LOG_E("%s:%d | Error parsing JSON: %s", __FILE__, __LINE__, esp_err_to_name(exp_result.error()));
        cJSON_Delete(root_json);
        return exp_result.error();
    }
    new_settings.remote_wifi_ssid = exp_result.value();

    exp_result = util::ExtractStringFromJSON(root_json, "remote_wifi_password");
    if (!exp_result.has_value()) {
        LOG_E("%s:%d | Error parsing JSON: %s", __FILE__, __LINE__, esp_err_to_name(exp_result.error()));
        cJSON_Delete(root_json);
        return exp_result.error();
    }
    new_settings.remote_wifi_password = exp_result.value();

    exp_result = util::ExtractStringFromJSON(root_json, "timezone");
    if (!exp_result.has_value()) {
        LOG_E("%s:%d | Error parsing JSON: %s", __FILE__, __LINE__, esp_err_to_name(exp_result.error()));
        cJSON_Delete(root_json);
        return exp_result.error();
    }
    new_settings.timezone = exp_result.value();

    cJSON_Delete(root_json);

    return UpdateSettings(new_settings, surpress_updates);
}

esp_err_t
Settings::Save(const Changable& new_settings)
{
    std::string file_path(SPIFFS_BASE_PATH);
    file_path.append("/").append(SCHEDULE_FILE);

    FILE* file_ptr = fopen(file_path.c_str(), "w");
    if (file_ptr == nullptr) {
        LOG_E("%s:%d | Failed to open the settings file.", __FILE__, __LINE__);
        return ESP_ERR_NOT_FOUND;
    }

    std::string json_data = ToJson();
    if (json_data.empty()) {
        LOG_E("%s:%d | Error creating the settings JSON.", __FILE__, __LINE__);
    }

    const std::size_t written = fwrite(json_data.c_str(), sizeof(char), json_data.length(), file_ptr);
    if (written != json_data.length()) {
        LOG_E("%s:%d | Failed to completely save the settings file.", __FILE__, __LINE__);
    }

    fclose(file_ptr);
    return written == json_data.length() ? ESP_OK : ESP_FAIL;
}

void
Settings::SetSettings(const Changable& new_settings)
{
    taskENTER_CRITICAL(&xSettingsSpinlock);
    mSettings = new_settings;
    taskEXIT_CRITICAL(&xSettingsSpinlock);
}

} // namespace settings