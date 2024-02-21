#include "RelayView.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace views::relay {

RelayView::RelayView(const RelayType relay_type, ScheduleModel& time_model)
  : mRelay(relay_type)
{
  time_model.AddScheduleObserver(this);
}

void RelayView::FireAlarm(data::AlarmType alarm_type)
{
  const std::size_t iterations = static_cast<std::size_t>(alarm_type);

  for (std::size_t i = 0; i < iterations; ++i) {
    mRelay.Open();
    vTaskDelay(pdMS_TO_TICKS(1'000));
    mRelay.Close();
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

}