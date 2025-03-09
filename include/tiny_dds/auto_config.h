#ifndef TINY_DDS_AUTO_CONFIG_H_
#define TINY_DDS_AUTO_CONFIG_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "include/tiny_dds/config.h"
#include "include/tiny_dds/domain_participant.h"
#include "include/tiny_dds/publisher.h"
#include "include/tiny_dds/subscriber.h"
#include "include/tiny_dds/topic.h"

namespace tiny_dds {
namespace auto_config {

// Define a composite key for entity maps
struct EntityKey {
    std::string participant_name;
    std::string entity_name;
    
    EntityKey(const std::string& p_name, const std::string& e_name)
        : participant_name(p_name), entity_name(e_name) {}
    
    bool operator==(const EntityKey& other) const {
        return participant_name == other.participant_name && 
               entity_name == other.entity_name;
    }
};

// Hash function for EntityKey
struct EntityKeyHash {
    std::size_t operator()(const EntityKey& key) const {
        return std::hash<std::string>()(key.participant_name) ^ 
               std::hash<std::string>()(key.entity_name);
    }
};

/**
 * @brief A class that manages DDS entities created from a configuration.
 */
class AutoConfigLoader {
public:
    /**
     * @brief Creates an AutoConfigLoader instance.
     * @return A shared pointer to the created AutoConfigLoader.
     */
    static std::shared_ptr<AutoConfigLoader> Create();
    
    /**
     * @brief Loads DDS entities from a YAML configuration file.
     * @param file_path Path to the YAML configuration file.
     * @return true if the loading was successful, false otherwise.
     */
    bool LoadFromFile(const std::string& file_path);
    
    /**
     * @brief Loads DDS entities from a YAML configuration string.
     * @param yaml_str YAML configuration string.
     * @return true if the loading was successful, false otherwise.
     */
    bool LoadFromString(const std::string& yaml_str);
    
    /**
     * @brief Gets all domain participants.
     * @return A vector of domain participants.
     */
    std::vector<std::shared_ptr<DomainParticipant>> GetParticipants() const;
    
    /**
     * @brief Gets a domain participant by name.
     * @param name The name of the domain participant.
     * @return A shared pointer to the domain participant, or nullptr if not found.
     */
    std::shared_ptr<DomainParticipant> GetParticipant(const std::string& name) const;
    
    /**
     * @brief Gets a publisher by participant and publisher name.
     * @param participant_name The name of the domain participant.
     * @param publisher_name The name of the publisher.
     * @return A shared pointer to the publisher, or nullptr if not found.
     */
    std::shared_ptr<Publisher> GetPublisher(
        const std::string& participant_name, const std::string& publisher_name) const;
    
    /**
     * @brief Gets a subscriber by participant and subscriber name.
     * @param participant_name The name of the domain participant.
     * @param subscriber_name The name of the subscriber.
     * @return A shared pointer to the subscriber, or nullptr if not found.
     */
    std::shared_ptr<Subscriber> GetSubscriber(
        const std::string& participant_name, const std::string& subscriber_name) const;
    
    /**
     * @brief Gets a topic by participant and topic name.
     * @param participant_name The name of the domain participant.
     * @param topic_name The name of the topic.
     * @return A shared pointer to the topic, or nullptr if not found.
     */
    std::shared_ptr<Topic> GetTopic(
        const std::string& participant_name, const std::string& topic_name) const;

private:
    // Private constructor
    AutoConfigLoader() = default;
    
    // Creates DDS entities from a configuration
    bool CreateEntities(const config::Config& config);
    
    // Creates a domain participant
    std::shared_ptr<DomainParticipant> CreateParticipant(
        const config::DomainParticipantConfig& config);
    
    // Creates a publisher
    std::shared_ptr<Publisher> CreatePublisher(
        std::shared_ptr<DomainParticipant> participant,
        const config::PublisherConfig& config);
    
    // Creates a subscriber
    std::shared_ptr<Subscriber> CreateSubscriber(
        std::shared_ptr<DomainParticipant> participant,
        const config::SubscriberConfig& config);
    
    // Creates a topic
    std::shared_ptr<Topic> CreateTopic(
        std::shared_ptr<DomainParticipant> participant,
        const config::TopicConfig& config);
    
    // Maps to store created entities
    std::unordered_map<std::string, std::shared_ptr<DomainParticipant>> participants_;
    std::unordered_map<EntityKey, std::shared_ptr<Publisher>, EntityKeyHash> publishers_;
    std::unordered_map<EntityKey, std::shared_ptr<Subscriber>, EntityKeyHash> subscribers_;
    std::unordered_map<EntityKey, std::shared_ptr<Topic>, EntityKeyHash> topics_;
};

} // namespace auto_config
} // namespace tiny_dds

#endif // TINY_DDS_AUTO_CONFIG_H_ 