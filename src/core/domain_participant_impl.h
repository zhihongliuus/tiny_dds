#ifndef TINY_DDS_CORE_DOMAIN_PARTICIPANT_IMPL_H_
#define TINY_DDS_CORE_DOMAIN_PARTICIPANT_IMPL_H_

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/synchronization/mutex.h"

#include "include/tiny_dds/domain_participant.h"

namespace tiny_dds {
namespace core {

// Forward declarations
class PublisherImpl;
class SubscriberImpl;
class TopicImpl;

/**
 * @brief Implementation of the DomainParticipant interface.
 */
class DomainParticipantImpl : public DomainParticipant,
                             public std::enable_shared_from_this<DomainParticipantImpl> {
public:
    /**
     * @brief Constructor for DomainParticipantImpl.
     * @param domain_id The domain identifier.
     * @param participant_name Name for the participant.
     */
    DomainParticipantImpl(DomainId domain_id, const std::string& participant_name);

    /**
     * @brief Destructor for DomainParticipantImpl.
     */
    ~DomainParticipantImpl() override;

    /**
     * @brief Creates a Publisher.
     * @return A shared pointer to the created Publisher.
     */
    std::shared_ptr<Publisher> CreatePublisher() override;

    /**
     * @brief Creates a Subscriber.
     * @return A shared pointer to the created Subscriber.
     */
    std::shared_ptr<Subscriber> CreateSubscriber() override;

    /**
     * @brief Creates a Topic with the given name and type.
     * @param topic_name The name of the topic.
     * @param type_name The name of the data type.
     * @return A shared pointer to the created Topic.
     */
    std::shared_ptr<Topic> CreateTopic(
        const std::string& topic_name, 
        const std::string& type_name) override;

    /**
     * @brief Gets the domain ID of this participant.
     * @return The domain ID.
     */
    DomainId GetDomainId() const override;

    /**
     * @brief Gets the name of this participant.
     * @return The participant name.
     */
    std::string GetName() const override;

private:
    // Domain ID for this participant
    DomainId domain_id_;

    // Name of this participant
    std::string participant_name_;

    // Mutex for thread safety
    mutable absl::Mutex mutex_;

    // Map of topics by name
    absl::flat_hash_map<std::string, std::shared_ptr<TopicImpl>> topics_;

    // List of publishers created by this participant
    std::vector<std::shared_ptr<PublisherImpl>> publishers_;

    // List of subscribers created by this participant
    std::vector<std::shared_ptr<SubscriberImpl>> subscribers_;
};

} // namespace core
} // namespace tiny_dds

#endif // TINY_DDS_CORE_DOMAIN_PARTICIPANT_IMPL_H_ 