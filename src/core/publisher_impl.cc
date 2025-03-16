#include "src/core/publisher_impl.h"

#include "src/core/data_writer_impl.h"
#include "src/core/domain_participant_impl.h"
#include "src/core/topic_impl.h"

namespace tiny_dds::core {

PublisherImpl::PublisherImpl(std::shared_ptr<DomainParticipantImpl> participant)
    : participant_(std::move(participant)) {
  // Initialize any resources needed for the publisher
}

PublisherImpl::~PublisherImpl() {
  // Clean up resources
  absl::MutexLock lock(&mutex_);
  data_writers_.clear();
}

auto PublisherImpl::CreateDataWriter(std::shared_ptr<Topic> topic) -> std::shared_ptr<DataWriter> {
  // Store the participant pointer locally to avoid locking during DataWriterImpl construction
  std::shared_ptr<DomainParticipantImpl> participant;
  {
    absl::MutexLock lock(&mutex_);
    participant = participant_;
  }

  // Create a new data writer
  auto data_writer = std::make_shared<DataWriterImpl>(std::move(topic), shared_from_this());

  // Add it to our map
  {
    absl::MutexLock lock(&mutex_);
    data_writers_[topic->GetName()] = data_writer;
  }

  return data_writer;
}

auto PublisherImpl::GetParticipant() const -> std::shared_ptr<DomainParticipantImpl> {
  absl::MutexLock lock(&mutex_);
  return participant_;
}

}  // namespace tiny_dds::core