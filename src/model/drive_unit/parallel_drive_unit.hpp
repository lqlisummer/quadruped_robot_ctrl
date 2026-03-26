#pragma once

#include <memory>
#include <vector>

#include "model/drive_unit/drive_unit_types.hpp"
#include "model/motor/motor_types.hpp"

namespace qrc {

class BoardClient;
class MotorDriver;

class ParallelDriveUnit {
public:
    ParallelDriveUnit(BoardConfig board_config, ParallelDriveUnitConfig config, MotorConfig motor_template);
    ~ParallelDriveUnit();

    ParallelDriveUnit(const ParallelDriveUnit&) = delete;
    ParallelDriveUnit& operator=(const ParallelDriveUnit&) = delete;

    bool initialize();
    void shutdown();

    bool clearFault();
    bool enable();
    bool disable();

    DriveUnitTelemetry sample(bool fast_feedback) const;

    DriveUnitState state() const noexcept;
    const ParallelDriveUnitConfig& config() const noexcept;

private:
    struct DriveMember {
        std::uint16_t motor_id{0};
        std::uint16_t can_line_id{0};
        double coupling{0.0};
        bool active{false};
        std::unique_ptr<MotorDriver> driver{};
    };

    BoardConfig board_config_;
    ParallelDriveUnitConfig config_;
    MotorConfig motor_template_;

    std::unique_ptr<BoardClient> board_;
    std::vector<DriveMember> members_;
    DriveUnitState state_{DriveUnitState::Fault};
};

}  // namespace qrc
