#pragma once

#include <cstdint>

#include "schedule/schedule.hpp"

#include "IScheduleObserver.hpp"

namespace models::schedule {

class ScheduleModel : public IScheduleObservable {
public:
  ScheduleModel();

  void SetWeekMinute(uint32_t new_week_minue);
  void SetSchedule(const data::schedule &new_schedule);

  void AddScheduleObserver(IScheduleObserver* observer) override;

private:
  void UpdateScheduleObservers(data::AlarmType alarm_type) override;

  void TryFireAlarm();

private:
  std::vector<IScheduleObserver*> mScheduleObservers;

  uint32_t mWeekMinute;
  data::schedule mSchedule;
};

} // namespace models::time
