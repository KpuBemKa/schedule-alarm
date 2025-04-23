// #pragma once

// #include <array>

// #include "schedule_monthly.hpp"

// namespace schd {

// class ScheduleYearly : public ISchedule
// {
//   public:
//     bool IsEmpty() const override { return mSchedule.empty(); };
//     std::size_t GetPointsCount() const override { return mSchedule.size(); };

//     Action GetAction() const override { return mSchedule.at(GetLocalMonth()).GetAction(); }
//     bool IsFired() const override { return mSchedule.at(GetLocalMonth()).IsFired(); }

//     void AdvanceSchedule() override;
//     void ReindexSchedule() override;

//     std::expected<std::vector<uint8_t>, esp_err_t> Serialize() const override;
//     esp_err_t Serialize(std::span<uint8_t> output) const override;
//     esp_err_t Deserialize(const std::span<const uint8_t> raw_data) override;

//   private:
//     static std::size_t GetLocalMonth();

//   private:
//     std::array<ScheduleMonthly, 12> mSchedule;
// };

// std::size_t
// ScheduleYearly::GetLocalMonth()
// {
//     sizeof(mSchedule);
//     const std::time_t raw_time = std::time(nullptr);
//     const tm* const tl = localtime(&raw_time);
//     return static_cast<std::size_t>(tl->tm_mday);
// }

// bool
// ScheduleYearly::IsFired() const
// {
//     return mSchedule.at(GetLocalMonth() - 1).IsFired();
// }

// void
// ScheduleYearly::AdvanceSchedule()
// {
//     mSchedule.at(GetLocalMonth() - 1).AdvanceSchedule();
// }

// void
// ScheduleYearly::ReindexSchedule()
// {
//     mSchedule.at(GetLocalMonth() - 1).ReindexSchedule();
// }

// std::expected<std::vector<uint8_t>, esp_err_t>
// ScheduleYearly::Serialize() const
// {
//     return std::unexpected(ESP_ERR_NOT_FOUND);
// }

// esp_err_t
// ScheduleYearly::Serialize(std::span<uint8_t> output) const
// {
//     return (ESP_ERR_NOT_FOUND);
// }

// esp_err_t
// ScheduleYearly::Deserialize(const std::span<const uint8_t> raw_data)
// {
//     return (ESP_ERR_NOT_FOUND);
// }

// } // namespace schd
