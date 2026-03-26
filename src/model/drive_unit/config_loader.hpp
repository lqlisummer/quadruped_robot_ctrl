#pragma once

#include <string>

#include "model/drive_unit/drive_unit_types.hpp"

namespace qrc {

class DriveUnitConfigLoader {
public:
    static bool load(const std::string& path, ParallelDriveUnitConfig& config, std::string* error = nullptr);
};

}  // namespace qrc
