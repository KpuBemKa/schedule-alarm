#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <vector>

#include "esp_err.h"

namespace schd {

enum class ScheduleType : uint8_t
{
    Daily = 0x1,
    Weekly = 0x2,
    Monthly = 0x3,
    Yearly = 0x4,
    Custom = 0x5,
};

enum class Action : uint8_t
{
    None,
    CloseRelay,
    OpenRelay
};

class ISchedule
{
  public:
    constexpr virtual ScheduleType GetScheduleType() const = 0;

    virtual bool IsEmpty() const = 0;
    virtual std::size_t GetPointsCount() const = 0;

    virtual bool IsFired() const = 0;
    virtual Action GetAction() const = 0;

    virtual void AdvanceSchedule() = 0;
    virtual void ReindexSchedule() = 0;

    virtual std::vector<uint8_t> Serialize() const = 0;
    virtual esp_err_t Serialize(std::span<uint8_t> output) const = 0;
    virtual std::expected<uint32_t, esp_err_t> Deserialize(const std::span<const uint8_t> raw_data) = 0;

    virtual ~ISchedule() {};
};

} // namespace schd
