#pragma once

#include "RelayController.hpp"

#include "ConstantRelay.hpp"
#include "ImpulseRelay.hpp"

namespace cmpt::relay {

RelayController::RelayController(RelayType relay_type)
{
  switch (relay_type) {
    case RelayType::Impulse:
      mRelayImplement = new ImpulseRelay();
      break;

    case RelayType::Constant:
      mRelayImplement = new ConstantRelay();
      break;

    default:
      break;
  }
}

RelayController::RelayController()
{
  mRelayImplement = new DefaultRelay();
}

RelayController::~RelayController()
{
  delete mRelayImplement;
}

esp_err_t
RelayController::Init()
{
  return mRelayImplement->Init();
}

esp_err_t
RelayController::Open()
{
  return mRelayImplement->Open();
}

esp_err_t
RelayController::Close()
{
  return mRelayImplement->Close();
}

}