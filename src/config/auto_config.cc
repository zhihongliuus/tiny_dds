#include "include/tiny_dds/auto_config.h"

#include <iostream>

namespace tiny_dds {
namespace auto_config {

std::shared_ptr<AutoConfigLoader> AutoConfigLoader::Create() {
    return std::shared_ptr<AutoConfigLoader>(new AutoConfigLoader());
}

bool AutoConfigLoader::LoadFromFile(const std::string& file_path) {
    config::Config config;
    if (!config::LoadFromYamlFile(file_path, config)) {
        std::cerr << "Failed to load configuration from file: " << file_path << std::endl;
        return false;
    }
    
    // Validate the configuration
    if (!config::ValidateConfig(config)) {
        std::cerr << "Invalid configuration" << std::endl;
        return false;
    }
    
    return CreateEntities(config);
}

bool AutoConfigLoader::LoadFromString(const std::string& yaml_str) {
    config::Config config;
    if (!config::LoadFromYamlString(yaml_str, config)) {
        std::cerr << "Failed to load configuration from string" << std::endl;
        return false;
    }
    
    // Validate the configuration
    if (!config::ValidateConfig(config)) {
        std::cerr << "Invalid configuration" << std::endl;
        return false;
    }
    
    return CreateEntities(config);
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
    auto participant_it = participants_.find(participant_name);
    if (participant_it == participants_.end()) {
        return nullptr;
    }
    
    auto publisher_it = publishers_.find(EntityKey(participant_name, publisher_name));
    if (publisher_it != publishers_.end()) {
        return publisher_it->second;
    }
    
    return nullptr;
}

std::shared_ptr<Subscriber> AutoConfigLoader::GetSubscriber(
    const std::string& participant_name, const std::string& subscriber_name) const {
    auto participant_it = participants_.find(participant_name);
    if (participant_it == participants_.end()) {
        return nullptr;
    }
    
    auto subscriber_it = subscribers_.find(EntityKey(participant_name, subscriber_name));
    if (subscriber_it != subscribers_.end()) {
        return subscriber_it->second;
    }
    
    return nullptr;
}

std::shared_ptr<Topic> AutoConfigLoader::GetTopic(
    const std::string& participant_name, const std::string& topic_name) const {
    auto participant_it = participants_.find(participant_name);
    if (participant_it == participants_.end()) {
        return nullptr;
    }
    
    auto topic_it = topics_.find(EntityKey(participant_name, topic_name));
    if (topic_it != topics_.end()) {
        return topic_it->second;
    }
    
    return nullptr;
}

bool AutoConfigLoader::CreateEntities(const config::Config& config) {
    // Create domain participants
    for (const auto& participant_config : config.participants) {
        auto participant = DomainParticipant::Create(
            participant_config.domain_id, participant_config.name);
        
        if (!participant) {
            std::cerr << "Failed to create domain participant: " << participant_config.name << std::endl;
            return false;
        }
        
        participants_[participant_config.name] = participant;
        
        // Create topics
        for (const auto& topic_config : participant_config.topics) {
            auto topic = CreateTopic(participant, topic_config);
            if (!topic) {
                std::cerr << "Failed to create topic: " << topic_config.name << std::endl;
                return false;
            }
            
            topics_[EntityKey(participant_config.name, topic_config.name)] = topic;
        }
        
        // Create publishers
        for (const auto& publisher_config : participant_config.publishers) {
            auto publisher = participant->CreatePublisher();
            if (!publisher) {
                std::cerr << "Failed to create publisher: " << publisher_config.name << std::endl;
                return false;
            }
            
            publishers_[EntityKey(participant_config.name, publisher_config.name)] = publisher;
            
            // Associate topics with the publisher
            for (const auto& topic_name : publisher_config.topic_names) {
                auto topic_it = topics_.find(EntityKey(participant_config.name, topic_name));
                if (topic_it == topics_.end()) {
                    std::cerr << "Topic not found: " << topic_name << std::endl;
                    return false;
                }
            }
        }
        
        // Create subscribers
        for (const auto& subscriber_config : participant_config.subscribers) {
            auto subscriber = participant->CreateSubscriber();
            if (!subscriber) {
                std::cerr << "Failed to create subscriber: " << subscriber_config.name << std::endl;
                return false;
            }
            
            subscribers_[EntityKey(participant_config.name, subscriber_config.name)] = subscriber;
            
            // Associate topics with the subscriber
            for (const auto& topic_name : subscriber_config.topic_names) {
                auto topic_it = topics_.find(EntityKey(participant_config.name, topic_name));
                if (topic_it == topics_.end()) {
                    std::cerr << "Topic not found: " << topic_name << std::endl;
                    return false;
                }
            }
        }
    }
    
    return true;
}

std::shared_ptr<DomainParticipant> AutoConfigLoader::CreateParticipant(
    const config::DomainParticipantConfig& config) {
    return DomainParticipant::Create(config.domain_id, config.name);
}

std::shared_ptr<Publisher> AutoConfigLoader::CreatePublisher(
    std::shared_ptr<DomainParticipant> participant,
    const config::PublisherConfig& config) {
    return participant->CreatePublisher();
}

std::shared_ptr<Subscriber> AutoConfigLoader::CreateSubscriber(
    std::shared_ptr<DomainParticipant> participant,
    const config::SubscriberConfig& config) {
    return participant->CreateSubscriber();
}

std::shared_ptr<Topic> AutoConfigLoader::CreateTopic(
    std::shared_ptr<DomainParticipant> participant,
    const config::TopicConfig& config) {
    return participant->CreateTopic(config.name, config.type_name);
}

} // namespace auto_config
} // namespace tiny_dds 