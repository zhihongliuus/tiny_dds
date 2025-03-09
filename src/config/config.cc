#include "include/tiny_dds/config.h"

#include <fstream>
#include <iostream>

#include "yaml-cpp/yaml.h"

namespace tiny_dds {
namespace config {

// Forward declarations
static bool ParseQosConfig(const YAML::Node& node, QosConfig& qos);
static bool ParseTransportConfig(const YAML::Node& node, TransportConfig& transport);
static bool ParseTopicConfig(const YAML::Node& node, TopicConfig& topic);
static bool ParsePublisherConfig(const YAML::Node& node, PublisherConfig& publisher, const std::vector<TopicConfig>& topics);
static bool ParseSubscriberConfig(const YAML::Node& node, SubscriberConfig& subscriber, const std::vector<TopicConfig>& topics);
static bool ParseDomainParticipantConfig(const YAML::Node& node, DomainParticipantConfig& participant);

// Helper functions to convert string to enum
static ReliabilityKind StringToReliabilityKind(const std::string& str) {
    if (str == "RELIABLE") {
        return ReliabilityKind::RELIABLE;
    }
    return ReliabilityKind::BEST_EFFORT;  // Default
}

static DurabilityKind StringToDurabilityKind(const std::string& str) {
    if (str == "TRANSIENT_LOCAL") {
        return DurabilityKind::TRANSIENT_LOCAL;
    } else if (str == "TRANSIENT") {
        return DurabilityKind::TRANSIENT;
    } else if (str == "PERSISTENT") {
        return DurabilityKind::PERSISTENT;
    }
    return DurabilityKind::VOLATILE;  // Default
}

// Helper function to parse QoS settings
static bool ParseQosConfig(const YAML::Node& node, QosConfig& qos) {
    if (node["reliability"] && node["reliability"].IsScalar()) {
        qos.reliability = StringToReliabilityKind(node["reliability"].as<std::string>());
    }
    
    if (node["durability"] && node["durability"].IsScalar()) {
        qos.durability = StringToDurabilityKind(node["durability"].as<std::string>());
    }
    
    return true;
}

// Helper function to parse Transport settings
static bool ParseTransportConfig(const YAML::Node& node, TransportConfig& transport) {
    if (node["type"] && node["type"].IsScalar()) {
        transport.type = StringToTransportType(node["type"].as<std::string>());
    }
    
    if (node["buffer_size"] && node["buffer_size"].IsScalar()) {
        transport.buffer_size = node["buffer_size"].as<size_t>();
    }
    
    if (node["max_message_size"] && node["max_message_size"].IsScalar()) {
        transport.max_message_size = node["max_message_size"].as<size_t>();
    }
    
    if (node["address"] && node["address"].IsScalar()) {
        transport.address = node["address"].as<std::string>();
    }
    
    if (node["port"] && node["port"].IsScalar()) {
        transport.port = node["port"].as<int>();
    }
    
    return true;
}

// Parse a topic from YAML
static bool ParseTopicConfig(const YAML::Node& node, TopicConfig& topic) {
    if (node["name"] && node["name"].IsScalar()) {
        topic.name = node["name"].as<std::string>();
    } else {
        std::cerr << "Error: Topic must have a name." << std::endl;
        return false;
    }
    
    if (node["type_name"] && node["type_name"].IsScalar()) {
        topic.type_name = node["type_name"].as<std::string>();
    } else {
        std::cerr << "Error: Topic must have a type_name." << std::endl;
        return false;
    }
    
    if (node["qos"]) {
        if (!ParseQosConfig(node["qos"], topic.qos)) {
            return false;
        }
    }
    
    return true;
}

// Parse a publisher from YAML
static bool ParsePublisherConfig(const YAML::Node& node, PublisherConfig& publisher, const std::vector<TopicConfig>& topics) {
    if (node["name"] && node["name"].IsScalar()) {
        publisher.name = node["name"].as<std::string>();
    } else {
        std::cerr << "Error: Publisher must have a name." << std::endl;
        return false;
    }
    
    if (node["qos"]) {
        if (!ParseQosConfig(node["qos"], publisher.qos)) {
            return false;
        }
    }
    
    if (node["transport"]) {
        if (!ParseTransportConfig(node["transport"], publisher.transport)) {
            return false;
        }
    }
    
    if (node["topic_names"] && node["topic_names"].IsSequence()) {
        for (const auto& topic_name_node : node["topic_names"]) {
            if (topic_name_node.IsScalar()) {
                std::string topic_name = topic_name_node.as<std::string>();
                
                // Verify the topic exists
                bool found = false;
                for (const auto& topic : topics) {
                    if (topic.name == topic_name) {
                        found = true;
                        break;
                    }
                }
                
                if (found) {
                    publisher.topic_names.push_back(topic_name);
                } else {
                    std::cerr << "Error: Publisher references topic '" << topic_name 
                              << "' which is not defined." << std::endl;
                    return false;
                }
            }
        }
    }
    
    if (publisher.topic_names.empty()) {
        std::cerr << "Warning: Publisher '" << publisher.name << "' has no topics." << std::endl;
    }
    
    return true;
}

// Parse a subscriber from YAML
static bool ParseSubscriberConfig(const YAML::Node& node, SubscriberConfig& subscriber, const std::vector<TopicConfig>& topics) {
    if (node["name"] && node["name"].IsScalar()) {
        subscriber.name = node["name"].as<std::string>();
    } else {
        std::cerr << "Error: Subscriber must have a name." << std::endl;
        return false;
    }
    
    if (node["qos"]) {
        if (!ParseQosConfig(node["qos"], subscriber.qos)) {
            return false;
        }
    }
    
    if (node["transport"]) {
        if (!ParseTransportConfig(node["transport"], subscriber.transport)) {
            return false;
        }
    }
    
    if (node["topic_names"] && node["topic_names"].IsSequence()) {
        for (const auto& topic_name_node : node["topic_names"]) {
            if (topic_name_node.IsScalar()) {
                std::string topic_name = topic_name_node.as<std::string>();
                
                // Verify the topic exists
                bool found = false;
                for (const auto& topic : topics) {
                    if (topic.name == topic_name) {
                        found = true;
                        break;
                    }
                }
                
                if (found) {
                    subscriber.topic_names.push_back(topic_name);
                } else {
                    std::cerr << "Error: Subscriber references topic '" << topic_name 
                              << "' which is not defined." << std::endl;
                    return false;
                }
            }
        }
    }
    
    if (subscriber.topic_names.empty()) {
        std::cerr << "Warning: Subscriber '" << subscriber.name << "' has no topics." << std::endl;
    }
    
    return true;
}

// Parse a domain participant from YAML
static bool ParseDomainParticipantConfig(const YAML::Node& node, DomainParticipantConfig& participant) {
    if (node["name"] && node["name"].IsScalar()) {
        participant.name = node["name"].as<std::string>();
    } else {
        std::cerr << "Error: Domain participant must have a name." << std::endl;
        return false;
    }
    
    if (node["domain_id"] && node["domain_id"].IsScalar()) {
        participant.domain_id = node["domain_id"].as<DomainId>();
    } else {
        std::cerr << "Error: Domain participant must have a domain_id." << std::endl;
        return false;
    }
    
    // Parse topics first so publishers and subscribers can reference them
    if (node["topics"] && node["topics"].IsSequence()) {
        for (const auto& topic_node : node["topics"]) {
            TopicConfig topic;
            if (ParseTopicConfig(topic_node, topic)) {
                participant.topics.push_back(topic);
            } else {
                return false;
            }
        }
    }
    
    if (node["publishers"] && node["publishers"].IsSequence()) {
        for (const auto& pub_node : node["publishers"]) {
            PublisherConfig publisher;
            if (ParsePublisherConfig(pub_node, publisher, participant.topics)) {
                participant.publishers.push_back(publisher);
            } else {
                return false;
            }
        }
    }
    
    if (node["subscribers"] && node["subscribers"].IsSequence()) {
        for (const auto& sub_node : node["subscribers"]) {
            SubscriberConfig subscriber;
            if (ParseSubscriberConfig(sub_node, subscriber, participant.topics)) {
                participant.subscribers.push_back(subscriber);
            } else {
                return false;
            }
        }
    }
    
    return true;
}

bool LoadFromYamlNode(const YAML::Node& node, Config& config) {
    if (!node.IsMap()) {
        std::cerr << "Error: Root YAML node must be a map." << std::endl;
        return false;
    }
    
    if (node["participants"] && node["participants"].IsSequence()) {
        for (const auto& participant_node : node["participants"]) {
            DomainParticipantConfig participant;
            if (ParseDomainParticipantConfig(participant_node, participant)) {
                config.participants.push_back(participant);
            } else {
                return false;
            }
        }
    } else {
        std::cerr << "Error: No participants found in configuration." << std::endl;
        return false;
    }
    
    return true;
}

bool LoadFromYamlFile(const std::string& file_path, Config& config) {
    try {
        YAML::Node root = YAML::LoadFile(file_path);
        return LoadFromYamlNode(root, config);
    } catch (const YAML::Exception& e) {
        std::cerr << "Error loading YAML file: " << e.what() << std::endl;
        return false;
    }
}

bool LoadFromYamlString(const std::string& yaml_string, Config& config) {
    try {
        YAML::Node root = YAML::Load(yaml_string);
        return LoadFromYamlNode(root, config);
    } catch (const YAML::Exception& e) {
        std::cerr << "Error parsing YAML string: " << e.what() << std::endl;
        return false;
    }
}

bool ValidateConfig(const Config& config) {
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
        
        // Check for topics
        if (participant.topics.empty()) {
            std::cerr << "Warning: Participant '" << participant.name 
                      << "' has no topics defined." << std::endl;
        }
        
        // Validate each topic
        for (const auto& topic : participant.topics) {
            if (topic.name.empty()) {
                std::cerr << "Error: Topic in participant '" << participant.name 
                          << "' must have a name." << std::endl;
                return false;
            }
            
            if (topic.type_name.empty()) {
                std::cerr << "Error: Topic '" << topic.name 
                          << "' in participant '" << participant.name 
                          << "' must have a type_name." << std::endl;
                return false;
            }
        }
        
        // Validate publishers
        for (const auto& publisher : participant.publishers) {
            if (publisher.name.empty()) {
                std::cerr << "Error: Publisher in participant '" << participant.name 
                          << "' must have a name." << std::endl;
                return false;
            }
            
            if (publisher.topic_names.empty()) {
                std::cerr << "Warning: Publisher '" << publisher.name 
                          << "' in participant '" << participant.name 
                          << "' has no topics." << std::endl;
            }
        }
        
        // Validate subscribers
        for (const auto& subscriber : participant.subscribers) {
            if (subscriber.name.empty()) {
                std::cerr << "Error: Subscriber in participant '" << participant.name 
                          << "' must have a name." << std::endl;
                return false;
            }
            
            if (subscriber.topic_names.empty()) {
                std::cerr << "Warning: Subscriber '" << subscriber.name 
                          << "' in participant '" << participant.name 
                          << "' has no topics." << std::endl;
            }
        }
    }
    
    return true;
}

} // namespace config
} // namespace tiny_dds 