#pragma once

#include "schedule/schedule.hpp"

namespace models::schedule {

class IScheduleObserver
{
public:
  virtual ~IScheduleObserver() = default;

  virtual void FireAlarm(data::AlarmType alarm_type) = 0;
};

class IScheduleObservable
{
public:
  virtual ~IScheduleObservable() = default;

  virtual void AddScheduleObserver(IScheduleObserver* observer) = 0;

protected:
  virtual void UpdateScheduleObservers(data::AlarmType alarm_type) = 0;
};

} // namespace models::schedule
