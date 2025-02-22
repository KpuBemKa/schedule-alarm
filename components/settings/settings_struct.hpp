#pragma once

#include <string>
#include <cstdint>

namespace settings {

struct Constant {
    static constexpr uint8_t WiFiApChannel = 1;
    static constexpr uint8_t MaxWifiConnections = 2;
};

struct Changable {
    std::string wifi_ssid;
    std::string wifi_password;

    std::string remote_wifi_ssid;
    std::string remote_wifi_password;
};

}  // namespace settings
