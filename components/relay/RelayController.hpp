#pragma once

#include "IRelay.hpp"

namespace cmpt::relay {

enum class RelayType
{
  Impulse,
  Constant
};

class RelayController
{
public:
  RelayController(RelayType relay_type);
  RelayController();
  ~RelayController();

  esp_err_t Init();
  esp_err_t Open();
  esp_err_t Close();

private:
private:
  IRelay* mRelayImplement;
};

} // namespace relay
