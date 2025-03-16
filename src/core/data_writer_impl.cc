#include "src/core/data_writer_impl.h"

#include "src/core/domain_participant_impl.h"
#include "src/core/publisher_impl.h"
#include "src/core/topic_impl.h"
#include "src/transport/transport_manager.h"

namespace tiny_dds::core {

// Constants to replace magic numbers
constexpr size_t kDefaultBufferSize = 1024 * 1024;    // 1MB buffer size
constexpr size_t kDefaultMaxMessageSize = 64 * 1024;  // 64KB max message size

DataWriterImpl::DataWriterImpl(std::shared_ptr<tiny_dds::Topic> topic,
                               std::shared_ptr<PublisherImpl> publisher)
    : topic_(std::move(topic)), publisher_(std::move(publisher)) {
  // Initialize any resources needed for the data writer

  // Get the transport manager
  auto transport_manager = transport::TransportManager::Create();

  // Create the transport for this topic
  transport_manager->CreateTransport(publisher_->GetParticipant()->GetDomainId(),
                                     publisher_->GetParticipant()->GetName(), topic_->GetName(),
                                     kDefaultBufferSize, kDefaultMaxMessageSize,
                                     publisher_->GetParticipant()->GetTransportType());

  // Advertise the topic
  transport_manager->Advertise(publisher_->GetParticipant()->GetDomainId(), topic_->GetName(),
                               publisher_->GetParticipant()->GetTransportType());
}

DataWriterImpl::~DataWriterImpl() {
  // Clean up resources
}

bool DataWriterImpl::Write(const void* data, size_t size) {
  absl::MutexLock lock(&mutex_);

  // Get the transport manager
  auto transport_manager = transport::TransportManager::Create();

  // Use the transport manager to publish the data
  return transport_manager->Send(publisher_->GetParticipant()->GetDomainId(), topic_->GetName(),
                                 data, size, publisher_->GetParticipant()->GetTransportType());
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

}  // namespace tiny_dds::core