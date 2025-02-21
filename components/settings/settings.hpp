#pragma once

#include <cstdint>
#include <list>
#include <memory>

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
    Settings(Settings& other) = delete;
    void operator=(const Settings&) = delete;

    static Settings& GetInstance();

    settings::Changable GetSettings() const;
    void UpdateSettings(const settings::Changable& new_settings);

    void AddSettingsObserver(ISettingsObserver* observer) {
        mSettingsObservers.push_back(observer);
    }

   private:
    Settings() { xSettingsSpinlock = portMUX_INITIALIZER_UNLOCKED; }

    void NotifySettingsChanged() {
        for (auto observer : mSettingsObservers) {
            observer->SettingsChangedUpdate();
        }
    }

   private:
    mutable portMUX_TYPE xSettingsSpinlock;
    settings::Changable mSettings;

    std::list<ISettingsObserver*> mSettingsObservers;
};

}  // namespace settings
