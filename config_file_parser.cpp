#include "config_file_parser.hpp"
#include <fstream>
#include <spdlog/spdlog.h>

Configuration::Configuration(const std::filesystem::path &config_path,
                             const SectionKeyPairToConfigLogic &section_key_to_config_logic)
    : config_path(expand_tilde(config_path)), section_key_to_config_logic(section_key_to_config_logic) {
    parse_config_file();
    apply_config_logic();
}

// Parses the configuration file
void Configuration::parse_config_file() {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        spdlog::error("Unable to open config file: {}", config_path.string());
        return;
    }

    std::string line, current_section;
    while (std::getline(file, line)) {
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        line = trim(line);
        if (line.empty())
            continue;

        if (line.front() == '[' && line.back() == ']') {
            current_section = trim(line.substr(1, line.length() - 2));
        } else {
            size_t delimiter_pos = line.find('=');
            if (delimiter_pos == std::string::npos) {
                spdlog::warn("Invalid line in config file: {}", line);
                continue;
            }

            std::string key = trim(line.substr(0, delimiter_pos));
            std::string value = trim(line.substr(delimiter_pos + 1));

            config_data[current_section][key] = value;
        }
    }
}

// Applies configuration logic for each section-key-value pair
void Configuration::apply_config_logic() {
    for (const auto &[section, key_values] : config_data) {
        for (const auto &[key, value] : key_values) {
            auto logic_it = section_key_to_config_logic.find({section, key});
            if (logic_it != section_key_to_config_logic.end()) {
                logic_it->second(value);
            }
        }
    }
}

// Trims leading and trailing whitespace from a string
std::string Configuration::trim(const std::string &str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}
