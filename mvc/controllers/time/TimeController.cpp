#include "TimeController.hpp"

#include <ctime>
#include <iomanip>

namespace ctlrs::time {

TimeController::TimeController(ScheduleModel& schedule_model)
  : mScheduleModel(schedule_model)
{
}

void
TimeController::EspTask(void* args)
{
  // Set timezone for Easter European Time
  setenv("TZ", "Europe/Kiev", 1);
  tzset();

  TimeController* self = reinterpret_cast<TimeController*>(args);
  self->Run();
}

void
TimeController::Run()
{
  std::time_t now;
  struct std::tm timeinfo;

  std::time(&now);

  localtime_r(&now, &timeinfo);
}

}