#include "include/tiny_dds/config.h"

#include <fstream>
#include <iostream>

#include "yaml-cpp/yaml.h"

namespace tiny_dds {
namespace config {

// Forward declarations
static QosConfig ParseQosConfig(const YAML::Node& node);
static TopicConfig ParseTopicConfig(const YAML::Node& node);
static PublisherConfig ParsePublisherConfig(const YAML::Node& node);
static SubscriberConfig ParseSubscriberConfig(const YAML::Node& node);
static DomainParticipantConfig ParseDomainParticipantConfig(const YAML::Node& node);
static std::shared_ptr<Config> LoadFromYamlNode(const YAML::Node& root);

// Helper function to parse QoS settings
static QosConfig ParseQosConfig(const YAML::Node& node) {
    QosConfig qos;
    
    if (node["reliability"] && node["reliability"].IsScalar()) {
        qos.reliability = node["reliability"].as<std::string>();
    }
    
    if (node["durability"] && node["durability"].IsScalar()) {
        qos.durability = node["durability"].as<std::string>();
    }
    
    return qos;
}

// Parse a topic from YAML
static TopicConfig ParseTopicConfig(const YAML::Node& node) {
    TopicConfig topic;
    
    if (node["name"] && node["name"].IsScalar()) {
        topic.name = node["name"].as<std::string>();
    }
    
    if (node["type"] && node["type"].IsScalar()) {
        topic.type = node["type"].as<std::string>();
    }
    
    if (node["qos"]) {
        topic.qos = ParseQosConfig(node["qos"]);
    }
    
    return topic;
}

// Parse a publisher from YAML
static PublisherConfig ParsePublisherConfig(const YAML::Node& node) {
    PublisherConfig publisher;
    
    if (node["name"] && node["name"].IsScalar()) {
        publisher.name = node["name"].as<std::string>();
    }
    
    if (node["qos"]) {
        publisher.qos = ParseQosConfig(node["qos"]);
    }
    
    if (node["topics"] && node["topics"].IsSequence()) {
        for (const auto& topic_node : node["topics"]) {
            publisher.topics.push_back(ParseTopicConfig(topic_node));
        }
    }
    
    return publisher;
}

// Parse a subscriber from YAML
static SubscriberConfig ParseSubscriberConfig(const YAML::Node& node) {
    SubscriberConfig subscriber;
    
    if (node["name"] && node["name"].IsScalar()) {
        subscriber.name = node["name"].as<std::string>();
    }
    
    if (node["qos"]) {
        subscriber.qos = ParseQosConfig(node["qos"]);
    }
    
    if (node["topics"] && node["topics"].IsSequence()) {
        for (const auto& topic_node : node["topics"]) {
            subscriber.topics.push_back(ParseTopicConfig(topic_node));
        }
    }
    
    return subscriber;
}

// Parse a domain participant from YAML
static DomainParticipantConfig ParseDomainParticipantConfig(const YAML::Node& node) {
    DomainParticipantConfig participant;
    
    if (node["name"] && node["name"].IsScalar()) {
        participant.name = node["name"].as<std::string>();
    }
    
    if (node["domain_id"] && node["domain_id"].IsScalar()) {
        participant.domain_id = node["domain_id"].as<uint32_t>();
    }
    
    if (node["publishers"] && node["publishers"].IsSequence()) {
        for (const auto& pub_node : node["publishers"]) {
            participant.publishers.push_back(ParsePublisherConfig(pub_node));
        }
    }
    
    if (node["subscribers"] && node["subscribers"].IsSequence()) {
        for (const auto& sub_node : node["subscribers"]) {
            participant.subscribers.push_back(ParseSubscriberConfig(sub_node));
        }
    }
    
    return participant;
}

std::shared_ptr<Config> LoadFromFile(const std::string& file_path) {
    try {
        YAML::Node root = YAML::LoadFile(file_path);
        return LoadFromYamlNode(root);
    } catch (const YAML::Exception& e) {
        std::cerr << "Error loading YAML file: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<Config> LoadFromString(const std::string& yaml_str) {
    try {
        YAML::Node root = YAML::Load(yaml_str);
        return LoadFromYamlNode(root);
    } catch (const YAML::Exception& e) {
        std::cerr << "Error parsing YAML string: " << e.what() << std::endl;
        return nullptr;
    }
}

// Helper function to load from a YAML node
static std::shared_ptr<Config> LoadFromYamlNode(const YAML::Node& root) {
    auto config = std::make_shared<Config>();
    
    if (root["participants"] && root["participants"].IsSequence()) {
        for (const auto& participant_node : root["participants"]) {
            config->participants.push_back(ParseDomainParticipantConfig(participant_node));
        }
    }
    
    return config;
}

bool Validate(const Config& config) {
    // Check if there are any participants
    if (config.participants.empty()) {
        std::cerr << "Error: No domain participants defined in configuration." << std::endl;
        return false;
    }
    
    // Validate each participant
    for (const auto& participant : config.participants) {
        // Check if participant has a name
        if (participant.name.empty()) {
            std::cerr << "Error: Domain participant must have a name." << std::endl;
            return false;
        }
        
        // Validate publishers
        for (const auto& publisher : participant.publishers) {
            // Check if publisher has a name
            if (publisher.name.empty()) {
                std::cerr << "Error: Publisher in participant '" << participant.name 
                          << "' must have a name." << std::endl;
                return false;
            }
            
            // Check if publisher has topics
            if (publisher.topics.empty()) {
                std::cerr << "Warning: Publisher '" << publisher.name 
                          << "' in participant '" << participant.name 
                          << "' has no topics." << std::endl;
            }
            
            // Validate topics
            for (const auto& topic : publisher.topics) {
                // Check if topic has a name
                if (topic.name.empty()) {
                    std::cerr << "Error: Topic in publisher '" << publisher.name 
                              << "' must have a name." << std::endl;
                    return false;
                }
                
                // Check if topic has a type
                if (topic.type.empty()) {
                    std::cerr << "Error: Topic '" << topic.name 
                              << "' in publisher '" << publisher.name 
                              << "' must have a type." << std::endl;
                    return false;
                }
                
                // Validate QoS
                if (topic.qos.reliability != "BEST_EFFORT" && 
                    topic.qos.reliability != "RELIABLE") {
                    std::cerr << "Error: Topic '" << topic.name 
                              << "' has invalid reliability QoS: " << topic.qos.reliability 
                              << std::endl;
                    return false;
                }
                
                if (topic.qos.durability != "VOLATILE" && 
                    topic.qos.durability != "TRANSIENT_LOCAL" && 
                    topic.qos.durability != "TRANSIENT" && 
                    topic.qos.durability != "PERSISTENT") {
                    std::cerr << "Error: Topic '" << topic.name 
                              << "' has invalid durability QoS: " << topic.qos.durability 
                              << std::endl;
                    return false;
                }
            }
        }
        
        // Validate subscribers
        for (const auto& subscriber : participant.subscribers) {
            // Check if subscriber has a name
            if (subscriber.name.empty()) {
                std::cerr << "Error: Subscriber in participant '" << participant.name 
                          << "' must have a name." << std::endl;
                return false;
            }
            
            // Check if subscriber has topics
            if (subscriber.topics.empty()) {
                std::cerr << "Warning: Subscriber '" << subscriber.name 
                          << "' in participant '" << participant.name 
                          << "' has no topics." << std::endl;
            }
            
            // Validate topics
            for (const auto& topic : subscriber.topics) {
                // Check if topic has a name
                if (topic.name.empty()) {
                    std::cerr << "Error: Topic in subscriber '" << subscriber.name 
                              << "' must have a name." << std::endl;
                    return false;
                }
                
                // Check if topic has a type
                if (topic.type.empty()) {
                    std::cerr << "Error: Topic '" << topic.name 
                              << "' in subscriber '" << subscriber.name 
                              << "' must have a type." << std::endl;
                    return false;
                }
                
                // Validate QoS
                if (topic.qos.reliability != "BEST_EFFORT" && 
                    topic.qos.reliability != "RELIABLE") {
                    std::cerr << "Error: Topic '" << topic.name 
                              << "' has invalid reliability QoS: " << topic.qos.reliability 
                              << std::endl;
                    return false;
                }
                
                if (topic.qos.durability != "VOLATILE" && 
                    topic.qos.durability != "TRANSIENT_LOCAL" && 
                    topic.qos.durability != "TRANSIENT" && 
                    topic.qos.durability != "PERSISTENT") {
                    std::cerr << "Error: Topic '" << topic.name 
                              << "' has invalid durability QoS: " << topic.qos.durability 
                              << std::endl;
                    return false;
                }
            }
        }
    }
    
    return true;
}

} // namespace config
} // namespace tiny_dds 