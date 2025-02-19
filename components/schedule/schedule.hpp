#pragma once

#include <cstdint>
#include <span>
#include <vector>

enum class AlarmType : uint16_t { Single = 1, Double = 2, Triple = 3 };

struct SchedulePoint {
    // Represents a minute of the day when the alarm should fire
    uint16_t day_minute;
    AlarmType alarm_type;
};

class Schedule {
   public:
    void SetIndex(const std::size_t new_index) { m_schedule_index = new_index; }
    void SetSchedule(const std::span<const SchedulePoint> schedule) {
        m_schedule.insert(m_schedule.begin(), schedule.begin(), schedule.end());
    }

    SchedulePoint GetSchedulePoint() { return m_schedule.at(m_schedule_index); }
    uint16_t GetSystemMinute();

    void AdvanceSchedule() {
        if (m_schedule_index == m_schedule.size() - 1) {
            m_schedule_index = 0;
        } else {
            ++m_schedule_index;
        }
    }

   private:
    std::size_t m_schedule_index = 0;
    std::vector<SchedulePoint> m_schedule = {};
};