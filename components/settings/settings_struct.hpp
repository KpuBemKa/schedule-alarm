#pragma once

#include <array>
#include <cstdint>

namespace settings {

struct Constant {
    static constexpr uint8_t WiFiApChannel = 1;
    static constexpr uint8_t MaxWifiConnections = 2;
};

struct Changable {
    std::array<char, 32> wifi_ssid;
    std::array<char, 32> wifi_password;

    std::array<char, 32> remote_wifi_ssid;
    std::array<char, 32> remote_wifi_password;
};

}  // namespace settings
