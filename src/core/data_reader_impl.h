#ifndef TINY_DDS_CORE_DATA_READER_IMPL_H_
#define TINY_DDS_CORE_DATA_READER_IMPL_H_

#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "absl/synchronization/mutex.h"
#include "absl/synchronization/notification.h"

#include "include/tiny_dds/data_reader.h"
#include "include/tiny_dds/types.h"

namespace tiny_dds {
namespace core {

// Forward declarations
class SubscriberImpl;

/**
 * @brief Implementation of the DataReader interface.
 */
class DataReaderImpl : public tiny_dds::DataReader,
                      public std::enable_shared_from_this<DataReaderImpl> {
public:
    /**
     * @brief Constructor for DataReaderImpl.
     * @param topic The topic to subscribe to.
     * @param subscriber The subscriber that created this data reader.
     */
    DataReaderImpl(std::shared_ptr<tiny_dds::Topic> topic, 
                  std::shared_ptr<SubscriberImpl> subscriber);

    /**
     * @brief Destructor for DataReaderImpl.
     */
    ~DataReaderImpl() override;

    /**
     * @brief Reads the next available data sample.
     * @param[out] buffer Buffer to store the data.
     * @param[in] buffer_size Size of the buffer.
     * @param[out] info Sample information.
     * @return Number of bytes read, or -1 if no data is available.
     */
    int32_t Read(void* buffer, size_t buffer_size, tiny_dds::SampleInfo& info) override;

    /**
     * @brief Takes the next available data sample (removes it from the reader's queue).
     * @param[out] buffer Buffer to store the data.
     * @param[in] buffer_size Size of the buffer.
     * @param[out] info Sample information.
     * @return Number of bytes read, or -1 if no data is available.
     */
    int32_t Take(void* buffer, size_t buffer_size, tiny_dds::SampleInfo& info) override;

    /**
     * @brief Sets a callback function to be called when data is received.
     * @param callback The callback function.
     */
    void SetDataReceivedCallback(tiny_dds::DataReaderCallback callback) override;

    /**
     * @brief Gets the topic associated with this DataReader.
     * @return A shared pointer to the associated Topic.
     */
    std::shared_ptr<tiny_dds::Topic> GetTopic() const override;

    /**
     * @brief Gets the subscription matched status.
     * @return The current subscription matched status.
     */
    tiny_dds::SubscriptionMatchedStatus GetSubscriptionMatchedStatus() const override;

    /**
     * @brief Gets the subscriber that created this data reader.
     * @return A shared pointer to the subscriber.
     */
    std::shared_ptr<SubscriberImpl> GetSubscriber() const;

    /**
     * @brief Called when data is received from a publisher.
     * @param data Pointer to the serialized message data.
     * @param size Size of the serialized data in bytes.
     */
    void OnDataReceived(const void* data, size_t size);

private:
    // The topic this data reader is associated with
    std::shared_ptr<tiny_dds::Topic> topic_;

    // The subscriber that created this data reader
    std::shared_ptr<SubscriberImpl> subscriber_;

    // Queue of received data samples
    struct DataSample {
        std::vector<uint8_t> data;
        tiny_dds::SampleInfo info;
    };
    std::deque<DataSample> samples_;

    // Callback function for data reception
    tiny_dds::DataReaderCallback data_received_callback_;

    // Subscription matched status
    tiny_dds::SubscriptionMatchedStatus subscription_matched_status_;

    // Mutex for thread safety
    mutable absl::Mutex mutex_;

    // Notification for waiting on data
    absl::Notification data_available_;
};

} // namespace core
} // namespace tiny_dds

#endif // TINY_DDS_CORE_DATA_READER_IMPL_H_ 