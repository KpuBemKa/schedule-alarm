#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <vector>

#include "esp_err.h"

namespace schd {

enum class Action : int
{
    None,
    CloseRelay,
    OpenRelay
};

class ISchedule
{
  public:
    virtual bool IsEmpty() const = 0;
    virtual std::size_t GetPointsCount() const = 0;

    virtual bool IsFired() const = 0;
    virtual Action GetAction() const = 0;

    virtual void AdvanceSchedule() = 0;
    virtual void ReindexSchedule() = 0;

    virtual std::vector<uint8_t> Serialize() const = 0;
    virtual esp_err_t Serialize(std::span<uint8_t> output) const = 0;
    virtual esp_err_t Deserialize(const std::span<const uint8_t> raw_data) = 0;
};

} // namespace schd
