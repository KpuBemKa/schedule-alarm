#pragma once

#include <cstdint>
#include <list>
#include <string>
#include <string_view>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "settings_struct.hpp"

namespace settings {

class ISettingsObserver {
   public:
    virtual void SettingsChangedUpdate() = 0;
};

class Settings {
   public:
    Settings() { xSettingsSpinlock = portMUX_INITIALIZER_UNLOCKED; }

    esp_err_t Load();
    esp_err_t Save();

    settings::Changable GetSettings() const;
    esp_err_t UpdateSettings(const settings::Changable& new_settings, bool surpress_updates = false);

    void AddSettingsObserver(ISettingsObserver* observer) {
        mSettingsObservers.push_back(observer);
    }

    std::string ToJson();
    esp_err_t FromJson(const std::string_view raw_json, bool surpress_updates = false);

   private:
    esp_err_t Save(const Changable& new_settings);
    void SetSettings(const Changable& new_settings);

    void NotifySettingsChanged() {
        for (auto observer : mSettingsObservers) {
            observer->SettingsChangedUpdate();
        }
    }

    // esp_err_t ExtractStringFromJson(cJSON*)

   private:
    static constexpr const std::string_view SCHEDULE_FILE = "settings.json";

    mutable portMUX_TYPE xSettingsSpinlock;
    settings::Changable mSettings;

    std::list<ISettingsObserver*> mSettingsObservers;
};

}  // namespace settings
