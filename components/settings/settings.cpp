#include "settings.hpp"

namespace settings {

Settings& Settings::GetInstance() {
    static Settings instance;
    return instance;
}

settings::Changable Settings::GetSettings() const {
    taskENTER_CRITICAL(&xSettingsSpinlock);
    auto copy = mSettings;
    taskEXIT_CRITICAL(&xSettingsSpinlock);

    return copy;
}

void Settings::UpdateSettings(const settings::Changable& new_settings) {
    taskENTER_CRITICAL(&xSettingsSpinlock);
    mSettings = new_settings;
    taskEXIT_CRITICAL(&xSettingsSpinlock);

    NotifySettingsChanged();
}

}  // namespace settings