#pragma once

#include "models/schedule/ScheduleModel.hpp"

namespace ctlrs::time
{

using models::schedule::ScheduleModel;

class TimeController
{
public:
  TimeController(ScheduleModel& schedule_model);
  
  static void EspTask(void* args);

private:
  void Run();

private:
  ScheduleModel& mScheduleModel;
};


} // namespace ctlrs::time


