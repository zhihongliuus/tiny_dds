#ifndef TINY_DDS_TRANSPORT_H_
#define TINY_DDS_TRANSPORT_H_

#include <cstddef>
#include <string>

#include "include/tiny_dds/transport_types.h"
#include "include/tiny_dds/types.h"

namespace tiny_dds {

/**
 * @brief Abstract base class for DDS transport implementations.
 * 
 * This class defines the interface that all transport implementations must follow.
 * It provides methods for sending and receiving data, as well as for managing
 * topic subscriptions and advertisements.
 */
class Transport {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~Transport() = default;
    
    /**
     * @brief Initializes the transport.
     * 
     * @return true if initialization was successful, false otherwise.
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Sends data to a topic.
     * 
     * @param topic_name The name of the topic.
     * @param data Pointer to the data to send.
     * @param size Size of the data in bytes.
     * @return true if the data was sent successfully, false otherwise.
     */
    virtual bool Send(const std::string& topic_name, const void* data, size_t size) = 0;
    
    /**
     * @brief Receives data from a topic.
     * 
     * @param topic_name The name of the topic.
     * @param buffer Pointer to the buffer to store the received data.
     * @param buffer_size Size of the buffer in bytes.
     * @param bytes_received Output parameter to store the number of bytes received.
     * @return true if data was received successfully, false otherwise.
     */
    virtual bool Receive(const std::string& topic_name, void* buffer, size_t buffer_size, 
                        size_t* bytes_received) = 0;
    
    /**
     * @brief Subscribes to a topic.
     * 
     * @param topic_name The name of the topic to subscribe to.
     * @return true if subscription was successful, false otherwise.
     */
    virtual bool Subscribe(const std::string& topic_name) = 0;
    
    /**
     * @brief Advertises a topic.
     * 
     * @param topic_name The name of the topic to advertise.
     * @return true if advertisement was successful, false otherwise.
     */
    virtual bool Advertise(const std::string& topic_name) = 0;
    
    /**
     * @brief Gets the type of this transport.
     * 
     * @return The transport type.
     */
    virtual TransportType GetType() const = 0;
};

} // namespace tiny_dds

#endif // TINY_DDS_TRANSPORT_H_ 