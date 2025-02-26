#pragma once

#include <string_view>

#include "esp_netif_sntp.h"

#include "settings.hpp"

namespace wifi {

class SNTP : public settings::ISettingsObserver
{
  public:
    SNTP(settings::Settings& settings)
      : mSettings(settings)
    {
        settings.AddSettingsObserver(this);
    }

    esp_err_t Init(const std::size_t wait_for_sync_ms = 0);

    esp_err_t Resync() { return esp_netif_sntp_start(); }

    void SetTimezone(const std::string_view timezone);

  private:
    void SettingsChangedUpdate() override { SetTimezone(mSettings.GetSettings().timezone); }

  private:
    settings::Settings mSettings;
};

} // namespace wifi
