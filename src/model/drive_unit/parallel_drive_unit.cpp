#include "model/drive_unit/parallel_drive_unit.hpp"

#include <utility>

#include "platform/phyarc/board_client.hpp"
#include "platform/phyarc/motor_driver.hpp"

namespace qrc {

ParallelDriveUnit::ParallelDriveUnit(BoardConfig board_config, ParallelDriveUnitConfig config, MotorConfig motor_template)
    : board_config_(std::move(board_config)),
      config_(std::move(config)),
      motor_template_(std::move(motor_template)) {}

ParallelDriveUnit::~ParallelDriveUnit() {
    shutdown();
}

bool ParallelDriveUnit::initialize() {
    shutdown();

    board_ = std::make_unique<BoardClient>(board_config_);
    if (!board_->connect()) {
        state_ = DriveUnitState::Fault;
        return false;
    }

    members_.clear();
    members_.reserve(config_.motor_ids.size());

    int active_count = 0;
    for (std::size_t i = 0; i < config_.motor_ids.size(); ++i) {
        DriveMember member;
        member.motor_id = config_.motor_ids[i];
        member.can_line_id = config_.can_line_ids[i];
        member.coupling = config_.coupling_ratio[i];

        auto motor_cfg = motor_template_;
        motor_cfg.can_id = member.motor_id;
        motor_cfg.can_line_id = member.can_line_id;

        member.driver = std::make_unique<MotorDriver>(*board_, motor_cfg);
        member.active = member.driver->create();
        if (!member.active && !config_.allow_single_motor_degrade) {
            state_ = DriveUnitState::Fault;
            shutdown();
            return false;
        }
        if (member.active) {
            ++active_count;
        }

        members_.push_back(std::move(member));
    }

    if (active_count <= 0) {
        state_ = DriveUnitState::Fault;
        return false;
    }

    state_ = (active_count == static_cast<int>(members_.size())) ? DriveUnitState::Normal : DriveUnitState::Degraded;
    return true;
}

void ParallelDriveUnit::shutdown() {
    for (auto& member : members_) {
        if (member.driver != nullptr) {
            member.driver->destroy();
        }
        member.active = false;
    }
    members_.clear();

    if (board_ != nullptr) {
        board_->disconnect();
        board_.reset();
    }

    state_ = DriveUnitState::Fault;
}

bool ParallelDriveUnit::clearFault() {
    bool ok = true;
    for (auto& member : members_) {
        if (member.driver != nullptr && member.active) {
            ok = member.driver->clearFault() && ok;
        }
    }
    return ok;
}

bool ParallelDriveUnit::enable() {
    bool ok = true;
    for (auto& member : members_) {
        if (member.driver != nullptr && member.active) {
            ok = member.driver->enable() && ok;
        }
    }
    return ok;
}

bool ParallelDriveUnit::disable() {
    bool ok = true;
    for (auto& member : members_) {
        if (member.driver != nullptr && member.active) {
            ok = member.driver->disable() && ok;
        }
    }
    return ok;
}

DriveUnitTelemetry ParallelDriveUnit::sample(bool fast_feedback) const {
    DriveUnitTelemetry out;

    double sum_weight = 0.0;
    int active = 0;

    for (const auto& member : members_) {
        DriveUnitMemberTelemetry mt;
        mt.motor_id = member.motor_id;
        mt.can_line_id = member.can_line_id;
        mt.active = member.active && member.driver != nullptr && member.driver->isCreated();

        if (mt.active) {
            mt.telemetry = fast_feedback ? member.driver->readPvctFast() : member.driver->readPvct();
            mt.telemetry.encoder_value = member.driver->readEncoder();

            out.equivalent.position_rad += mt.telemetry.position_rad * member.coupling;
            out.equivalent.velocity_rad_s += mt.telemetry.velocity_rad_s * member.coupling;
            out.equivalent.current_a += mt.telemetry.current_a * member.coupling;
            out.equivalent.torque_load_nm += mt.telemetry.torque_load_nm * member.coupling;
            out.equivalent.torque_elec_nm += mt.telemetry.torque_elec_nm * member.coupling;
            sum_weight += member.coupling;
            ++active;
        }

        out.members.push_back(mt);
    }

    out.active_member_count = active;
    if (sum_weight > 0.0) {
        out.equivalent.position_rad /= sum_weight;
        out.equivalent.velocity_rad_s /= sum_weight;
        out.equivalent.current_a /= sum_weight;
        out.equivalent.torque_load_nm /= sum_weight;
        out.equivalent.torque_elec_nm /= sum_weight;
    }

    if (active == 0) {
        out.state = DriveUnitState::Fault;
    } else if (active < static_cast<int>(members_.size())) {
        out.state = DriveUnitState::Degraded;
    } else {
        out.state = DriveUnitState::Normal;
    }
    return out;
}

DriveUnitState ParallelDriveUnit::state() const noexcept {
    return state_;
}

const ParallelDriveUnitConfig& ParallelDriveUnit::config() const noexcept {
    return config_;
}

}  // namespace qrc
