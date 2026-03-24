#pragma once

#include <string>

#include "model/motor/motor_types.hpp"

namespace qrc {

class ConfigLoader {
public:
    static bool load(const std::string& path, BoardConfig& board, MotorConfig& motor, std::string* error = nullptr);
};

}  // namespace qrc

