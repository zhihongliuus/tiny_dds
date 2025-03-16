#ifndef TINY_DDS_TRANSPORT_TRANSPORT_MANAGER_H_
#define TINY_DDS_TRANSPORT_TRANSPORT_MANAGER_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "include/tiny_dds/transport.h"
#include "include/tiny_dds/transport_types.h"
#include "include/tiny_dds/types.h"
#include "src/transport/shared_memory_transport.h"
#include "src/transport/udp_transport.h"

namespace tiny_dds {
namespace transport {

/**
 * @brief Manages different transport implementations.
 */
class TransportManager {
 public:
  /**
   * @brief Creates a new transport manager.
   *
   * @return std::shared_ptr<TransportManager> The created manager.
   */
  static std::shared_ptr<TransportManager> Create();

  /**
   * @brief Sends data via the appropriate transport.
   *
   * @param domain_id The domain ID.
   * @param topic_name The topic name.
   * @param data The data to send.
   * @param size The size of the data.
   * @param transport_type The transport type to use.
   * @return true if successful, false otherwise.
   */
  bool Send(DomainId domain_id, const std::string& topic_name, const void* data, size_t size,
            TransportType transport_type = TransportType::UDP);

  /**
   * @brief Receives data via the appropriate transport.
   *
   * @param domain_id The domain ID.
   * @param topic_name The topic name.
   * @param buffer The buffer to store the data.
   * @param buffer_size The size of the buffer.
   * @param bytes_received The number of bytes received.
   * @param transport_type The transport type to use.
   * @return true if successful, false otherwise.
   */
  bool Receive(DomainId domain_id, const std::string& topic_name, void* buffer, size_t buffer_size,
               size_t* bytes_received, TransportType transport_type = TransportType::UDP);

  /**
   * @brief Creates a transport for a topic.
   *
   * @param domain_id The domain ID.
   * @param participant_name The participant name.
   * @param topic_name The topic name.
   * @param buffer_size The size of the buffer (for shared memory).
   * @param max_message_size The maximum message size (for shared memory).
   * @param transport_type The transport type to use.
   * @return true if successful, false otherwise.
   */
  bool CreateTransport(DomainId domain_id, const std::string& participant_name,
                       const std::string& topic_name, size_t buffer_size, size_t max_message_size,
                       TransportType transport_type = TransportType::UDP);

  /**
   * @brief Advertises a topic on the specified transport.
   *
   * @param domain_id The domain ID.
   * @param topic_name The topic name.
   * @param transport_type The transport type to use.
   * @return true if successful, false otherwise.
   */
  bool Advertise(DomainId domain_id, const std::string& topic_name,
                 TransportType transport_type = TransportType::UDP);

  /**
   * @brief Subscribes to a topic on the specified transport.
   *
   * @param domain_id The domain ID.
   * @param topic_name The topic name.
   * @param transport_type The transport type to use.
   * @return true if successful, false otherwise.
   */
  bool Subscribe(DomainId domain_id, const std::string& topic_name,
                 TransportType transport_type = TransportType::UDP);

 private:
  /**
   * @brief Constructor.
   */
  TransportManager();

  /**
   * @brief Gets the appropriate transport for the given type.
   *
   * @param domain_id The domain ID.
   * @param transport_type The transport type.
   * @return std::shared_ptr<Transport> The transport instance.
   */
  std::shared_ptr<Transport> GetTransport(DomainId domain_id, TransportType transport_type);

  // Map of domain ID to transport instances
  std::unordered_map<DomainId, std::shared_ptr<Transport>> udp_transports_;
  std::unordered_map<DomainId, std::shared_ptr<Transport>> shared_memory_transports_;

  // Mutex for thread safety
  std::mutex mutex_;
};

}  // namespace transport
}  // namespace tiny_dds

#endif  // TINY_DDS_TRANSPORT_TRANSPORT_MANAGER_H_