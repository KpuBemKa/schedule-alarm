#pragma once

#include "hal/gpio_hal.h"

#define DEBUG_WIFI 1
#define DEBUG_HTTP 1
#define DEBUG_SLEEP 1

namespace pins {

constexpr gpio_num_t BUTTON = gpio_num_t::GPIO_NUM_10;
constexpr gpio_num_t RELAY = gpio_num_t::GPIO_NUM_12;

}  // namespace pins
