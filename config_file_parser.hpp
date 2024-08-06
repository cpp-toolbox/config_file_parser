#ifndef CONFIG_FILE_PARSER_HPP
#define CONFIG_FILE_PARSER_HPP

#include <string>
#include <unordered_map>

using KeyToValueMap = std::unordered_map<std::string, std::string>;
using ParsedConfigFile = std::unordered_map<std::string, KeyToValueMap>;
// Function to load configuration from a file
ParsedConfigFile parse_config_file(const std::string &filename);

#endif // CONFIG_FILE_PARSER_HPP
