#ifndef TINY_DDS_TRANSPORT_UDP_TRANSPORT_H_
#define TINY_DDS_TRANSPORT_UDP_TRANSPORT_H_

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

#include "include/tiny_dds/transport.h"
#include "include/tiny_dds/transport_types.h"
#include "include/tiny_dds/types.h"

namespace tiny_dds::transport {

/**
 * @brief Implements transport over UDP for network communication.
 */
class UdpTransport : public Transport {
public:
    /**
     * @brief Creates a new UDP transport instance.
     * 
     * @param domain_id The domain ID for this transport.
     * @param participant_name The name of the participant using this transport.
     * @return A shared pointer to the created transport.
     */
    static auto Create(
        DomainId domain_id,
        const std::string& participant_name) -> std::shared_ptr<UdpTransport>;

    /**
     * @brief Destructor, cleans up all UDP sockets.
     */
    ~UdpTransport() override;
    
    /**
     * @brief Initializes the transport.
     * 
     * @return true if initialization was successful, false otherwise.
     */
    auto Initialize() -> bool override;
    
    /**
     * @brief Sends data to a topic.
     * 
     * @param topic_name The name of the topic.
     * @param data Pointer to the data to send.
     * @param size Size of the data in bytes.
     * @return true if the data was sent successfully, false otherwise.
     */
    auto Send(const std::string& topic_name, const void* data, size_t size) -> bool override;
    
    /**
     * @brief Receives data from a topic.
     * 
     * @param topic_name The name of the topic.
     * @param buffer Pointer to the buffer to store the received data.
     * @param buffer_size Size of the buffer in bytes.
     * @param bytes_received Output parameter to store the number of bytes received.
     * @return true if data was received successfully, false otherwise.
     */
    auto Receive(const std::string& topic_name, void* buffer, size_t buffer_size, 
                size_t* bytes_received) -> bool override;
    
    /**
     * @brief Subscribes to a topic.
     * 
     * @param topic_name The name of the topic to subscribe to.
     * @return true if subscription was successful, false otherwise.
     */
    auto Subscribe(const std::string& topic_name) -> bool override;
    
    /**
     * @brief Advertises a topic.
     * 
     * @param topic_name The name of the topic to advertise.
     * @return true if advertisement was successful, false otherwise.
     */
    auto Advertise(const std::string& topic_name) -> bool override;
    
    /**
     * @brief Gets the type of this transport.
     * 
     * @return The transport type.
     */
    auto GetType() const -> TransportType override { return TransportType::UDP; }

private:
    /**
     * @brief Constructor.
     * 
     * @param domain_id The domain ID for this transport.
     * @param participant_name The name of the participant using this transport.
     */
    UdpTransport(DomainId domain_id, std::string participant_name);

    /**
     * @brief Creates a UDP socket for a topic.
     * 
     * @param topic_name The topic name.
     * @return true if successful, false otherwise.
     */
    auto CreateSocket(const std::string& topic_name) -> bool;

    /**
     * @brief Connects to an existing UDP socket for a topic.
     * 
     * @param topic_name The topic name.
     * @return true if successful, false otherwise.
     */
    auto ConnectToSocket(const std::string& topic_name) -> bool;

    /**
     * @brief Closes a UDP socket.
     * 
     * @param topic_name The topic name.
     */
    void CloseSocket(const std::string& topic_name);

    /**
     * @brief Generates a UDP port for a topic.
     * 
     * @param topic_name The topic name.
     * @return int The UDP port.
     */
    auto GenerateUdpPort(const std::string& topic_name) -> int;

    /**
     * @brief Information about a UDP socket.
     */
    struct UdpSocketInfo {
        int socket_fd{-1};         // Socket file descriptor
        int port{0};               // UDP port
        std::string address;       // UDP address
        bool is_publisher{false};  // Whether this is a publisher socket
    };

    // Domain ID for this transport
    DomainId domain_id_;
    
    // Participant name
    std::string participant_name_;
    
    // Mutex for thread safety
    std::mutex mutex_;
    
    // Flag to indicate if the transport is initialized
    bool initialized_;

    // Map of UDP sockets by topic name
    std::unordered_map<std::string, UdpSocketInfo> udp_sockets_;
};

} // namespace tiny_dds::transport

#endif // TINY_DDS_TRANSPORT_UDP_TRANSPORT_H_ 