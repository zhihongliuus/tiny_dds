#ifndef TINY_DDS_CONFIG_H_
#define TINY_DDS_CONFIG_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace tiny_dds {
namespace config {

/**
 * @brief Configuration for QoS (Quality of Service) settings.
 */
struct QosConfig {
    std::string reliability = "BEST_EFFORT";  // BEST_EFFORT or RELIABLE
    std::string durability = "VOLATILE";      // VOLATILE, TRANSIENT_LOCAL, TRANSIENT, or PERSISTENT
    
    // Additional QoS parameters can be added here
};

/**
 * @brief Configuration for a Topic.
 */
struct TopicConfig {
    std::string name;
    std::string type;
    QosConfig qos;
};

/**
 * @brief Configuration for a Publisher.
 */
struct PublisherConfig {
    std::string name;
    QosConfig qos;
    std::vector<TopicConfig> topics;
};

/**
 * @brief Configuration for a Subscriber.
 */
struct SubscriberConfig {
    std::string name;
    QosConfig qos;
    std::vector<TopicConfig> topics;
};

/**
 * @brief Configuration for a DomainParticipant.
 */
struct DomainParticipantConfig {
    std::string name;
    uint32_t domain_id;
    std::vector<PublisherConfig> publishers;
    std::vector<SubscriberConfig> subscribers;
};

/**
 * @brief Root configuration structure.
 */
struct Config {
    std::vector<DomainParticipantConfig> participants;
};

/**
 * @brief Loads a configuration from a YAML file.
 * 
 * @param file_path Path to the YAML configuration file.
 * @return The loaded configuration, or nullptr if loading failed.
 */
std::shared_ptr<Config> LoadFromFile(const std::string& file_path);

/**
 * @brief Loads a configuration from a YAML string.
 * 
 * @param yaml_str YAML configuration string.
 * @return The loaded configuration, or nullptr if loading failed.
 */
std::shared_ptr<Config> LoadFromString(const std::string& yaml_str);

/**
 * @brief Validates a configuration.
 * 
 * @param config The configuration to validate.
 * @return true if the configuration is valid, false otherwise.
 */
bool Validate(const Config& config);

} // namespace config
} // namespace tiny_dds

#endif // TINY_DDS_CONFIG_H_ 