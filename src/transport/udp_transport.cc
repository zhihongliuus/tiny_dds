#include "src/transport/udp_transport.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <array>

namespace tiny_dds::transport {

// Constants for port generation
constexpr int kBasePortNumber = 40000;
constexpr int kPortRangeSize = 10000;
constexpr size_t kBufferSize = 256;

auto UdpTransport::Create(
    DomainId domain_id,
    const std::string& participant_name) -> std::shared_ptr<UdpTransport> {
    return std::shared_ptr<UdpTransport>(
        new UdpTransport(domain_id, participant_name));
}

UdpTransport::UdpTransport(
    DomainId domain_id,
    std::string participant_name)
    : domain_id_(domain_id),
      participant_name_(std::move(participant_name)),
      initialized_(false) {
}

UdpTransport::~UdpTransport() {
    // Close all sockets
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& pair : udp_sockets_) {
        if (pair.second.socket_fd >= 0) {
            close(pair.second.socket_fd);
        }
    }
    
    udp_sockets_.clear();
}

bool UdpTransport::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        return true;
    }
    
    // Nothing to do here for now, initialization happens when topics are advertised or subscribed
    
    initialized_ = true;
    return true;
}

bool UdpTransport::Advertise(const std::string& topic_name) {
    return CreateSocket(topic_name);
}

bool UdpTransport::Subscribe(const std::string& topic_name) {
    return ConnectToSocket(topic_name);
}

auto UdpTransport::Send(const std::string& topic_name, const void* data, size_t size) -> bool {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the socket for this topic
    auto it = udp_sockets_.find(topic_name);
    if (it == udp_sockets_.end()) {
        std::cerr << "Socket not found for topic: " << topic_name << std::endl;
        return false;
    }
    
    // Get the socket info
    const UdpSocketInfo& info = it->second;
    
    // Set up the destination address
    struct sockaddr_in dest_addr{};  // Zero-initialize the struct
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(info.port);
    
    // Convert the IP address string to binary form
    if (inet_pton(AF_INET, info.address.c_str(), &dest_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address: " << info.address << std::endl;
        return false;
    }
    
    // Send the data
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    // Unavoidable system call that requires sockaddr* - standard practice in socket programming
    ssize_t sent = sendto(info.socket_fd, data, size, 0, 
                         reinterpret_cast<struct sockaddr*>(&dest_addr), sizeof(dest_addr));
    
    if (sent < 0) {
        std::cerr << "Failed to send data: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

auto UdpTransport::Receive(
    const std::string& topic_name, void* buffer, size_t buffer_size, size_t* bytes_received) -> bool {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the socket for this topic
    auto it = udp_sockets_.find(topic_name);
    if (it == udp_sockets_.end()) {
        std::cerr << "Socket not found for topic: " << topic_name << std::endl;
        return false;
    }
    
    // Get the socket info
    const UdpSocketInfo& info = it->second;
    
    // Set up the source address
    struct sockaddr_in src_addr{};  // Zero-initialize the struct
    socklen_t src_addr_len = sizeof(src_addr);
    
    // Try to receive data (non-blocking)
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    // Unavoidable system call that requires sockaddr* - standard practice in socket programming
    ssize_t received = recvfrom(info.socket_fd, buffer, buffer_size, MSG_DONTWAIT,
                               reinterpret_cast<struct sockaddr*>(&src_addr), &src_addr_len);
    
    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No data available, not an error
            return false;
        }
        
        std::cerr << "Failed to receive data: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Set the number of bytes received
    if (bytes_received != nullptr) {
        *bytes_received = static_cast<size_t>(received);
    }
    
    return true;
}

bool UdpTransport::CreateSocket(const std::string& topic_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if socket already exists
    auto it = udp_sockets_.find(topic_name);
    if (it != udp_sockets_.end()) {
        // Socket already exists, just return success
        return true;
    }
    
    // Create a new UDP socket
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Set socket options for broadcast
    int broadcast = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        std::cerr << "Failed to set socket options: " << strerror(errno) << std::endl;
        close(socket_fd);
        return false;
    }
    
    // Set socket to non-blocking mode
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags < 0) {
        std::cerr << "Failed to get socket flags: " << strerror(errno) << std::endl;
        close(socket_fd);
        return false;
    }
    
    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        std::cerr << "Failed to set socket to non-blocking mode: " << strerror(errno) << std::endl;
        close(socket_fd);
        return false;
    }
    
    // Generate a port for this topic
    int port = GenerateUdpPort(topic_name);
    
    // Create socket info
    UdpSocketInfo info;
    info.socket_fd = socket_fd;
    info.port = port;
    info.address = "0.0.0.0";  // Bind to all interfaces
    info.is_publisher = true;
    
    // Store the socket info
    udp_sockets_[topic_name] = info;
    
    return true;
}

