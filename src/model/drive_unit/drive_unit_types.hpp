#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "model/motor/motor_types.hpp"

namespace qrc {

enum class DriveUnitState {
    Normal,
    Degraded,
    Fault,
};

struct ParallelDriveUnitConfig {
    std::string name{"parallel_drive_unit"};
    std::vector<std::uint16_t> motor_ids{};
    std::vector<std::uint16_t> can_line_ids{};
    std::vector<double> coupling_ratio{};
    bool allow_single_motor_degrade{true};
};

struct DriveUnitMemberTelemetry {
    std::uint16_t motor_id{0};
    std::uint16_t can_line_id{0};
    bool active{false};
    MotorTelemetry telemetry{};
};

struct DriveUnitTelemetry {
    DriveUnitState state{DriveUnitState::Fault};
    int active_member_count{0};
    std::vector<DriveUnitMemberTelemetry> members{};
    MotorTelemetry equivalent{};
};

}  // namespace qrc
