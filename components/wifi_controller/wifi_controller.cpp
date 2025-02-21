#include "wifi_controller.hpp"

#include <algorithm>
#include <cstring>

#include "esp_log.h"
#include "esp_mac.h"

#include "config.hpp"
#include "settings.hpp"

const char TAG[] = "WIFI";

#if DEBUG_WIFI
#define LOG_I(...) ESP_LOGI(TAG, __VA_ARGS__)
#else
#define LOG_I(...)
#endif

#define LOG_W(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_E(...) ESP_LOGE(TAG, __VA_ARGS__)

namespace wifi {

constexpr uint8_t DHCPS_OFFER_DNS = 0x02;

esp_err_t WifiController::Start() {
    LOG_I("Starting Wi-Fi in AP&STA mode...");

    // Start the netif
    esp_err_t result = esp_netif_init();
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error initializing netif: %s", __FILE__, __LINE__, esp_err_to_name(result));
        return result;
    }

    // Create the event loop
    result = esp_event_loop_create_default();
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error creating event loop : %s", __FILE__, __LINE__,
              esp_err_to_name(result));
        return result;
    }

    // Register the events handler
    result = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, WifiEventHandler,
                                                 this, &mEventHandlerWifi);
    result |= esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, WifiEventHandler,
                                                  this, &mEventHandlerIP);
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error registrering the handler for one the events: %s", __FILE__, __LINE__,
              esp_err_to_name(result));
        return result;
    }

    // Initialize the Wi-Fi
    const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result = esp_wifi_init(&cfg);
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error initializing Wi-Fi : %s", __FILE__, __LINE__, esp_err_to_name(result));
        return result;
    }

    // Set Wi-Fi to AP&STA mode
    result = esp_wifi_set_mode(WIFI_MODE_APSTA);
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error setting Wi-Fi in AP mode: %s", __FILE__, __LINE__,
              esp_err_to_name(result));
        return result;
    }

    result = ConfigAP();
    if (result != ESP_OK)
        return result;

    result = ConfigSTA();
    if (result != ESP_OK)
        return result;

    // Start Wi-Fi
    result = esp_wifi_start();
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error starting Wi-Fi: %s", __FILE__, __LINE__, esp_err_to_name(result));
        return result;
    }

    while (!mIsConnected && xTaskGetTickCount() <= pdMS_TO_TICKS(30'000)) {
        vTaskDelay(pdMS_TO_TICKS(1'000));
    }

    if (IsConnected()) {
        LOG_I("Connected to remote AP.");
        ConfigDnsAP();
    } else {
        LOG_E("Failed to connect to remote AP.");
    }

    esp_netif_set_default_netif(mStaNetif);

    /* Enable napt on the AP netif */
    result = esp_netif_napt_enable(mApNetif);
    if (result != ESP_OK) {
        LOG_E("%s:%d | NAPT not enabled on the netif: %s", __FILE__, __LINE__,
              esp_err_to_name(result));
    }

    mIsStarted = true;
    LOG_I("Wi-Fi has been started.");

    return ESP_OK;
}

esp_err_t WifiController::Stop() {
    esp_err_t result = esp_wifi_stop();
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error stopping Wi-Fi: %s", __FILE__, __LINE__, esp_err_to_name(result));
        return result;
    }

    result = esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, mEventHandlerWifi);
    result |= esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, mEventHandlerIP);
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error unregistering one the Wi-Fi event handlers: %s", __FILE__, __LINE__,
              esp_err_to_name(result));
        return result;
    }

    esp_netif_destroy(mApNetif);
    esp_netif_destroy(mStaNetif);

    result = esp_netif_deinit();
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error de-initializing netif: %s", __FILE__, __LINE__,
              esp_err_to_name(result));
        return result;
    }

    mIsStarted = false;

    return ESP_OK;
}

int WifiController::GetConnectionsCount() {
    wifi_sta_list_t sta_list;
    const esp_err_t esp_result = esp_wifi_ap_get_sta_list(&sta_list);

    if (esp_result != ESP_OK) {
        LOG_E("%s:%d | Failed to retreive STA connections: %s", __FILE__, __LINE__,
              esp_err_to_name(esp_result));
        return 0;
    }

    return sta_list.num;
}

