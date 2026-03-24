#include "common/stage_manifest.hpp"

int main() {
    qrc::print_stage_header("Stage 2", "Parallel Drive Unit");
    qrc::print_named_item("Goal", "Abstract a motor group into one parallel drive unit.");
    qrc::print_named_item("Default Config", "stages/stage2_parallel_drive_unit/config/default.yaml");
    qrc::print_named_item("Handoff Gate", "Synchronized actuation, equivalent output tracking, and fault isolation.");
    qrc::print_named_item("Key Modules", "");
    qrc::print_module_item("src/model/drive_unit");
    qrc::print_module_item("src/control/drive_unit");
    qrc::print_module_item("src/platform/phyarc");
    qrc::print_module_item("src/safety");
    return 0;
}

