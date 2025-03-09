#ifndef TINY_DDS_SHARED_MEMORY_TRANSPORT_H_
#define TINY_DDS_SHARED_MEMORY_TRANSPORT_H_

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "include/tiny_dds/transport.h"
#include "include/tiny_dds/transport_types.h"

namespace tiny_dds {
namespace transport {

/**
 * @brief Shared memory transport implementation.
 * 
 * This class implements a shared memory transport for DDS communication.
 * It allows processes on the same machine to communicate efficiently
 * by sharing memory regions instead of using network sockets.
 */
class SharedMemoryTransport : public Transport {
public:
    /**
     * @brief Creates a shared memory transport instance.
     * 
     * @param domain_id The domain ID for this transport.
     * @param participant_name The name of the participant using this transport.
     * @param buffer_size The size of the shared memory buffer in bytes.
     * @param max_message_size The maximum size of a single message in bytes.
     * @return A shared pointer to the created transport.
     */
    static std::shared_ptr<SharedMemoryTransport> Create(
        DomainId domain_id,
        const std::string& participant_name,
        size_t buffer_size = 1024 * 1024,  // 1MB default
        size_t max_message_size = 64 * 1024  // 64KB default
    );
    
    /**
     * @brief Destructor.
     */
    ~SharedMemoryTransport() override;
    
    /**
     * @brief Initializes the transport.
     * 
     * @return true if initialization was successful, false otherwise.
     */
    bool Initialize() override;
    
    /**
     * @brief Sends data to a topic.
     * 
     * @param topic_name The name of the topic.
     * @param data Pointer to the data to send.
     * @param size Size of the data in bytes.
     * @return true if the data was sent successfully, false otherwise.
     */
    bool Send(const std::string& topic_name, const void* data, size_t size) override;
    
    /**
     * @brief Receives data from a topic.
     * 
     * @param topic_name The name of the topic.
     * @param buffer Pointer to the buffer to store the received data.
     * @param buffer_size Size of the buffer in bytes.
     * @param bytes_received Output parameter to store the number of bytes received.
     * @return true if data was received successfully, false otherwise.
     */
    bool Receive(const std::string& topic_name, void* buffer, size_t buffer_size, 
                size_t* bytes_received) override;
    
    /**
     * @brief Subscribes to a topic.
     * 
     * @param topic_name The name of the topic to subscribe to.
     * @return true if subscription was successful, false otherwise.
     */
    bool Subscribe(const std::string& topic_name) override;
    
    /**
     * @brief Advertises a topic.
     * 
     * @param topic_name The name of the topic to advertise.
     * @return true if advertisement was successful, false otherwise.
     */
    bool Advertise(const std::string& topic_name) override;
    
    /**
     * @brief Gets the type of this transport.
     * 
     * @return The transport type.
     */
    TransportType GetType() const override { return TransportType::SHARED_MEMORY; }

private:
    // Private constructor
    SharedMemoryTransport(DomainId domain_id, const std::string& participant_name,
                         size_t buffer_size, size_t max_message_size);
    
    // Shared memory segment structure
    struct SharedMemorySegment {
        std::string name;
        void* memory;
        size_t size;
        
        // Default constructor
        SharedMemorySegment() : memory(nullptr), size(0) {}
        
        // Constructor
        SharedMemorySegment(const std::string& segment_name, size_t segment_size)
            : name(segment_name), memory(nullptr), size(segment_size) {}
    };
    
    // Message header structure
    struct MessageHeader {
        uint32_t magic;          // Magic number to identify valid messages
        uint32_t sequence;       // Sequence number
        uint32_t size;           // Size of the message data
        uint32_t checksum;       // Checksum for data integrity
        uint64_t timestamp;      // Timestamp when the message was written
        char topic_name[64];     // Name of the topic
        char sender_name[64];    // Name of the sender
    };
    
    // Ring buffer structure
    struct RingBuffer {
        std::atomic<uint32_t> write_index;  // Current write position
        std::atomic<uint32_t> read_index;   // Current read position
        uint32_t buffer_size;               // Total size of the buffer
        uint32_t max_message_size;          // Maximum size of a single message
        char data[1];                       // Flexible array member for the actual data
    };
    
    // Creates or opens a shared memory segment
    bool CreateOrOpenSegment(const std::string& topic_name, SharedMemorySegment& segment);
    
    // Closes a shared memory segment
    void CloseSegment(SharedMemorySegment& segment);
    
    // Writes a message to a ring buffer
    bool WriteToRingBuffer(RingBuffer* buffer, const std::string& topic_name,
                          const void* data, size_t size);
    
    // Reads a message from a ring buffer
    bool ReadFromRingBuffer(RingBuffer* buffer, const std::string& topic_name,
                           void* data, size_t buffer_size, size_t* bytes_read);
    
    // Domain ID for this transport
    DomainId domain_id_;
    
    // Participant name
    std::string participant_name_;
    
    // Buffer size for shared memory segments
    size_t buffer_size_;
    
    // Maximum message size
    size_t max_message_size_;
    
    // Map of topic names to shared memory segments
    std::unordered_map<std::string, SharedMemorySegment> segments_;
    
    // Mutex for thread safety
    std::mutex mutex_;
    
    // Flag to indicate if the transport is initialized
    bool initialized_;
    
    // Magic number for message headers
    static constexpr uint32_t MAGIC_NUMBER = 0x44445348;  // "SHDD" in ASCII
};

} // namespace transport
} // namespace tiny_dds

#endif // TINY_DDS_SHARED_MEMORY_TRANSPORT_H_ 