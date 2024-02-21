#pragma once

#include "esp_err.h"

namespace cmpt::relay {

class IRelay
{
public:
  virtual esp_err_t Init() = 0;
  virtual esp_err_t Open() = 0;
  virtual esp_err_t Close() = 0;

  virtual ~IRelay() = default;
};

class DefaultRelay : public IRelay
{
public:
  esp_err_t Init() override { return ESP_OK; };
  esp_err_t Open() override { return ESP_OK; };
  esp_err_t Close() override { return ESP_OK; };
};

} // namespace relay
