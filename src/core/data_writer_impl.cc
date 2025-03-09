#include "src/core/data_writer_impl.h"

#include "src/core/domain_participant_impl.h"
#include "src/core/publisher_impl.h"
#include "src/core/topic_impl.h"
#include "src/transport/transport_manager.h"

namespace tiny_dds {
namespace core {

DataWriterImpl::DataWriterImpl(
    std::shared_ptr<tiny_dds::Topic> topic, 
    std::shared_ptr<PublisherImpl> publisher)
    : topic_(topic),
      publisher_(publisher) {
    // Initialize any resources needed for the data writer
    
    // Register with the transport manager
    transport::TransportManager::GetInstance().RegisterPublisher(
        publisher_->GetParticipant()->GetDomainId(),
        topic_->GetName(),
        topic_->GetTypeName());
}

DataWriterImpl::~DataWriterImpl() {
    // Clean up resources
    
    // Unregister from the transport manager
    transport::TransportManager::GetInstance().UnregisterPublisher(
        publisher_->GetParticipant()->GetDomainId(),
        topic_->GetName());
}

bool DataWriterImpl::Write(const void* data, size_t size) {
    absl::MutexLock lock(&mutex_);
    
    // Use the transport manager to publish the data
    return transport::TransportManager::GetInstance().Publish(
        publisher_->GetParticipant()->GetDomainId(),
        topic_->GetName(),
        data,
        size);
}

std::shared_ptr<tiny_dds::Topic> DataWriterImpl::GetTopic() const {
    absl::MutexLock lock(&mutex_);
    return topic_;
}

tiny_dds::PublicationMatchedStatus DataWriterImpl::GetPublicationMatchedStatus() const {
    absl::MutexLock lock(&mutex_);
    return publication_matched_status_;
}

std::shared_ptr<PublisherImpl> DataWriterImpl::GetPublisher() const {
    absl::MutexLock lock(&mutex_);
    return publisher_;
}

} // namespace core
} // namespace tiny_dds 