#ifndef TINY_DDS_DATA_READER_H_
#define TINY_DDS_DATA_READER_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "include/tiny_dds/types.h"

// Forward declarations
namespace tiny_dds {
class Topic;

/**
 * @brief Callback function type for data reception.
 * 
 * @param data Pointer to the serialized message data.
 * @param size Size of the serialized data in bytes.
 * @param info Sample information.
 */
using DataReaderCallback = std::function<void(const void* data, size_t size, const SampleInfo& info)>;

/**
 * @brief Callback function type for data reception with domain and topic information.
 * 
 * @param domain_id The domain ID the data was received on.
 * @param topic_name The name of the topic the data was received on.
 * @param data Pointer to the serialized message data.
 * @param size Size of the serialized data in bytes.
 */
using DataCallback = std::function<void(DomainId domain_id, const std::string& topic_name, 
                                       const void* data, size_t size)>;

/**
 * @brief DataReader is the interface for reading data from a topic.
 * 
 * Subscribers create DataReader objects to receive data of a specific type.
 */
class DataReader {
public:
    virtual ~DataReader() = default;

    /**
     * @brief Reads the next available data sample.
     * @param[out] buffer Buffer to store the data.
     * @param[in] buffer_size Size of the buffer.
     * @param[out] info Sample information.
     * @return Number of bytes read, or -1 if no data is available.
     */
    virtual int32_t Read(void* buffer, size_t buffer_size, SampleInfo& info) = 0;

    /**
     * @brief Takes the next available data sample (removes it from the reader's queue).
     * @param[out] buffer Buffer to store the data.
     * @param[in] buffer_size Size of the buffer.
     * @param[out] info Sample information.
     * @return Number of bytes read, or -1 if no data is available.
     */
    virtual int32_t Take(void* buffer, size_t buffer_size, SampleInfo& info) = 0;

    /**
     * @brief Sets a callback function to be called when data is received.
     * @param callback The callback function.
     */
    virtual void SetDataReceivedCallback(DataReaderCallback callback) = 0;
    
    /**
     * @brief Sets a callback function to be called when data is received, with domain and topic information.
     * @param callback The callback function.
     */
    virtual void SetDataCallback(DataCallback callback) = 0;

    /**
     * @brief Gets the topic associated with this DataReader.
     * @return A shared pointer to the associated Topic.
     */
    virtual std::shared_ptr<Topic> GetTopic() const = 0;

    /**
     * @brief Gets the subscription matched status.
     * @return The current subscription matched status.
     */
    virtual SubscriptionMatchedStatus GetSubscriptionMatchedStatus() const = 0;
};

} // namespace tiny_dds

#endif // TINY_DDS_DATA_READER_H_ 