#ifndef TINY_DDS_TRANSPORT_TRANSPORT_MANAGER_H_
#define TINY_DDS_TRANSPORT_TRANSPORT_MANAGER_H_

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/synchronization/mutex.h"

#include "include/tiny_dds/types.h"

namespace tiny_dds {
namespace transport {

/**
 * @brief Callback function type for data reception.
 * 
 * @param domain_id The domain ID the data was received on.
 * @param topic_name The name of the topic the data was received on.
 * @param data Pointer to the serialized message data.
 * @param size Size of the serialized data in bytes.
 */
using DataReceivedCallback = std::function<void(
    DomainId domain_id, 
    const std::string& topic_name, 
    const void* data, 
    size_t size)>;

/**
 * @brief Manager for transport-level communication.
 * 
 * This class handles the low-level communication between DDS participants,
 * including discovery and data transfer.
 */
class TransportManager {
public:
    /**
     * @brief Gets the singleton instance of the TransportManager.
     * @return A reference to the TransportManager instance.
     */
    static TransportManager& GetInstance();

    /**
     * @brief Registers a publisher for a topic in a domain.
     * @param domain_id The domain ID.
     * @param topic_name The name of the topic.
     * @param type_name The name of the data type.
     * @return True if registration was successful, false otherwise.
     */
    bool RegisterPublisher(
        DomainId domain_id, 
        const std::string& topic_name, 
        const std::string& type_name);

    /**
     * @brief Unregisters a publisher for a topic in a domain.
     * @param domain_id The domain ID.
     * @param topic_name The name of the topic.
     */
    void UnregisterPublisher(
        DomainId domain_id, 
        const std::string& topic_name);

    /**
     * @brief Registers a subscriber for a topic in a domain.
     * @param domain_id The domain ID.
     * @param topic_name The name of the topic.
     * @param type_name The name of the data type.
     * @param callback The callback function to be called when data is received.
     * @return True if registration was successful, false otherwise.
     */
    bool RegisterSubscriber(
        DomainId domain_id, 
        const std::string& topic_name, 
        const std::string& type_name,
        DataReceivedCallback callback);

    /**
     * @brief Unregisters a subscriber for a topic in a domain.
     * @param domain_id The domain ID.
     * @param topic_name The name of the topic.
     */
    void UnregisterSubscriber(
        DomainId domain_id, 
        const std::string& topic_name);

    /**
     * @brief Publishes data to a topic in a domain.
     * @param domain_id The domain ID.
     * @param topic_name The name of the topic.
     * @param data Pointer to the serialized message data.
     * @param size Size of the serialized data in bytes.
     * @return True if publish was successful, false otherwise.
     */
    bool Publish(
        DomainId domain_id, 
        const std::string& topic_name, 
        const void* data, 
        size_t size);

private:
    // Private constructor to enforce singleton pattern
    TransportManager();
    
    // Private destructor
    ~TransportManager();
    
    // Deleted copy constructor and assignment operator
    TransportManager(const TransportManager&) = delete;
    TransportManager& operator=(const TransportManager&) = delete;

    // Structure to hold publisher information
    struct PublisherInfo {
        std::string type_name;
    };
    
    // Structure to hold subscriber information
    struct SubscriberInfo {
        std::string type_name;
        DataReceivedCallback callback;
    };
    
    // Map of publishers by domain ID and topic name
    absl::flat_hash_map<
        DomainId, 
        absl::flat_hash_map<std::string, PublisherInfo>> publishers_;
    
    // Map of subscribers by domain ID and topic name
    absl::flat_hash_map<
        DomainId, 
        absl::flat_hash_map<std::string, SubscriberInfo>> subscribers_;
    
    // Mutex for thread safety
    mutable absl::Mutex mutex_;
};

} // namespace transport
} // namespace tiny_dds

#endif // TINY_DDS_TRANSPORT_TRANSPORT_MANAGER_H_ 