auto UdpTransport::ConnectToSocket(const std::string& topic_name) -> bool {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if socket already exists
    auto it = udp_sockets_.find(topic_name);
    if (it != udp_sockets_.end()) {
        // Socket already exists, just return success
        return true;
    }
    
    // Create a new UDP socket
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Set socket to non-blocking mode
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags < 0) {
        std::cerr << "Failed to get socket flags: " << strerror(errno) << std::endl;
        close(socket_fd);
        return false;
    }
    
    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        std::cerr << "Failed to set socket to non-blocking mode: " << strerror(errno) << std::endl;
        close(socket_fd);
        return false;
    }
    
    // Generate a port for this topic
    int port = GenerateUdpPort(topic_name);
    
    // Set up the local address
    struct sockaddr_in local_addr{};  // Zero-initialize the struct
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;  // Bind to all interfaces
    
    // Bind the socket to the local address
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    // Unavoidable system call that requires sockaddr* - standard practice in socket programming
    if (bind(socket_fd, reinterpret_cast<struct sockaddr*>(&local_addr), sizeof(local_addr)) < 0) {
        std::cerr << "Failed to bind socket: " << strerror(errno) << std::endl;
        close(socket_fd);
        return false;
    }
    
    // Create socket info
    UdpSocketInfo info;
    info.socket_fd = socket_fd;
    info.port = port;
    info.address = "0.0.0.0";  // Bind to all interfaces
    info.is_publisher = false;
    
    // Store the socket info
    udp_sockets_[topic_name] = info;
    
    return true;
}

void UdpTransport::CloseSocket(const std::string& topic_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the socket for this topic
    auto it = udp_sockets_.find(topic_name);
    if (it == udp_sockets_.end()) {
        return;  // Socket not found, nothing to do
    }
    
    // Close the socket
    if (it->second.socket_fd >= 0) {
        close(it->second.socket_fd);
    }
    
    // Remove from map
    udp_sockets_.erase(it);
}

auto UdpTransport::GenerateUdpPort(const std::string& topic_name) -> int {
    // Use a simple hash function to generate a port number
    // This ensures that the same topic name always gets the same port
    
    // Calculate a hash of the topic name
    size_t hash = std::hash<std::string>{}(topic_name);
    
    // Use the domain ID and hash to generate a port number
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    // Using snprintf for string formatting is safe here with proper buffer size
    std::array<char, kBufferSize> buffer{};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    std::snprintf(buffer.data(), buffer.size(), "%d_%zu", domain_id_, hash);
    
    // Generate another hash from the combined string
    std::string buffer_str(buffer.data());
    size_t final_hash = std::hash<std::string>{}(buffer_str);
    
    // Map the hash to a port number in the range [kBasePortNumber, kBasePortNumber + kPortRangeSize)
    return static_cast<int>(kBasePortNumber + (final_hash % static_cast<size_t>(kPortRangeSize)));
}

} // namespace tiny_dds::transport 