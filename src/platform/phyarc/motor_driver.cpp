#include "platform/phyarc/motor_driver.hpp"

#include <array>
#include <chrono>
#include <utility>

#if defined(__unix__) || defined(__APPLE__)
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "platform/phyarc/board_client.hpp"

#if QRC_ENABLE_PHYARC_SDK
extern "C" {
#include "CTypes.h"
#include "RobotControl.h"
}
#endif

namespace qrc {
namespace {

#if defined(__unix__) || defined(__APPLE__)
class ScopedStdoutSilencer {
public:
    ScopedStdoutSilencer() {
        fflush(stdout);
        null_fd_ = open("/dev/null", O_WRONLY);
        if (null_fd_ < 0) {
            return;
        }
        saved_stdout_ = dup(STDOUT_FILENO);
        if (saved_stdout_ < 0) {
            close(null_fd_);
            null_fd_ = -1;
            return;
        }
        if (dup2(null_fd_, STDOUT_FILENO) < 0) {
            close(saved_stdout_);
            close(null_fd_);
            saved_stdout_ = -1;
            null_fd_ = -1;
        }
    }

    ~ScopedStdoutSilencer() {
        if (saved_stdout_ >= 0) {
            fflush(stdout);
            dup2(saved_stdout_, STDOUT_FILENO);
            close(saved_stdout_);
        }
        if (null_fd_ >= 0) {
            close(null_fd_);
        }
    }

private:
    int saved_stdout_{-1};
    int null_fd_{-1};
};
#else
class ScopedStdoutSilencer {
public:
    ScopedStdoutSilencer() = default;
};
#endif

#if QRC_ENABLE_PHYARC_SDK
unsigned int toSdkMode(MotorControlMode mode) {
    return static_cast<unsigned int>(mode);
}

unsigned int toSdkWord(ControlWord word) {
    return static_cast<unsigned int>(word);
}

unsigned int toSdkTheta(ThetaSource source) {
    return static_cast<unsigned int>(source);
}
#endif

}  // namespace

MotorDriver::MotorDriver(BoardClient& board, MotorConfig config)
    : board_(board), config_(std::move(config)) {}

MotorDriver::~MotorDriver() {
    destroy();
}

bool MotorDriver::create() {
#if !QRC_ENABLE_PHYARC_SDK
    return false;
#else
    if (created_ && motor_ != nullptr) {
        return true;
    }
    if (!board_.isConnected() || board_.raw() == nullptr) {
        return false;
    }

    motor_ = robot_create_motor(board_.raw(), config_.can_id, config_.can_line_id);
    created_ = (motor_ != nullptr);
    return created_;
#endif
}

void MotorDriver::destroy() {
#if QRC_ENABLE_PHYARC_SDK
    if (motor_ != nullptr && board_.raw() != nullptr) {
        ScopedStdoutSilencer silencer;
        robot_destroy_motor(board_.raw(), motor_);
    }
#endif
    motor_ = nullptr;
    created_ = false;
}

bool MotorDriver::isCreated() const noexcept {
    return created_ && motor_ != nullptr;
}

bool MotorDriver::setControlMode(MotorControlMode mode) {
#if !QRC_ENABLE_PHYARC_SDK
    (void)mode;
    return false;
#else
    return isCreated() && robot_motor_set_control_mode(motor_, toSdkMode(mode)) != 0;
#endif
}

bool MotorDriver::setControlWord(ControlWord word) {
#if !QRC_ENABLE_PHYARC_SDK
    (void)word;
    return false;
#else
    return isCreated() && robot_motor_set_control_world(motor_, toSdkWord(word)) != 0;
#endif
}

bool MotorDriver::enable() {
    return setControlWord(ControlWord::ServoOn);
}

bool MotorDriver::disable() {
    return setControlWord(ControlWord::ServoOff);
}

bool MotorDriver::clearFault() {
    return setControlWord(ControlWord::ClearFault);
}

void MotorDriver::setPid(double pos_kp, double vel_kp, double vel_ki) {
#if QRC_ENABLE_PHYARC_SDK
    if (!isCreated()) {
        return;
    }
    robot_motor_set_pid(motor_, static_cast<float>(pos_kp), static_cast<float>(vel_kp), static_cast<float>(vel_ki));
#else
    (void)pos_kp;
    (void)vel_kp;
    (void)vel_ki;
#endif
}

void MotorDriver::setPd(double kp, double kd) {
#if QRC_ENABLE_PHYARC_SDK
    if (!isCreated()) {
        return;
    }
    robot_motor_set_pd(motor_, static_cast<float>(kp), static_cast<float>(kd));
#else
    (void)kp;
    (void)kd;
#endif
}

void MotorDriver::setPosition(double position_rad) {
#if QRC_ENABLE_PHYARC_SDK
    if (!isCreated()) {
        return;
    }
    robot_motor_set_pose(motor_, static_cast<float>(position_rad));
#else
    (void)position_rad;
#endif
}

void MotorDriver::setVelocity(double velocity_rad_s) {
#if QRC_ENABLE_PHYARC_SDK
    if (!isCreated()) {
        return;
    }
    robot_motor_set_vel(motor_, static_cast<float>(velocity_rad_s));
#else
    (void)velocity_rad_s;
#endif
}

void MotorDriver::setCurrent(double current_a) {
#if QRC_ENABLE_PHYARC_SDK
    if (!isCreated()) {
        return;
    }
    robot_motor_set_cur(motor_, static_cast<float>(current_a));
#else
    (void)current_a;
#endif
}

void MotorDriver::setPosVelCur(double position_rad, double velocity_rad_s, double current_a) {
#if QRC_ENABLE_PHYARC_SDK
    if (!isCreated()) {
        return;
    }
    robot_motor_set_pos(motor_, static_cast<float>(position_rad), static_cast<float>(velocity_rad_s), static_cast<float>(current_a));
#else
    (void)position_rad;
    (void)velocity_rad_s;
    (void)current_a;
#endif
}

void MotorDriver::flushCommand() {
#if QRC_ENABLE_PHYARC_SDK
    if (board_.raw() == nullptr) {
        return;
    }
    robot_motor_set_big_pose(board_.raw());
#endif
}

MotorTelemetry MotorDriver::readPvct() const {
    MotorTelemetry telemetry{};
#if QRC_ENABLE_PHYARC_SDK
    if (!isCreated()) {
        return telemetry;
    }

    float pos = 0.0f;
    float vel = 0.0f;
    float cur = 0.0f;
    float tor_l = 0.0f;
    float tor_e = 0.0f;
    robot_motor_get_PVCT(motor_, &pos, &vel, &cur, &tor_l, &tor_e);

    telemetry.position_rad = pos;
    telemetry.velocity_rad_s = vel;
    telemetry.current_a = cur;
    telemetry.torque_load_nm = tor_l;
    telemetry.torque_elec_nm = tor_e;
    telemetry.stamp = std::chrono::steady_clock::now();
#endif
    return telemetry;
}

MotorTelemetry MotorDriver::readPvctFast() const {
    MotorTelemetry telemetry{};
#if QRC_ENABLE_PHYARC_SDK
    if (!isCreated()) {
        return telemetry;
    }

    float pos = 0.0f;
    float vel = 0.0f;
    float cur = 0.0f;
    robot_motor_get_PVCTFast(motor_, &pos, &vel, &cur);

    telemetry.position_rad = pos;
    telemetry.velocity_rad_s = vel;
    telemetry.current_a = cur;
    telemetry.stamp = std::chrono::steady_clock::now();
#endif
    return telemetry;
}

int MotorDriver::readEncoder() const {
#if !QRC_ENABLE_PHYARC_SDK
    return 0;
#else
    if (!isCreated()) {
        return 0;
    }
    int encoder = 0;
    robot_motor_get_EncoderValue(motor_, &encoder);
    return encoder;
#endif
}

std::string MotorDriver::queryMotorModel() const {
#if !QRC_ENABLE_PHYARC_SDK
    return {};
#else
    if (!isCreated()) {
        return {};
    }
    std::array<char, 128> buffer{};
    const int ok = robot_motor_get_motor_model(motor_, buffer.data());
    return ok ? std::string(buffer.data()) : std::string{};
#endif
}

std::string MotorDriver::queryFirmwareVersion() const {
#if !QRC_ENABLE_PHYARC_SDK
    return {};
#else
    if (!isCreated()) {
        return {};
    }
    std::array<char, 128> buffer{};
    const int ok = robot_motor_get_motor_firmware_version(motor_, buffer.data());
    return ok ? std::string(buffer.data()) : std::string{};
#endif
}

bool MotorDriver::setZero() {
    return setControlWord(ControlWord::PositionSetZero);
}

bool MotorDriver::calibrateEncoder() {
    return setControlWord(ControlWord::CalibrateEncoder);
}

bool MotorDriver::setStrongDragging(ThetaSource source, double voltage) {
#if !QRC_ENABLE_PHYARC_SDK
    (void)source;
    (void)voltage;
    return false;
#else
    return isCreated() && robot_motor_set_StrongDragging(motor_, toSdkTheta(source), static_cast<float>(voltage)) != 0;
#endif
}

bool MotorDriver::getStrongDraggingMode(ThetaSource& source) const {
#if !QRC_ENABLE_PHYARC_SDK
    (void)source;
    return false;
#else
    if (!isCreated()) {
        return false;
    }
    unsigned int value = 0;
    if (!robot_motor_get_StrongDragging(motor_, &value)) {
        return false;
    }
    source = static_cast<ThetaSource>(value);
    return true;
#endif
}

const MotorConfig& MotorDriver::config() const noexcept {
    return config_;
}

}  // namespace qrc

