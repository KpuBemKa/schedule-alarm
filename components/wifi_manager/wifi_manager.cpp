#include "wifi_manager.hpp"

#include <algorithm>

#include "esp_log.h"
#include "esp_mac.h"

#include "settings.hpp"

const char TAG[] = "WIFI";

#if DEBUG_HTTP
#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#else
#define LOG_I(...)
#endif

#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

namespace wifi {

esp_err_t
WiFiManager::StartAP(const std::string_view ssid, const std::string_view password)
{
  LOG_I("Initializing Wi-Fi Access Point...");

  esp_err_t result = esp_netif_init();
  if (result != ESP_OK) {
    LOG_E("%s:%d | Error initializing netif: %s", __FILE__, __LINE__, esp_err_to_name(result));
    return result;
  }

  // Create the event loop
  result = esp_event_loop_create_default();
  if (result != ESP_ERR_INVALID_STATE && result != ESP_OK) {
    LOG_E("%s:%d | Error creating event loop : %s", __FILE__, __LINE__, esp_err_to_name(result));
    return result;
  }

  // Create the netif
  mApNetif = esp_netif_create_default_wifi_ap();

  // Initialize the Wi-Fi
  const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  result = esp_wifi_init(&cfg);
  if (result != ESP_OK) {
    LOG_E("%s:%d | Error initializing Wi-Fi : %s", __FILE__, __LINE__, esp_err_to_name(result));
    return result;
  }

  // Register the event handler
  result = esp_event_handler_instance_register(
    WIFI_EVENT, settings::WiFiEventId, WifiEventHandler, nullptr, &mEventHandlerInstance);
  if (result != ESP_OK) {
    LOG_E("%s:%d | Error registering event handler : %s", __FILE__, __LINE__, esp_err_to_name(result));
    return result;
  }

  // Initialize the config structure
  wifi_config_t wifi_ap_config = {
        .ap = {
            .ssid = 0,
            .password = 0,
            .ssid_len = static_cast<uint8_t>(ssid.size()),
            .channel = settings::WiFiApChannel,
            .authmode = password.size() > 0 ? wifi_auth_mode_t::WIFI_AUTH_WPA2_PSK : wifi_auth_mode_t::WIFI_AUTH_OPEN,
            .ssid_hidden = 0,
            .max_connection = settings::MaxWifiConnections,
            .beacon_interval = 100,
            .pairwise_cipher = wifi_cipher_type_t::WIFI_CIPHER_TYPE_TKIP_CCMP,
            .ftm_responder = true,
            .pmf_cfg = {
                .capable = true, // is deprecated but compiler throws a warning
                .required = false,
            },
            .sae_pwe_h2e =wifi_sae_pwe_method_t::WPA3_SAE_PWE_UNSPECIFIED
        },
    };
  std::copy_n(ssid.data(), ssid.size(), wifi_ap_config.ap.ssid);
  std::copy_n(password.data(), password.size(), wifi_ap_config.ap.password);

  // Set Wi-Fi in AP mode
  result = esp_wifi_set_mode(WIFI_MODE_AP);
  if (result != ESP_OK) {
    LOG_E("%s:%d | Error setting Wi-Fi in AP mode: %s", __FILE__, __LINE__, esp_err_to_name(result));
    return result;
  }

  // Apply the settings
  result = esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config);
  if (result != ESP_OK) {
    LOG_E("%s:%d | Error configuring Wi-Fi settings: %s", __FILE__, __LINE__, esp_err_to_name(result));
    return result;
  }

  // Start Wi-Fi
  result = esp_wifi_start();
  if (result != ESP_OK) {
    LOG_E("%s:%d | Error starting Wi-Fi: %s", __FILE__, __LINE__, esp_err_to_name(result));
    return result;
  }

  LOG_I("Wi-Fi has been successfully started.\nSSID: %s\nPassword: %s\nChannel:%d",
        ssid.data(),
        password.size() > 0 ? password.data() : "None",
        settings::WiFiApChannel);

  return ESP_OK;
}

esp_err_t
WiFiManager::StopAP()
{
  esp_err_t result = esp_wifi_stop();
  if (result != ESP_OK) {
    LOG_E("%s:%d | Error stopping Wi-Fi: %s", __FILE__, __LINE__, esp_err_to_name(result));
    return result;
  }

  result = esp_event_handler_instance_unregister(WIFI_EVENT, settings::WiFiEventId, mEventHandlerInstance);
  if (result != ESP_OK) {
    LOG_E("%s:%d | Error unregistering Wi-Fi event handler: %s", __FILE__, __LINE__, esp_err_to_name(result));
    return result;
  }

  esp_netif_destroy(mApNetif);

  result = esp_netif_deinit();
  if (result != ESP_OK) {
    LOG_E("%s:%d | Error de-initializing netif: %s", __FILE__, __LINE__, esp_err_to_name(result));
    return result;
  }

  return ESP_OK;
}

void
WiFiManager::WifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  switch (event_id) {
    case wifi_event_t::WIFI_EVENT_AP_STACONNECTED: {
      wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*)event_data;
      LOG_I("station " MACSTR " connected, AID=%d", MAC2STR(event->mac), event->aid);
      break;
    }

    case wifi_event_t::WIFI_EVENT_AP_STADISCONNECTED: {
      wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*)event_data;
      LOG_I("station " MACSTR " disconnected, AID=%d", MAC2STR(event->mac), event->aid);
      break;
    }

    default:
      break;
  }
}

} // namespace wifi
