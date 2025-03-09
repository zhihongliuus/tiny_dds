#include "src/core/data_reader_impl.h"

#include <algorithm>
#include <cstring>

#include "src/core/domain_participant_impl.h"
#include "src/core/subscriber_impl.h"
#include "src/core/topic_impl.h"
#include "src/transport/transport_manager.h"

namespace tiny_dds {
namespace core {

DataReaderImpl::DataReaderImpl(
    std::shared_ptr<Topic> topic, 
    std::shared_ptr<SubscriberImpl> subscriber)
    : topic_(topic),
      subscriber_(subscriber),
      data_received_callback_(nullptr) {
    // Initialize any resources needed for the data reader
    
    // Register with the transport manager
    transport::TransportManager::GetInstance().RegisterSubscriber(
        subscriber_->GetParticipant()->GetDomainId(),
        topic_->GetName(),
        topic_->GetTypeName(),
        [this](DomainId domain_id, const std::string& topic_name, const void* data, size_t size) {
            // Forward the data to our OnDataReceived method
            this->OnDataReceived(data, size);
        });
}

DataReaderImpl::~DataReaderImpl() {
    // Clean up resources
    
    // Unregister from the transport manager
    transport::TransportManager::GetInstance().UnregisterSubscriber(
        subscriber_->GetParticipant()->GetDomainId(),
        topic_->GetName());
}

int32_t DataReaderImpl::Read(void* buffer, size_t buffer_size, SampleInfo& info) {
    absl::MutexLock lock(&mutex_);
    
    if (samples_.empty()) {
        return -1;  // No data available
    }
    
    const DataSample& sample = samples_.front();
    
    // Check if buffer is large enough
    if (buffer_size < sample.data.size()) {
        return -1;  // Buffer too small
    }
    
    // Copy data to buffer
    std::memcpy(buffer, sample.data.data(), sample.data.size());
    
    // Copy sample info
    info = sample.info;
    
    return static_cast<int32_t>(sample.data.size());
}

int32_t DataReaderImpl::Take(void* buffer, size_t buffer_size, SampleInfo& info) {
    absl::MutexLock lock(&mutex_);
    
    if (samples_.empty()) {
        return -1;  // No data available
    }
    
    const DataSample& sample = samples_.front();
    
    // Check if buffer is large enough
    if (buffer_size < sample.data.size()) {
        return -1;  // Buffer too small
    }
    
    // Copy data to buffer
    std::memcpy(buffer, sample.data.data(), sample.data.size());
    
    // Copy sample info
    info = sample.info;
    
    // Remove the sample from the queue
    samples_.pop_front();
    
    return static_cast<int32_t>(sample.data.size());
}

void DataReaderImpl::SetDataReceivedCallback(DataReaderCallback callback) {
    absl::MutexLock lock(&mutex_);
    data_received_callback_ = callback;
}

std::shared_ptr<Topic> DataReaderImpl::GetTopic() const {
    absl::MutexLock lock(&mutex_);
    return topic_;
}

SubscriptionMatchedStatus DataReaderImpl::GetSubscriptionMatchedStatus() const {
    absl::MutexLock lock(&mutex_);
    return subscription_matched_status_;
}

std::shared_ptr<SubscriberImpl> DataReaderImpl::GetSubscriber() const {
    absl::MutexLock lock(&mutex_);
    return subscriber_;
}

void DataReaderImpl::OnDataReceived(const void* data, size_t size) {
    // Create a new data sample
    DataSample sample;
    sample.data.resize(size);
    std::memcpy(sample.data.data(), data, size);
    
    // Set sample info
    sample.info.valid_data = true;
    
    // Make a copy of the callback to avoid holding the lock during the callback
    DataReaderCallback callback_copy;
    
    {
        absl::MutexLock lock(&mutex_);
        
        // Add to queue
        samples_.push_back(std::move(sample));
        
        // Only notify if this is the first sample
        if (samples_.size() == 1) {
            data_available_.Notify();
        }
        
        // Copy the callback if set
        if (data_received_callback_) {
            callback_copy = data_received_callback_;
        }
    }
    
    // Call callback if set
    if (callback_copy) {
        callback_copy(data, size, sample.info);
    }
}

} // namespace core
} // namespace tiny_dds 