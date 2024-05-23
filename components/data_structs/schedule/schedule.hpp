#pragma once

#include <cstdint>
#include <vector>

namespace data {

enum class AlarmType : uint16_t
{
  Single = 1,
  Double = 2,
  Triple = 3
};

struct ScheduleItem
{
  // Represents a minute of the week when the alarm should fire
  // Monday, 00:00 is 0, Sunday 23:59 is 10'079
  uint16_t week_minute;
  AlarmType alarm_type;
};

/// This is stored in NVS as a regular array, but NVS also has a separate key for the ammount of rows
/// Just to avoid a lot of potentially unused space
struct Schedule
{
  std::vector<ScheduleItem> rows;
};

} // namespace data