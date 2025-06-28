#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <string_view>
#include <vector>

#include "esp_err.h"

namespace schd {

constexpr std::string_view SCHD_FILE_PATH = "";

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

enum class ScheduleError
{
    OK = 0x0,
    Error = 0x1,

    InvalidArgument = 0x11,

    Overlap = 0x101,
    Duplicate = 0x102,
    LoopOverflow = 0x103,
    Unordered = 0x104,
    InvalidLoopRange = 0x105,
};

class ISchedule
{
  public:
    constexpr virtual ScheduleType GetScheduleType() const = 0;

    virtual bool IsEmpty() const = 0;
    virtual std::size_t GetPointsCount() const = 0;

    /**
     * Check if currently selected schedule point should fire
     */
    virtual bool IsFired() const = 0;
    /**
     * Get the action associated with currently selected schedule point
     */
    virtual Action GetAction() const = 0;

    /**
     * Selects next schedule point to be followed
     */
    virtual void AdvanceSchedule() = 0;
    /**
     * Modifies internal values to re-align the selected schedule point with local time
     */
    virtual void ReindexSchedule() = 0;

    virtual ScheduleError VerifySchedule() const = 0;

    virtual std::vector<uint8_t> Serialize() const = 0;
    virtual esp_err_t Serialize(std::span<uint8_t> output) const = 0;

    /// @brief Construct internal schedule variables from the provided raw bytes
    /// @param raw_data
    /// @return Amount of bytes read from `raw_data` in case of no errors, `esp_err_t` in case of errors
    virtual std::expected<uint32_t, ScheduleError> Deserialize(const std::span<const uint8_t> raw_data) = 0;

    virtual ~ISchedule() {};
};

} // namespace schd
