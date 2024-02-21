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

struct schedule_row
{
  uint16_t week_minute;
  AlarmType alarm_type;

  friend bool operator==(const schedule_row& lhs, const schedule_row& rhs)
  {
    return lhs.week_minute == rhs.week_minute && lhs.alarm_type == rhs.alarm_type;
  }
};

struct schedule
{
  std::vector<schedule_row> rows;

  schedule();
  schedule(const std::size_t no_of_rows, const void* data);

  friend bool operator==(const schedule& lhs, const schedule& rhs) { return lhs.rows == rhs.rows; }
};

} // namespace data