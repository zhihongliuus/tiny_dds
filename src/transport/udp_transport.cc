#include "src/transport/udp_transport.h"

#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace tiny_dds {
namespace transport {

std::shared_ptr<UdpTransport> UdpTransport::Create(
    DomainId domain_id,
    const std::string& participant_name) {
    return std::shared_ptr<UdpTransport>(
        new UdpTransport(domain_id, participant_name));
}

UdpTransport::UdpTransport(
    DomainId domain_id,
    const std::string& participant_name)
    : domain_id_(domain_id),
      participant_name_(participant_name),
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

bool UdpTransport::Send(const std::string& topic_name, const void* data, size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the socket for this topic
    auto it = udp_sockets_.find(topic_name);
    if (it == udp_sockets_.end()) {
        std::cerr << "Socket not found for topic: " << topic_name << std::endl;
        return false;
    }
    
    // Get the socket info
    const UdpSocketInfo& info = it->second;
    
    // Create destination address
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(info.port);
    
    // Use broadcast address for publishing
    if (inet_pton(AF_INET, "255.255.255.255", &dest_addr.sin_addr) != 1) {
        std::cerr << "Failed to convert broadcast address: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Send the data
    ssize_t sent = sendto(info.socket_fd, data, size, 0, 
                         (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    
    if (sent < 0) {
        std::cerr << "Failed to send data: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

bool UdpTransport::Receive(
    const std::string& topic_name, void* buffer, size_t buffer_size, size_t* bytes_received) {
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
    struct sockaddr_in src_addr;
    socklen_t src_addr_len = sizeof(src_addr);
    
    // Try to receive data (non-blocking)
    ssize_t received = recvfrom(info.socket_fd, buffer, buffer_size, MSG_DONTWAIT,
                               (struct sockaddr*)&src_addr, &src_addr_len);
    
    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No data available, not an error
            return false;
        }
        
        std::cerr << "Failed to receive data: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Set the number of bytes received
    if (bytes_received) {
        *bytes_received = received;
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

bool UdpTransport::ConnectToSocket(const std::string& topic_name) {
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
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;  // Bind to all interfaces
    
    // Bind the socket to the local address
    if (bind(socket_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
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

int UdpTransport::GenerateUdpPort(const std::string& topic_name) {
    // Use a simple hash function to generate a port in the range 10000-65535
    size_t hash = std::hash<std::string>()(topic_name) ^ std::hash<DomainId>()(domain_id_);
    return 10000 + (hash % 55535);
}

} // namespace transport
} // namespace tiny_dds 