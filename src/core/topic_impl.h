#ifndef TINY_DDS_CORE_TOPIC_IMPL_H_
#define TINY_DDS_CORE_TOPIC_IMPL_H_

#include <memory>
#include <string>

#include "absl/synchronization/mutex.h"

#include "include/tiny_dds/topic.h"

namespace tiny_dds {
namespace core {

// Forward declarations
class DomainParticipantImpl;

/**
 * @brief Implementation of the Topic interface.
 */
class TopicImpl : public tiny_dds::Topic,
                 public std::enable_shared_from_this<TopicImpl> {
public:
    /**
     * @brief Constructor for TopicImpl.
     * @param topic_name The name of the topic.
     * @param type_name The name of the data type.
     * @param participant The domain participant that created this topic.
     */
    TopicImpl(const std::string& topic_name, 
             const std::string& type_name,
             std::shared_ptr<DomainParticipantImpl> participant);

    /**
     * @brief Destructor for TopicImpl.
     */
    ~TopicImpl() override;

    /**
     * @brief Gets the name of this topic.
     * @return The topic name.
     */
    std::string GetName() const override;

    /**
     * @brief Gets the type name associated with this topic.
     * @return The type name.
     */
    std::string GetTypeName() const override;

    /**
     * @brief Gets the domain participant that created this topic.
     * @return A shared pointer to the domain participant.
     */
    std::shared_ptr<DomainParticipantImpl> GetParticipant() const;

private:
    // The name of this topic
    std::string topic_name_;

    // The name of the data type associated with this topic
    std::string type_name_;

    // The domain participant that created this topic
    std::shared_ptr<DomainParticipantImpl> participant_;

    // Mutex for thread safety
    mutable absl::Mutex mutex_;
};

} // namespace core
} // namespace tiny_dds

#endif // TINY_DDS_CORE_TOPIC_IMPL_H_ 