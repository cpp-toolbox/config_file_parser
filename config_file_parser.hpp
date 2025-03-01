#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <unordered_map>
#include <functional>
#include <filesystem>

#include "sbpt_generated_includes.hpp"

/**
 * @brief Class to parse a configuration file and apply logic based on section-key pairs.
 */
class Configuration {
  public:
    using SectionKeyPair = std::pair<std::string, std::string>;
    using ConfigLogic = std::function<void(const std::string)>;

    /**
     * @brief Hash function for ConfigKey (pair of strings).
     */
    struct PairHash {
        PairHash() = default;

        size_t operator()(const SectionKeyPair &pair) const {
            return std::hash<std::string>()(pair.first) ^ (std::hash<std::string>()(pair.second) << 1);
        }
    };

    using SectionKeyPairToConfigLogic = std::unordered_map<SectionKeyPair, ConfigLogic, PairHash>;
    using ConfigData = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;

    /**
     * @brief Constructs the Configuration object, parses the file, and applies configuration logic.
     *
     * @param config_path Path to the configuration file.
     * @param section_key_to_config_logic Map of section-key pairs to their corresponding logic functions.
     */
    Configuration(const std::filesystem::path &config_path,
                  const SectionKeyPairToConfigLogic &section_key_to_config_logic);

    void reload_config();

  private:
    std::filesystem::path config_path;
    SectionKeyPairToConfigLogic section_key_to_config_logic;
    ConfigData config_data;

    void parse_config_file();
    void apply_config_logic();
    static std::string trim(const std::string &str);
};

#endif // CONFIGURATION_HPP
