#include "common/stage_manifest.hpp"

int main() {
    qrc::print_stage_header("Stage 4", "Quadruped");
    qrc::print_named_item("Goal", "Coordinate four legs into a full quadruped controller.");
    qrc::print_named_item("Default Config", "stages/stage4_quadruped/config/default.yaml");
    qrc::print_named_item("Handoff Gate", "Stable gait scheduling, body coordination, and machine-level safety.");
    qrc::print_named_item("Key Modules", "");
    qrc::print_module_item("src/model/quadruped");
    qrc::print_module_item("src/control/quadruped");
    qrc::print_module_item("src/runtime");
    qrc::print_module_item("src/safety");
    return 0;
}

