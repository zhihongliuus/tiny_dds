#ifndef TINY_DDS_CORE_DATA_WRITER_IMPL_H_
#define TINY_DDS_CORE_DATA_WRITER_IMPL_H_

#include <memory>
#include <mutex>
#include <string>

#include "absl/synchronization/mutex.h"

#include "include/tiny_dds/data_writer.h"
#include "include/tiny_dds/types.h"

namespace tiny_dds {
namespace core {

// Forward declarations
class PublisherImpl;
class TopicImpl;

/**
 * @brief Implementation of the DataWriter interface.
 */
class DataWriterImpl : public tiny_dds::DataWriter,
                      public std::enable_shared_from_this<DataWriterImpl> {
public:
    /**
     * @brief Constructor for DataWriterImpl.
     * @param topic The topic to publish data on.
     * @param publisher The publisher that created this data writer.
     */
    DataWriterImpl(std::shared_ptr<tiny_dds::Topic> topic, 
                  std::shared_ptr<PublisherImpl> publisher);

    /**
     * @brief Destructor for DataWriterImpl.
     */
    ~DataWriterImpl() override;

    /**
     * @brief Writes a data sample to the topic.
     * @param data Pointer to the serialized message data.
     * @param size Size of the serialized data in bytes.
     * @return True if write was successful, false otherwise.
     */
    bool Write(const void* data, size_t size) override;

    /**
     * @brief Gets the topic associated with this DataWriter.
     * @return A shared pointer to the associated Topic.
     */
    std::shared_ptr<tiny_dds::Topic> GetTopic() const override;

    /**
     * @brief Gets the publication matched status.
     * @return The current publication matched status.
     */
    tiny_dds::PublicationMatchedStatus GetPublicationMatchedStatus() const override;

    /**
     * @brief Gets the publisher that created this data writer.
     * @return A shared pointer to the publisher.
     */
    std::shared_ptr<PublisherImpl> GetPublisher() const;

private:
    // The topic this data writer is associated with
    std::shared_ptr<tiny_dds::Topic> topic_;

    // The publisher that created this data writer
    std::shared_ptr<PublisherImpl> publisher_;

    // Publication matched status
    tiny_dds::PublicationMatchedStatus publication_matched_status_;

    // Mutex for thread safety
    mutable absl::Mutex mutex_;
};

} // namespace core
} // namespace tiny_dds

#endif // TINY_DDS_CORE_DATA_WRITER_IMPL_H_ 