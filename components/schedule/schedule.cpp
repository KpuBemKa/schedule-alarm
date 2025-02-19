#include "schedule.hpp"

#include <ctime>

uint16_t Schedule::GetSystemMinute() {
    time_t rawtime;
    time(&rawtime);
    const tm* const tl = localtime(&rawtime);

    return static_cast<uint16_t>(tl->tm_hour * 60 + tl->tm_min);
}