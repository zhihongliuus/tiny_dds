#include "src/core/subscriber_impl.h"

#include "src/core/data_reader_impl.h"
#include "src/core/domain_participant_impl.h"
#include "src/core/topic_impl.h"

namespace tiny_dds {
namespace core {

SubscriberImpl::SubscriberImpl(std::shared_ptr<DomainParticipantImpl> participant)
    : participant_(participant) {
  // Initialize any resources needed for the subscriber
}

SubscriberImpl::~SubscriberImpl() {
  // Clean up resources
  absl::MutexLock lock(&mutex_);
  data_readers_.clear();
}

std::shared_ptr<DataReader> SubscriberImpl::CreateDataReader(std::shared_ptr<Topic> topic) {
  // Store the participant pointer locally to avoid locking during DataReaderImpl construction
  std::shared_ptr<DomainParticipantImpl> participant;
  {
    absl::MutexLock lock(&mutex_);
    participant = participant_;
  }

  // Create a new data reader
  auto data_reader = std::make_shared<DataReaderImpl>(topic, shared_from_this());

  // Add it to our map
  {
    absl::MutexLock lock(&mutex_);
    data_readers_[topic->GetName()] = data_reader;
  }

  return data_reader;
}

std::shared_ptr<DomainParticipantImpl> SubscriberImpl::GetParticipant() const {
  absl::MutexLock lock(&mutex_);
  return participant_;
}

}  // namespace core
}  // namespace tiny_dds