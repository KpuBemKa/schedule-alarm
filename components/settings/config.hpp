#pragma once

#include <string_view>

#include "hal/gpio_hal.h"

#define DEBUG_WIFI 1
#define DEBUG_HTTP 1
#define DEBUG_SLEEP 1

namespace config {

constexpr std::size_t MS_COUNT_BEFORE_RESET = 10'000;

constexpr std::string_view SPIFFS_BASE_PATH = "/spiffs";

} // namespace config

namespace pins {

constexpr gpio_num_t BUTTON = gpio_num_t::GPIO_NUM_3;
constexpr gpio_num_t RELAY = gpio_num_t::GPIO_NUM_0;

} // namespace pins
