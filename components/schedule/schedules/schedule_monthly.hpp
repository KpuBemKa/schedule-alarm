// #pragma once

// #include <array>

// #include "schedule_daily.hpp"

// namespace schd {

// class ScheduleMonthly : public ISchedule
// {
//   public:
//     bool IsEmpty() const override { return mSchedule.empty(); };
//     std::size_t GetPointsCount() const override { return mSchedule.size(); };

//     Action GetAction() const override { return mSchedule.at(GetLocalMonthDay() - 1).GetAction(); }
//     bool IsFired() const override { return mSchedule.at(GetLocalMonthDay() - 1).IsFired(); }

//     void AdvanceSchedule() override;
//     void ReindexSchedule() override;

//     std::expected<std::vector<uint8_t>, esp_err_t> Serialize() const override;
//     esp_err_t Serialize(std::span<uint8_t> output) const override;
//     esp_err_t Deserialize(const std::span<const uint8_t> raw_data) override;

//   private:
//     static std::size_t GetLocalMonthDay();

//   private:
//     std::array<ScheduleDaily, 31> mSchedule;
// };

// std::size_t
// ScheduleMonthly::GetLocalMonthDay()
// {

//     const std::time_t raw_time = std::time(nullptr);
//     const tm* const tl = localtime(&raw_time);
//     return static_cast<std::size_t>(tl->tm_mday);
// }

// void
// ScheduleMonthly::AdvanceSchedule()
// {
//     mSchedule.at(GetLocalMonthDay() - 1).AdvanceSchedule();
// }

// void
// ScheduleMonthly::ReindexSchedule()
// {
//     mSchedule.at(GetLocalMonthDay() - 1).ReindexSchedule();
// }

// std::expected<std::vector<uint8_t>, esp_err_t>
// ScheduleMonthly::Serialize() const
// {
//     return std::unexpected(ESP_ERR_NOT_FOUND);
// }

// esp_err_t
// ScheduleMonthly::Serialize(std::span<uint8_t> output) const
// {
//     return (ESP_ERR_NOT_FOUND);
// }

// esp_err_t
// ScheduleMonthly::Deserialize(const std::span<const uint8_t> raw_data)
// {
//     return (ESP_ERR_NOT_FOUND);
// }

// } // namespace schd
