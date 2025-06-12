#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "sbpt_generated_includes.hpp"

#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Class to parse a configuration file and apply logic based on
 * section-key pairs.
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
     * @brief Constructs the Configuration object, parses the file, and applies
     * configuration logic.
     *
     * @param config_path Path to the configuration file.
     * @param config_logic Map of section-key pairs to their corresponding logic
     * functions.
     */
    Configuration(const std::filesystem::path &config_path, const SectionKeyPairToConfigLogic &config_logic);

    /**
     * @brief Reloads the configuration from file, discarding any in-memory
     * changes.
     */
    void reload_config();

    // Live configuration modification methods

    /**
     * @brief Sets a configuration value and applies associated logic if present.
     *
     * @param section The section name
     * @param key The key name
     * @param value The value to set
     * @return true if successful, false if config logic failed
     */
    bool set_value(const std::string &section, const std::string &key, const std::string &value);

    /**
     * @brief Gets a configuration value.
     *
     * @param section The section name
     * @param key The key name
     * @return The value if found, std::nullopt otherwise
     */
    std::optional<std::string> get_value(const std::string &section, const std::string &key);

    /**
     * @brief Removes a configuration value.
     *
     * @param section The section name
     * @param key The key name
     * @return true if the value was removed, false if it didn't exist
     */
    bool remove_value(const std::string &section, const std::string &key);

    // Query methods

    /**
     * @brief Checks if a section exists.
     *
     * @param section The section name
     * @return true if section exists
     */
    bool has_section(const std::string &section) const;

    /**
     * @brief Checks if a specific key exists in a section.
     *
     * @param section The section name
     * @param key The key name
     * @return true if the key exists in the section
     */
    bool has_value(const std::string &section, const std::string &key);

    /**
     * @brief Gets all section names.
     *
     * @return Vector of section names
     */
    std::vector<std::string> get_sections() const;

    /**
     * @brief Gets all keys in a section.
     *
     * @param section The section name
     * @return Vector of key names in the section
     */
    std::vector<std::string> get_keys(const std::string &section);

    // File operations

    /**
     * @brief Saves the current configuration to the original file.
     *
     * @return true if successful
     */
    bool save_to_file();

    /**
     * @brief Saves the current configuration to a specified file.
     *
     * @param path The file path to save to
     * @return true if successful
     */
    bool save_to_file(const std::filesystem::path &path);

    /**
     * @brief Creates a backup of the current configuration file.
     *
     * @param backup_path The path where to save the backup
     * @return true if successful
     */
    bool backup_config(const std::filesystem::path &backup_path);

    /**
     * @brief Applies config logic for a specific key.
     *
     * @param section The section name
     * @param key The key name
     */
    void apply_config_logic_for_key(const std::string &section, const std::string &key);

    void apply_config_logic();

  private:
    std::filesystem::path config_path;
    SectionKeyPairToConfigLogic section_key_to_config_logic;
    ConfigData section_to_key_to_value;
    ConsoleLogger console_logger;

    void parse_config_file();
    static std::string trim(const std::string &str);
};

#endif // CONFIGURATION_HPP
