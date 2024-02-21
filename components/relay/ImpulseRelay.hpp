#pragma once

#include "IRelay.hpp"

namespace cmpt::relay {

class ImpulseRelay : public IRelay
{
public:
  esp_err_t Init() override;
  esp_err_t Open() override;
  esp_err_t Close() override;
};

} // namespace relay
