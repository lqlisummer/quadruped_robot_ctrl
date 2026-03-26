#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "common/stage_manifest.hpp"
#include "model/drive_unit/config_loader.hpp"
#include "model/drive_unit/parallel_drive_unit.hpp"

namespace qrc {
namespace {

constexpr const char* kDefaultConfigPath = "stages/stage2_parallel_drive_unit/config/default.yaml";

void print_usage() {
    std::cout
        << "qrc_stage2_parallel_drive_unit --config <path> <command> [args]\n\n"
        << "Commands:\n"
        << "  manifest\n"
        << "  dump-config\n"
        << "  info\n"
        << "  monitor [count] [period_ms] [fast]\n"
        << "  enable\n"
        << "  disable\n"
        << "  clear-fault\n";
}

const char* to_string(DriveUnitState state) {
    switch (state) {
        case DriveUnitState::Normal:
            return "normal";
        case DriveUnitState::Degraded:
            return "degraded";
        case DriveUnitState::Fault:
        default:
            return "fault";
    }
}

void print_manifest() {
    qrc::print_stage_header("Stage 2", "Parallel Drive Unit");
    qrc::print_named_item("Goal", "Abstract a motor group into one parallel drive unit.");
    qrc::print_named_item("Default Config", kDefaultConfigPath);
    qrc::print_named_item("Handoff Gate", "Synchronized actuation, equivalent output tracking, and fault isolation.");
    qrc::print_named_item("Key Modules", "");
    qrc::print_module_item("src/model/drive_unit");
    qrc::print_module_item("src/control/drive_unit");
    qrc::print_module_item("src/platform/phyarc");
    qrc::print_module_item("src/safety");
}

void print_config(const ParallelDriveUnitConfig& cfg) {
    std::cout << "drive_unit.name=" << cfg.name << "\n";
    std::cout << "drive_unit.allow_single_motor_degrade=" << (cfg.allow_single_motor_degrade ? "true" : "false") << "\n";
    for (std::size_t i = 0; i < cfg.motor_ids.size(); ++i) {
        std::cout << "drive_unit.member[" << i << "]="
                  << "motor_id=" << cfg.motor_ids[i]
                  << ", can_line_id=" << cfg.can_line_ids[i]
                  << ", coupling=" << cfg.coupling_ratio[i] << "\n";
    }
}

void print_sample(const DriveUnitTelemetry& sample) {
    std::cout << "state=" << to_string(sample.state) << " active=" << sample.active_member_count << "\n";
    std::cout << std::fixed << std::setprecision(6)
              << "eq.pos=" << sample.equivalent.position_rad
              << " eq.vel=" << sample.equivalent.velocity_rad_s
              << " eq.cur=" << sample.equivalent.current_a
              << " eq.tor_l=" << sample.equivalent.torque_load_nm
              << " eq.tor_e=" << sample.equivalent.torque_elec_nm << "\n";

    for (const auto& member : sample.members) {
        std::cout << "  member motor=" << member.motor_id
                  << " line=" << member.can_line_id
                  << " active=" << (member.active ? "true" : "false")
                  << " pos=" << member.telemetry.position_rad
                  << " vel=" << member.telemetry.velocity_rad_s
                  << " cur=" << member.telemetry.current_a
                  << " enc=" << member.telemetry.encoder_value
                  << "\n";
    }
}

int print_command_result(bool ok) {
    if (ok) {
        std::cout << "ok\n";
        return 0;
    }
    std::cout << "failed\n";
    return 4;
}

}  // namespace
}  // namespace qrc

int main(int argc, char** argv) {
    using namespace qrc;

    std::string config_path = kDefaultConfigPath;
    std::vector<std::string> args(argv + 1, argv + argc);

    for (auto it = args.begin(); it != args.end();) {
        if (*it == "--config") {
            if (std::next(it) == args.end()) {
                std::cerr << "missing value for --config\n";
                return 1;
            }
            config_path = *std::next(it);
            it = args.erase(it, std::next(it, 2));
        } else if (*it == "--help" || *it == "-h") {
            print_usage();
            return 0;
        } else {
            ++it;
        }
    }

    const std::string command = args.empty() ? "manifest" : args[0];

    if (command == "manifest") {
        print_manifest();
        return 0;
    }

    ParallelDriveUnitConfig drive_cfg;
    std::string error;
    if (!DriveUnitConfigLoader::load(config_path, drive_cfg, &error)) {
        std::cerr << error << "\n";
        return 2;
    }

    if (command == "dump-config") {
        print_config(drive_cfg);
        return 0;
    }

    BoardConfig board_cfg;
    MotorConfig motor_template;
    ParallelDriveUnit unit(board_cfg, drive_cfg, motor_template);

    if (!unit.initialize()) {
        std::cerr << "failed to initialize parallel drive unit\n";
        std::cerr << "hint: build on Linux control host with -DQRC_ENABLE_PHYARC_SDK=ON and ensure board network is reachable\n";
        return 3;
    }

    if (command == "info") {
        const auto sample = unit.sample(false);
        print_sample(sample);
        return 0;
    }

    if (command == "clear-fault") {
        return print_command_result(unit.clearFault());
    }

    if (command == "enable") {
        return print_command_result(unit.enable());
    }

    if (command == "disable") {
        return print_command_result(unit.disable());
    }

    if (command == "monitor") {
        const int count = args.size() >= 2 ? std::stoi(args[1]) : 10;
        const int period_ms = args.size() >= 3 ? std::stoi(args[2]) : 100;
        const bool fast_feedback = args.size() >= 4 ? (args[3] == "fast") : false;

        for (int i = 0; i < count; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(period_ms));
            print_sample(unit.sample(fast_feedback));
        }
        return 0;
    }

    std::cerr << "unknown command: " << command << "\n";
    print_usage();
    return 1;
}
