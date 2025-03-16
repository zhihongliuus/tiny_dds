#include "src/transport/transport_manager.h"

#include <iostream>

namespace tiny_dds::transport {

auto TransportManager::Create() -> std::shared_ptr<TransportManager> {
  return std::shared_ptr<TransportManager>(new TransportManager());
}

TransportManager::TransportManager() = default;

auto TransportManager::Send(DomainId domain_id, const std::string& topic_name, const void* data,
                            size_t size, TransportType transport_type) -> bool {
  auto transport = GetTransport(domain_id, transport_type);
  if (!transport) {
    std::cerr << "Transport not found for domain " << domain_id << std::endl;
    return false;
  }

  return transport->Send(topic_name, data, size);
}

auto TransportManager::Receive(DomainId domain_id, const std::string& topic_name, void* buffer,
                               size_t buffer_size, size_t* bytes_received,
                               TransportType transport_type) -> bool {
  auto transport = GetTransport(domain_id, transport_type);
  if (!transport) {
    std::cerr << "Transport not found for domain " << domain_id << std::endl;
    return false;
  }

  return transport->Receive(topic_name, buffer, buffer_size, bytes_received);
}

auto TransportManager::CreateTransport(DomainId domain_id, const std::string& participant_name,
                                       const std::string& topic_name, size_t buffer_size,
                                       size_t max_message_size, TransportType transport_type)
    -> bool {
  std::lock_guard<std::mutex> lock(mutex_);

  // Check if we already have a transport for this domain and type
  auto transport = GetTransport(domain_id, transport_type);
  if (transport) {
    // Transport already exists, just return success
    return true;
  }

  // Create a new transport based on the type
  switch (transport_type) {
    case TransportType::UDP: {
      auto udp_transport = UdpTransport::Create(domain_id, participant_name);
      if (!udp_transport || !udp_transport->Initialize()) {
        std::cerr << "Failed to create UDP transport for domain " << domain_id << std::endl;
        return false;
      }
      udp_transports_[domain_id] = udp_transport;
      break;
    }
    case TransportType::SHARED_MEMORY: {
      auto shm_transport =
          SharedMemoryTransport::Create(domain_id, participant_name, buffer_size, max_message_size);
      if (!shm_transport || !shm_transport->Initialize()) {
        std::cerr << "Failed to create shared memory transport for domain " << domain_id
                  << std::endl;
        return false;
      }
      shared_memory_transports_[domain_id] = shm_transport;
      break;
    }
    default:
      std::cerr << "Unsupported transport type" << std::endl;
      return false;
  }

  return true;
}

auto TransportManager::Advertise(DomainId domain_id, const std::string& topic_name,
                                 TransportType transport_type) -> bool {
  auto transport = GetTransport(domain_id, transport_type);
  if (!transport) {
    std::cerr << "Transport not found for domain " << domain_id << std::endl;
    return false;
  }

  return transport->Advertise(topic_name);
}

auto TransportManager::Subscribe(DomainId domain_id, const std::string& topic_name,
                                 TransportType transport_type) -> bool {
  auto transport = GetTransport(domain_id, transport_type);
  if (!transport) {
    std::cerr << "Transport not found for domain " << domain_id << std::endl;
    return false;
  }

  return transport->Subscribe(topic_name);
}

auto TransportManager::GetTransport(DomainId domain_id, TransportType transport_type)
    -> std::shared_ptr<Transport> {
  std::lock_guard<std::mutex> lock(mutex_);

  switch (transport_type) {
    case TransportType::UDP: {
      auto it = udp_transports_.find(domain_id);
      if (it != udp_transports_.end()) {
        return it->second;
      }
      break;
    }
    case TransportType::SHARED_MEMORY: {
      auto it = shared_memory_transports_.find(domain_id);
      if (it != shared_memory_transports_.end()) {
        return it->second;
      }
      break;
    }
    default:
      std::cerr << "Unsupported transport type" << std::endl;
      break;
  }

  return nullptr;
}

}  // namespace tiny_dds::transport