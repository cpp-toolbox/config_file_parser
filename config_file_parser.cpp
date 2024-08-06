#include "config_file_parser.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <spdlog/spdlog.h>

// Helper function to trim whitespace from a string
std::string trim(const std::string &str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Function to load configuration from a file
std::unordered_map<std::string, std::unordered_map<std::string, std::string>>
parse_config_file(const std::string &filename) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> config_map;
    std::ifstream file(filename);
    if (!file.is_open()) {
        spdlog::error("Unable to open config file: {}", filename);
        return config_map;
    }

    std::string line;
    std::string current_section;
    while (std::getline(file, line)) {
        // Remove comments and trim whitespace
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        // Check for section headers
        if (line.front() == '[' && line.back() == ']') {
            current_section = trim(line.substr(1, line.length() - 2));
            config_map[current_section] = {}; // Initialize section
        } else {
            size_t delimiter_pos = line.find('=');
            if (delimiter_pos == std::string::npos) {
                spdlog::warn("Invalid line in config file: {}", line);
                continue;
            }

            std::string key = trim(line.substr(0, delimiter_pos));
            std::string value = trim(line.substr(delimiter_pos + 1));

            if (current_section.empty()) {
                spdlog::warn("Key-value pair outside of section: {} = {}", key, value);
                continue;
            }

            config_map[current_section][key] = value;
        }
    }

    return config_map;
}
