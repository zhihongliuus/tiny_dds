#ifndef TINY_DDS_CONFIG_H_
#define TINY_DDS_CONFIG_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "include/tiny_dds/transport_types.h"
#include "include/tiny_dds/types.h"
#include "yaml-cpp/yaml.h"

namespace tiny_dds {
namespace config {

/**
 * @brief Quality of Service configuration for DDS entities.
 */
struct QosConfig {
    ReliabilityKind reliability = ReliabilityKind::BEST_EFFORT;
    DurabilityKind durability = DurabilityKind::VOLATILE;
    // Other QoS settings can be added here
};

/**
 * @brief Transport configuration for DDS entities.
 */
struct TransportConfig {
    TransportType type = TransportType::UDP;
    
    // Shared memory specific configuration
    size_t buffer_size = 1024 * 1024;  // 1MB default
    size_t max_message_size = 64 * 1024;  // 64KB default
    
    // UDP specific configuration (can be expanded as needed)
    std::string address = "127.0.0.1";
    int port = 0;  // 0 means auto-assign
};

/**
 * @brief Configuration for a Topic.
 */
struct TopicConfig {
    std::string name;
    std::string type_name;
    QosConfig qos;
};

/**
 * @brief Configuration for a Publisher.
 */
struct PublisherConfig {
    std::string name;
    QosConfig qos;
    TransportConfig transport;
    std::vector<std::string> topic_names;  // Names of associated topics
};

/**
 * @brief Configuration for a Subscriber.
 */
struct SubscriberConfig {
    std::string name;
    QosConfig qos;
    TransportConfig transport;
    std::vector<std::string> topic_names;  // Names of associated topics
};

/**
 * @brief Configuration for a Domain Participant.
 */
struct DomainParticipantConfig {
    std::string name;
    DomainId domain_id;
    std::vector<TopicConfig> topics;
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
 * @brief Loads configuration from a YAML node.
 * 
 * @param node The YAML node.
 * @param config The config to populate.
 * @return True if loading was successful, false otherwise.
 */
bool LoadFromYamlNode(const YAML::Node& node, Config& config);

/**
 * @brief Loads configuration from a YAML file.
 * 
 * @param file_path Path to the YAML file.
 * @param config The config to populate.
 * @return True if loading was successful, false otherwise.
 */
bool LoadFromYamlFile(const std::string& file_path, Config& config);

/**
 * @brief Loads configuration from a YAML string.
 * 
 * @param yaml_string The YAML content as a string.
 * @param config The config to populate.
 * @return True if loading was successful, false otherwise.
 */
bool LoadFromYamlString(const std::string& yaml_string, Config& config);

/**
 * @brief Validates a configuration.
 * 
 * @param config The configuration to validate.
 * @return True if the configuration is valid, false otherwise.
 */
bool ValidateConfig(const Config& config);

} // namespace config
} // namespace tiny_dds

#endif // TINY_DDS_CONFIG_H_ 