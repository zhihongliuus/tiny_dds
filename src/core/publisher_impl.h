#ifndef TINY_DDS_CORE_PUBLISHER_IMPL_H_
#define TINY_DDS_CORE_PUBLISHER_IMPL_H_

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/synchronization/mutex.h"

#include "include/tiny_dds/publisher.h"

namespace tiny_dds {
namespace core {

// Forward declarations
class DataWriterImpl;
class DomainParticipantImpl;

/**
 * @brief Implementation of the Publisher interface.
 */
class PublisherImpl : public tiny_dds::Publisher,
                     public std::enable_shared_from_this<PublisherImpl> {
public:
    /**
     * @brief Constructor for PublisherImpl.
     * @param participant The domain participant that created this publisher.
     */
    explicit PublisherImpl(std::shared_ptr<DomainParticipantImpl> participant);

    /**
     * @brief Destructor for PublisherImpl.
     */
    ~PublisherImpl() override;

    /**
     * @brief Creates a DataWriter for a specific topic.
     * @param topic The topic to publish data on.
     * @return A shared pointer to the created DataWriter.
     */
    std::shared_ptr<tiny_dds::DataWriter> CreateDataWriter(
        std::shared_ptr<tiny_dds::Topic> topic) override;

    /**
     * @brief Gets the domain participant that created this publisher.
     * @return A shared pointer to the domain participant.
     */
    std::shared_ptr<DomainParticipantImpl> GetParticipant() const;

private:
    // The domain participant that created this publisher
    std::shared_ptr<DomainParticipantImpl> participant_;

    // Map of data writers by topic name
    absl::flat_hash_map<std::string, std::shared_ptr<DataWriterImpl>> data_writers_;

    // Mutex for thread safety
    mutable absl::Mutex mutex_;
};

} // namespace core
} // namespace tiny_dds

#endif // TINY_DDS_CORE_PUBLISHER_IMPL_H_ 