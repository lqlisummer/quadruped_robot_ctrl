#pragma once

#include <iostream>
#include <string_view>

namespace qrc {

inline void print_stage_header(std::string_view stage_id, std::string_view title) {
    std::cout << "== " << stage_id << " | " << title << " ==\n";
}

inline void print_named_item(std::string_view key, std::string_view value) {
    std::cout << key << ": " << value << '\n';
}

inline void print_module_item(std::string_view module_name) {
    std::cout << "  - " << module_name << '\n';
}

}  // namespace qrc

