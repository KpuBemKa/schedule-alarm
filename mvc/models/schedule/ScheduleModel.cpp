#include "ScheduleModel.hpp"

namespace models::schedule {
ScheduleModel::ScheduleModel() {}

void
ScheduleModel::SetWeekMinute(uint32_t new_week_minue)
{
  if (mWeekMinute == new_week_minue) {
    return;
  }

  mWeekMinute = new_week_minue;

  TryFireAlarm();
}

void
ScheduleModel::SetSchedule(const data::schedule& new_schedule)
{
  if (mSchedule == new_schedule) {
    return;
  }

  mSchedule = new_schedule;
}

void
ScheduleModel::AddScheduleObserver(IScheduleObserver* observer)
{
  mScheduleObservers.push_back(observer);
}

void
ScheduleModel::TryFireAlarm()
{
  for (auto schedule_row : mSchedule.rows) {
    if (schedule_row.week_minute == mWeekMinute) {
      UpdateScheduleObservers(schedule_row.alarm_type);
    }
  }
}

void
ScheduleModel::UpdateScheduleObservers(data::AlarmType alarm_type)
{
  for (auto observer : mScheduleObservers) {
    observer->FireAlarm(alarm_type);
  }
}

} // namespace models::time