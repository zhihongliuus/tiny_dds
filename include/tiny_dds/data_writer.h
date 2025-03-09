#ifndef TINY_DDS_DATA_WRITER_H_
#define TINY_DDS_DATA_WRITER_H_

#include <memory>
#include <string>

#include "include/tiny_dds/types.h"

// Forward declarations
namespace tiny_dds {
class Topic;

/**
 * @brief DataWriter is the interface for writing data to a topic.
 * 
 * Publishers create DataWriter objects to publish data of a specific type.
 */
class DataWriter {
public:
    virtual ~DataWriter() = default;

    /**
     * @brief Writes a data sample to the topic.
     * @param data Pointer to the serialized message data.
     * @param size Size of the serialized data in bytes.
     * @return True if write was successful, false otherwise.
     */
    virtual bool Write(const void* data, size_t size) = 0;

    /**
     * @brief Gets the topic associated with this DataWriter.
     * @return A shared pointer to the associated Topic.
     */
    virtual std::shared_ptr<Topic> GetTopic() const = 0;

    /**
     * @brief Gets the publication matched status.
     * @return The current publication matched status.
     */
    virtual PublicationMatchedStatus GetPublicationMatchedStatus() const = 0;
};

} // namespace tiny_dds

#endif // TINY_DDS_DATA_WRITER_H_ 