void WifiController::WifiEventHandler(void* arg,
                                      esp_event_base_t event_base,
                                      int32_t event_id,
                                      void* event_data) {
    WifiController* self = reinterpret_cast<WifiController*>(arg);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*)event_data;
        LOG_I("Station " MACSTR " joined, AID=%d", MAC2STR(event->mac), event->aid);

    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*)event_data;
        LOG_I("Station " MACSTR " left, AID=%d, reason:%d", MAC2STR(event->mac), event->aid,
              event->reason);

    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        LOG_I("Station started");

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        LOG_I("Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
        self->mRetryNum = 0;
        self->mIsConnected = true;
    }
}

esp_err_t WifiController::ConfigAP() {
    // Create the netif
    mApNetif = esp_netif_create_default_wifi_ap();

    const settings::Changable settings = settings::Settings::GetInstance().GetSettings();

    // Initialize the config structure
    wifi_config_t wifi_ap_config = {
        .ap =
            {
                .ssid = {},
                .password = {},
                .ssid_len = static_cast<uint8_t>(std::strlen(settings.wifi_ssid.data())),
                .channel = 1,
                .authmode = wifi_auth_mode_t::WIFI_AUTH_WPA2_PSK,
                .max_connection = settings::Constant::MaxWifiConnections,
                .pmf_cfg =
                    {
                        .required = false,
                    },
            },
    };

    std::copy_n(settings.wifi_ssid.data(), wifi_ap_config.ap.ssid_len, wifi_ap_config.ap.ssid);
    std::copy_n(settings.wifi_password.data(), std::strlen(settings.wifi_password.data()),
                wifi_ap_config.ap.password);

    if (settings.wifi_password.at(0) == '\0') {
        wifi_ap_config.ap.authmode = wifi_auth_mode_t::WIFI_AUTH_OPEN;
    }

    // Apply the settings
    esp_err_t result = esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config);
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error configuring Wi-Fi AP settings: %s", __FILE__, __LINE__,
              esp_err_to_name(result));
    }

    LOG_I("WiFi AP configured.");

    return result;
}

esp_err_t WifiController::ConfigSTA() {
    mStaNetif = esp_netif_create_default_wifi_sta();

    const settings::Changable settings = settings::Settings::GetInstance().GetSettings();

    wifi_config_t wifi_sta_config = {
        .sta =
            {
                .ssid = {},
                .password = {},
                .scan_method = WIFI_ALL_CHANNEL_SCAN,
                .threshold = {.authmode = WIFI_AUTH_WPA2_WPA3_PSK},
                .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
                .failure_retry_cnt = 5,
            },
    };

    std::copy_n(settings.remote_wifi_ssid.data(), settings.remote_wifi_ssid.size(),
                wifi_sta_config.sta.ssid);
    std::copy_n(settings.remote_wifi_password.data(), settings.remote_wifi_password.size(),
                wifi_sta_config.sta.password);

    // Apply the settings
    const esp_err_t result = esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config);
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error configuring Wi-Fi STA settings: %s", __FILE__, __LINE__,
              esp_err_to_name(result));
    }

    LOG_I("WiFi STA configured.");

    return result;
}

esp_err_t WifiController::ConfigDnsAP() {
    esp_netif_dns_info_t dns;
    esp_netif_get_dns_info(mStaNetif, ESP_NETIF_DNS_MAIN, &dns);

    esp_err_t result = esp_netif_dhcps_stop(mApNetif);
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error during disabling AP's DHSPS: %s", __FILE__, __LINE__,
              esp_err_to_name(result));
    }

    uint8_t dhcps_offer_dns = DHCPS_OFFER_DNS;
    result = esp_netif_dhcps_option(mApNetif, ESP_NETIF_OP_SET, ESP_NETIF_DOMAIN_NAME_SERVER,
                                    &dhcps_offer_dns, sizeof(dhcps_offer_dns));
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error during setting AP's DHSPS option: %s", __FILE__, __LINE__,
              esp_err_to_name(result));
        return result;
    }

    result = esp_netif_set_dns_info(mApNetif, ESP_NETIF_DNS_MAIN, &dns);
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error during setting AP's DHSPS info: %s", __FILE__, __LINE__,
              esp_err_to_name(result));
        return result;
    }

    result = esp_netif_dhcps_start(mApNetif);
    if (result != ESP_OK) {
        LOG_E("%s:%d | Error starting AP's DHSPS: %s", __FILE__, __LINE__, esp_err_to_name(result));
    }

    return result;
}

void WifiController::SettingsChangedUpdate() {}

}  // namespace wifi
