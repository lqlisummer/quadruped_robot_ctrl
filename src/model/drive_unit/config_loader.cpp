#include "model/drive_unit/config_loader.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>

namespace qrc {
namespace {

std::string trim(const std::string& text) {
    auto begin = text.begin();
    while (begin != text.end() && std::isspace(static_cast<unsigned char>(*begin))) {
        ++begin;
    }

    auto end = text.end();
    while (end != begin) {
        const auto prev = std::prev(end);
        if (!std::isspace(static_cast<unsigned char>(*prev))) {
            break;
        }
        end = prev;
    }
    return std::string(begin, end);
}

bool parseBool(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value == "1" || value == "true" || value == "yes" || value == "on";
}

}  // namespace

bool DriveUnitConfigLoader::load(const std::string& path, ParallelDriveUnitConfig& config, std::string* error) {
    std::ifstream in(path);
    if (!in) {
        if (error != nullptr) {
            *error = "failed to open stage2 config: " + path;
        }
        return false;
    }

    config = ParallelDriveUnitConfig{};

    std::string line;
    std::size_t line_no = 0;
    std::string list_key;

    while (std::getline(in, line)) {
        ++line_no;
        const auto comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        const std::string t = trim(line);
        if (t.empty()) {
            continue;
        }

        if (t == "motor_ids:" || t == "can_line_ids:" || t == "coupling_ratio:") {
            list_key = t.substr(0, t.size() - 1);
            continue;
        }

        if (!list_key.empty() && t.rfind("-", 0) == 0) {
            const std::string value = trim(t.substr(1));
            try {
                if (list_key == "motor_ids") {
                    config.motor_ids.push_back(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
                } else if (list_key == "can_line_ids") {
                    config.can_line_ids.push_back(static_cast<std::uint16_t>(std::stoul(value, nullptr, 0)));
                } else if (list_key == "coupling_ratio") {
                    config.coupling_ratio.push_back(std::stod(value));
                }
            } catch (const std::exception&) {
                if (error != nullptr) {
                    *error = "invalid list value at line " + std::to_string(line_no) + ": " + t;
                }
                return false;
            }
            continue;
        }

        list_key.clear();

        const auto sep = t.find(':');
        if (sep == std::string::npos) {
            continue;
        }

        const std::string key = trim(t.substr(0, sep));
        const std::string value = trim(t.substr(sep + 1));

        try {
            if (key == "name") {
                config.name = value;
            } else if (key == "allow_single_motor_degrade") {
                config.allow_single_motor_degrade = parseBool(value);
            }
        } catch (const std::exception&) {
            if (error != nullptr) {
                *error = "invalid scalar value at line " + std::to_string(line_no) + ": " + t;
            }
            return false;
        }
    }

    if (config.motor_ids.empty() || config.can_line_ids.empty() || config.coupling_ratio.empty()) {
        if (error != nullptr) {
            *error = "stage2 config missing required drive_unit arrays";
        }
        return false;
    }

    if (config.motor_ids.size() != config.can_line_ids.size() || config.motor_ids.size() != config.coupling_ratio.size()) {
        if (error != nullptr) {
            *error = "motor_ids / can_line_ids / coupling_ratio size mismatch";
        }
        return false;
    }

    return true;
}

}  // namespace qrc
