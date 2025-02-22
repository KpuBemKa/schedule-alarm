#pragma once

#include <functional>
#include <span>

#include "esp_wifi.h"

#include "settings.hpp"

namespace wifi {

class WifiController : public settings::ISettingsObserver
{
  public:
    WifiController(settings::Settings& settings)
      : mSettings(settings)
    {
        mSettings.AddSettingsObserver(this);
    }

    esp_err_t Init();

    esp_err_t Start();
    esp_err_t Stop();

    bool IsStarted() { return mIsStarted; }
    bool IsConnected() { return mIsConnected; }

    int GetConnectionsCount();

  private:
    static void WifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

    esp_err_t ConfigAP();
    esp_err_t ConfigSTA();
    esp_err_t ConfigDnsAP();

    void SettingsChangedUpdate() override;

  private:
    settings::Settings& mSettings;

    esp_netif_t* mApNetif = nullptr;
    esp_netif_t* mStaNetif = nullptr;
    esp_event_handler_instance_t mEventHandlerWifi, mEventHandlerIP;

    bool mIsStarted = false;
    bool mIsConnected = false;
    int mRetryNum = 0;
};

} // namespace wifi
