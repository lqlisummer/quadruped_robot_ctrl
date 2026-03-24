#include "common/stage_manifest.hpp"

int main() {
    qrc::print_stage_header("Stage 1", "Single Motor");
    qrc::print_named_item("Goal", "Bring up one motor and close the minimum control loop.");
    qrc::print_named_item("Default Config", "stages/stage1_single_motor/config/default.yaml");
    qrc::print_named_item("Handoff Gate", "Stable single-motor control with feedback logging and safety limits.");
    qrc::print_named_item("Key Modules", "");
    qrc::print_module_item("src/platform/phyarc");
    qrc::print_module_item("src/model/motor");
    qrc::print_module_item("src/control/motor");
    qrc::print_module_item("src/safety");
    return 0;
}

