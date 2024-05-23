#pragma once

#include <functional>
#include <string_view>

#include "esp_wifi.h"

namespace wifi {

class WiFiManager
{
public:
  /// @brief Start the Wi-Fi module in Access Point mode
  /// @param ssid Access Point SSID
  /// @param password Access Point password. Pass empty string for no password
  /// @return
  esp_err_t StartAP(const std::string_view ssid, const std::string_view password);
  esp_err_t StopAP();

private:
  static void WifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

private:
  esp_netif_t* mApNetif;
  esp_event_handler_instance_t mEventHandlerInstance;
};

} // namespace wifi
