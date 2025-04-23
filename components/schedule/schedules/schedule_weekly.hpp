// #pragma once

// #include <array>

// #include "schedule_daily.hpp"

// namespace schd {

// class ScheduleWeekly : public ISchedule
// {
//   public:
//     bool IsEmpty() const override { return mSchedule.empty(); };
//     std::size_t GetPointsCount() const override { return mSchedule.size(); };

//     Action GetAction() const override { return mSchedule.at(GetLocalWeekDay()).GetAction(); }
//     bool IsFired() const override { return mSchedule.at(GetLocalWeekDay()).IsFired(); }

//     void AdvanceSchedule() override;
//     void ReindexSchedule() override;

//     std::expected<std::vector<uint8_t>, esp_err_t> Serialize() const override;
//     esp_err_t Serialize(std::span<uint8_t> output) const override;
//     esp_err_t Deserialize(const std::span<const uint8_t> raw_data) override;

//   private:
//     static std::size_t GetLocalWeekDay();

//   private:
//     std::array<ScheduleDaily, 7> mSchedule;
// };

// std::size_t
// ScheduleWeekly::GetLocalWeekDay()
// {
//     const std::time_t raw_time = std::time(nullptr);
//     const tm* const tl = localtime(&raw_time);
//     return static_cast<std::size_t>(tl->tm_wday);
// }

// void
// ScheduleWeekly::ReindexSchedule()
// {
//     mSchedule.at(GetLocalWeekDay()).ReindexSchedule();
// }

// std::expected<std::vector<uint8_t>, esp_err_t>
// ScheduleWeekly::Serialize() const
// {
//     return std::unexpected(ESP_ERR_NOT_FOUND);
// }

// esp_err_t
// ScheduleWeekly::Serialize(std::span<uint8_t> output) const
// {
//     return (ESP_ERR_NOT_FOUND);
// }

// esp_err_t
// ScheduleWeekly::Deserialize(const std::span<const uint8_t> raw_data)
// {
//     return (ESP_ERR_NOT_FOUND);
// }

// } // namespace schd
