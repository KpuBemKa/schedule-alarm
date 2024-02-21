#pragma once

#include "RelayController.hpp"

#include "models/schedule/ScheduleModel.hpp"

namespace views::relay {

using cmpt::relay::RelayController;
using cmpt::relay::RelayType;
using models::schedule::ScheduleModel;

class RelayView : public models::schedule::IScheduleObserver
{
public:
  RelayView(const RelayType relay_type, ScheduleModel& time_model);

  void FireAlarm(data::AlarmType alarm_type) override;

private:
  RelayController mRelay;
};

}