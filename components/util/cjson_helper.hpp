#pragma once

#include <cstring>
#include <expected>
#include <string_view>

#include "cJSON.h"
#include "esp_err.h"

namespace util {

std::expected<std::string, esp_err_t> ExtractStringFromJSON(cJSON* json,
                                                            const std::string_view item_name) {
    cJSON* wifi_ssid_json = cJSON_GetObjectItemCaseSensitive(json, item_name.data());
    if (wifi_ssid_json == nullptr)
        return std::unexpected(ESP_ERR_NOT_FOUND);

    char* string_value = cJSON_GetStringValue(wifi_ssid_json);
    if (string_value == nullptr)
        return std::unexpected(ESP_ERR_NOT_FOUND);

    return std::string(string_value);
}

}  // namespace util
