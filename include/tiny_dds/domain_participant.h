#ifndef TINY_DDS_DOMAIN_PARTICIPANT_H_
#define TINY_DDS_DOMAIN_PARTICIPANT_H_

#include <memory>
#include <string>
#include <vector>

#include "include/tiny_dds/types.h"

// Forward declarations
namespace tiny_dds {
class Publisher;
class Subscriber;
class Topic;

/**
 * @brief DomainParticipant is the entry point to the DDS communication.
 * 
 * It represents an entity that can publish and subscribe to topics within a 
 * DDS domain. A domain is a logical partition of the communication space.
 */
class DomainParticipant {
public:
    /**
     * @brief Creates a new DomainParticipant.
     * @param domain_id The domain identifier.
     * @param participant_name Optional name for the participant.
     * @return A shared pointer to the created DomainParticipant.
     */
    static std::shared_ptr<DomainParticipant> Create(
        DomainId domain_id,
        const std::string& participant_name = "");

    virtual ~DomainParticipant() = default;

    /**
     * @brief Creates a Publisher.
     * @return A shared pointer to the created Publisher.
     */
    virtual std::shared_ptr<Publisher> CreatePublisher() = 0;

    /**
     * @brief Creates a Subscriber.
     * @return A shared pointer to the created Subscriber.
     */
    virtual std::shared_ptr<Subscriber> CreateSubscriber() = 0;

    /**
     * @brief Creates a Topic with the given name and type.
     * @param topic_name The name of the topic.
     * @param type_name The name of the data type.
     * @return A shared pointer to the created Topic.
     */
    virtual std::shared_ptr<Topic> CreateTopic(
        const std::string& topic_name, 
        const std::string& type_name) = 0;

    /**
     * @brief Gets the domain ID of this participant.
     * @return The domain ID.
     */
    virtual DomainId GetDomainId() const = 0;

    /**
     * @brief Gets the name of this participant.
     * @return The participant name.
     */
    virtual std::string GetName() const = 0;
};

} // namespace tiny_dds

#endif // TINY_DDS_DOMAIN_PARTICIPANT_H_ 