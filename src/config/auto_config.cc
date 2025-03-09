#include "include/tiny_dds/auto_config.h"

#include <iostream>

namespace tiny_dds {
namespace auto_config {

std::shared_ptr<AutoConfigLoader> AutoConfigLoader::Create() {
    return std::shared_ptr<AutoConfigLoader>(new AutoConfigLoader());
}

bool AutoConfigLoader::LoadFromFile(const std::string& file_path) {
    auto config = config::LoadFromFile(file_path);
    if (!config) {
        std::cerr << "Failed to load configuration from file: " << file_path << std::endl;
        return false;
    }
    
    if (!config::Validate(*config)) {
        std::cerr << "Invalid configuration in file: " << file_path << std::endl;
        return false;
    }
    
    return CreateEntities(*config);
}

bool AutoConfigLoader::LoadFromString(const std::string& yaml_str) {
    auto config = config::LoadFromString(yaml_str);
    if (!config) {
        std::cerr << "Failed to load configuration from string" << std::endl;
        return false;
    }
    
    if (!config::Validate(*config)) {
        std::cerr << "Invalid configuration in string" << std::endl;
        return false;
    }
    
    return CreateEntities(*config);
}

std::vector<std::shared_ptr<DomainParticipant>> AutoConfigLoader::GetParticipants() const {
    std::vector<std::shared_ptr<DomainParticipant>> result;
    for (const auto& pair : participants_) {
        result.push_back(pair.second);
    }
    return result;
}

std::shared_ptr<DomainParticipant> AutoConfigLoader::GetParticipant(const std::string& name) const {
    auto it = participants_.find(name);
    if (it != participants_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Publisher> AutoConfigLoader::GetPublisher(
    const std::string& participant_name, const std::string& publisher_name) const {
    std::string key = participant_name + ":" + publisher_name;
    auto it = publishers_.find(key);
    if (it != publishers_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Subscriber> AutoConfigLoader::GetSubscriber(
    const std::string& participant_name, const std::string& subscriber_name) const {
    std::string key = participant_name + ":" + subscriber_name;
    auto it = subscribers_.find(key);
    if (it != subscribers_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Topic> AutoConfigLoader::GetTopic(
    const std::string& participant_name, const std::string& topic_name) const {
    std::string key = participant_name + ":" + topic_name;
    auto it = topics_.find(key);
    if (it != topics_.end()) {
        return it->second;
    }
    return nullptr;
}

bool AutoConfigLoader::CreateEntities(const config::Config& config) {
    // Clear all existing entities
    participants_.clear();
    publishers_.clear();
    subscribers_.clear();
    topics_.clear();
    
    // Create domain participants
    for (const auto& participant_config : config.participants) {
        auto participant = CreateParticipant(participant_config);
        if (!participant) {
            std::cerr << "Failed to create domain participant: " << participant_config.name << std::endl;
            return false;
        }
        
        participants_[participant_config.name] = participant;
        
        // Create publishers
        for (const auto& publisher_config : participant_config.publishers) {
            auto publisher = CreatePublisher(participant, publisher_config);
            if (!publisher) {
                std::cerr << "Failed to create publisher: " << publisher_config.name << std::endl;
                return false;
            }
            
            std::string publisher_key = participant_config.name + ":" + publisher_config.name;
            publishers_[publisher_key] = publisher;
            
            // Create topics for this publisher
            for (const auto& topic_config : publisher_config.topics) {
                auto topic = CreateTopic(participant, topic_config);
                if (!topic) {
                    std::cerr << "Failed to create topic: " << topic_config.name << std::endl;
                    return false;
                }
                
                std::string topic_key = participant_config.name + ":" + topic_config.name;
                topics_[topic_key] = topic;
            }
        }
        
        // Create subscribers
        for (const auto& subscriber_config : participant_config.subscribers) {
            auto subscriber = CreateSubscriber(participant, subscriber_config);
            if (!subscriber) {
                std::cerr << "Failed to create subscriber: " << subscriber_config.name << std::endl;
                return false;
            }
            
            std::string subscriber_key = participant_config.name + ":" + subscriber_config.name;
            subscribers_[subscriber_key] = subscriber;
            
            // Create topics for this subscriber
            for (const auto& topic_config : subscriber_config.topics) {
                // Check if the topic already exists for this participant
                std::string topic_key = participant_config.name + ":" + topic_config.name;
                if (topics_.find(topic_key) == topics_.end()) {
                    auto topic = CreateTopic(participant, topic_config);
                    if (!topic) {
                        std::cerr << "Failed to create topic: " << topic_config.name << std::endl;
                        return false;
                    }
                    
                    topics_[topic_key] = topic;
                }
            }
        }
    }
    
    return true;
}

std::shared_ptr<DomainParticipant> AutoConfigLoader::CreateParticipant(
    const config::DomainParticipantConfig& config) {
    // Create a domain participant with the specified domain ID and name
    return DomainParticipant::Create(config.domain_id, config.name);
}

std::shared_ptr<Publisher> AutoConfigLoader::CreatePublisher(
    std::shared_ptr<DomainParticipant> participant,
    const config::PublisherConfig& config) {
    if (!participant) {
        return nullptr;
    }
    
    // Create a publisher
    return participant->CreatePublisher();
}

std::shared_ptr<Subscriber> AutoConfigLoader::CreateSubscriber(
    std::shared_ptr<DomainParticipant> participant,
    const config::SubscriberConfig& config) {
    if (!participant) {
        return nullptr;
    }
    
    // Create a subscriber
    return participant->CreateSubscriber();
}

std::shared_ptr<Topic> AutoConfigLoader::CreateTopic(
    std::shared_ptr<DomainParticipant> participant,
    const config::TopicConfig& config) {
    if (!participant) {
        return nullptr;
    }
    
    // Create a topic with the specified name and type
    return participant->CreateTopic(config.name, config.type);
}

} // namespace auto_config
} // namespace tiny_dds 