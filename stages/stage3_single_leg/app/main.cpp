#include "common/stage_manifest.hpp"

int main() {
    qrc::print_stage_header("Stage 3", "Single Leg");
    qrc::print_named_item("Goal", "Close joint-space and task-space control for one leg.");
    qrc::print_named_item("Default Config", "stages/stage3_single_leg/config/default.yaml");
    qrc::print_named_item("Handoff Gate", "Reliable kinematics, trajectory tracking, and leg-level protection.");
    qrc::print_named_item("Key Modules", "");
    qrc::print_module_item("src/model/leg");
    qrc::print_module_item("src/control/leg");
    qrc::print_module_item("src/model/drive_unit");
    qrc::print_module_item("src/safety");
    return 0;
}

