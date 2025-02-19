#pragma once

#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "settings_struct.hpp"

namespace settings {

class Settings {
   public:
    Settings(Settings& other) = delete;
    void operator=(const Settings&) = delete;

    static Settings& GetInstance();

    void LoadFromMemory();
    void SaveToMemory() const;

    settings::Changable GetSettings() const;
    void UpdateSettings(const settings::Changable& new_settings);

   private:
    Settings() {}

   private:
    mutable portMUX_TYPE xSettingsSpinlock;
    settings::Changable mSettings;
};

}  // namespace settings
