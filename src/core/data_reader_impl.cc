#include "src/core/data_reader_impl.h"

#include <algorithm>
#include <cstring>

#include "src/core/domain_participant_impl.h"
#include "src/core/subscriber_impl.h"
#include "src/core/topic_impl.h"
#include "src/transport/transport_manager.h"

namespace tiny_dds::core {

// Constants to replace magic numbers
constexpr size_t kDefaultBufferSize = 1024 * 1024;  // 1MB buffer size
constexpr size_t kDefaultMaxMessageSize = 64 * 1024;  // 64KB max message size

DataReaderImpl::DataReaderImpl(
    std::shared_ptr<Topic> topic, 
    std::shared_ptr<SubscriberImpl> subscriber)
    : topic_(std::move(topic)),
      subscriber_(std::move(subscriber)),
      data_received_callback_(nullptr) {
    // Initialize any resources needed for the data reader
    
    // Get the transport manager
    auto transport_manager = transport::TransportManager::Create();
    
    // Create the transport for this topic
    transport_manager->CreateTransport(
        subscriber_->GetParticipant()->GetDomainId(),
        subscriber_->GetParticipant()->GetName(),
        topic_->GetName(),
        kDefaultBufferSize,
        kDefaultMaxMessageSize,
        subscriber_->GetParticipant()->GetTransportType());
    
    // Subscribe to the topic
    transport_manager->Subscribe(
        subscriber_->GetParticipant()->GetDomainId(),
        topic_->GetName(),
        subscriber_->GetParticipant()->GetTransportType());
}

DataReaderImpl::~DataReaderImpl() {
    // Clean up resources
}

int32_t DataReaderImpl::Read(void* buffer, size_t buffer_size, SampleInfo& info) {
    absl::MutexLock lock(&mutex_);
    
    // Get the transport manager
    auto transport_manager = transport::TransportManager::Create();
    
    // Use the transport manager to receive data
    size_t bytes_received = 0;
    bool result = transport_manager->Receive(
        subscriber_->GetParticipant()->GetDomainId(),
        topic_->GetName(),
        buffer,
        buffer_size,
        &bytes_received,
        subscriber_->GetParticipant()->GetTransportType());
    
    if (result) {
        // Update sample info
        info.valid_data = true;
    }
    
    return result ? static_cast<int32_t>(bytes_received) : -1;
}

int32_t DataReaderImpl::Take(void* buffer, size_t buffer_size, SampleInfo& info) {
    // For now, Take is the same as Read
    return Read(buffer, buffer_size, info);
}

void DataReaderImpl::SetDataReceivedCallback(tiny_dds::DataReaderCallback callback) {
    absl::MutexLock lock(&mutex_);
    data_received_callback_ = callback;
}

void DataReaderImpl::SetDataCallback(tiny_dds::DataCallback callback) {
    absl::MutexLock lock(&mutex_);
    data_callback_ = callback;
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

} // namespace tiny_dds::core 