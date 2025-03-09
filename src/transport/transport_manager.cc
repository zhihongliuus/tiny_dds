#include "src/transport/transport_manager.h"

namespace tiny_dds {
namespace transport {

TransportManager& TransportManager::GetInstance() {
    static TransportManager instance;
    return instance;
}

TransportManager::TransportManager() {
    // Initialize transport resources
}

TransportManager::~TransportManager() {
    // Clean up transport resources
}

bool TransportManager::RegisterPublisher(
    DomainId domain_id, 
    const std::string& topic_name, 
    const std::string& type_name) {
    absl::MutexLock lock(&mutex_);
    
    // Create publisher info
    PublisherInfo info;
    info.type_name = type_name;
    
    // Add to publishers map
    publishers_[domain_id][topic_name] = std::move(info);
    
    // TODO: Implement discovery mechanism to notify subscribers
    
    return true;
}

void TransportManager::UnregisterPublisher(
    DomainId domain_id, 
    const std::string& topic_name) {
    absl::MutexLock lock(&mutex_);
    
    // Remove from publishers map
    auto domain_it = publishers_.find(domain_id);
    if (domain_it != publishers_.end()) {
        domain_it->second.erase(topic_name);
        
        // Remove domain if empty
        if (domain_it->second.empty()) {
            publishers_.erase(domain_it);
        }
    }
    
    // TODO: Implement discovery mechanism to notify subscribers
}

bool TransportManager::RegisterSubscriber(
    DomainId domain_id, 
    const std::string& topic_name, 
    const std::string& type_name,
    DataReceivedCallback callback) {
    absl::MutexLock lock(&mutex_);
    
    // Create subscriber info
    SubscriberInfo info;
    info.type_name = type_name;
    info.callback = callback;
    
    // Add to subscribers map
    subscribers_[domain_id][topic_name] = std::move(info);
    
    // TODO: Implement discovery mechanism to find publishers
    
    return true;
}

void TransportManager::UnregisterSubscriber(
    DomainId domain_id, 
    const std::string& topic_name) {
    absl::MutexLock lock(&mutex_);
    
    // Remove from subscribers map
    auto domain_it = subscribers_.find(domain_id);
    if (domain_it != subscribers_.end()) {
        domain_it->second.erase(topic_name);
        
        // Remove domain if empty
        if (domain_it->second.empty()) {
            subscribers_.erase(domain_it);
        }
    }
}

bool TransportManager::Publish(
    DomainId domain_id, 
    const std::string& topic_name, 
    const void* data, 
    size_t size) {
    // Make a copy of the callbacks to avoid holding the lock during callbacks
    std::vector<DataReceivedCallback> callbacks;
    
    {
        absl::MutexLock lock(&mutex_);
        
        // Check if there are subscribers for this domain and topic
        auto domain_it = subscribers_.find(domain_id);
        if (domain_it == subscribers_.end()) {
            return false;  // No subscribers for this domain
        }
        
        auto topic_it = domain_it->second.find(topic_name);
        if (topic_it == domain_it->second.end()) {
            return false;  // No subscribers for this topic
        }
        
        // Add callback to the list
        callbacks.push_back(topic_it->second.callback);
    }
    
    // Call all callbacks
    for (const auto& callback : callbacks) {
        callback(domain_id, topic_name, data, size);
    }
    
    return true;
}

} // namespace transport
} // namespace tiny_dds 