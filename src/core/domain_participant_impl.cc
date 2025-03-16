#include "src/core/domain_participant_impl.h"

#include "src/core/publisher_impl.h"
#include "src/core/subscriber_impl.h"
#include "src/core/topic_impl.h"

namespace tiny_dds {

// Static method implementation for DomainParticipant::Create
std::shared_ptr<DomainParticipant> DomainParticipant::Create(DomainId domain_id,
                                                             const std::string& participant_name) {
  return std::make_shared<core::DomainParticipantImpl>(domain_id, participant_name);
}

namespace core {

DomainParticipantImpl::DomainParticipantImpl(DomainId domain_id,
                                             const std::string& participant_name)
    : domain_id_(domain_id),
      participant_name_(participant_name),
      transport_type_(TransportType::UDP) {
  // Initialize any resources needed for the domain participant
}

DomainParticipantImpl::~DomainParticipantImpl() {
  // Clean up resources
  absl::MutexLock lock(&mutex_);

  // Clear publishers, subscribers, and topics
  publishers_.clear();
  subscribers_.clear();
  topics_.clear();
}

std::shared_ptr<Publisher> DomainParticipantImpl::CreatePublisher() {
  absl::MutexLock lock(&mutex_);

  auto publisher = std::make_shared<PublisherImpl>(shared_from_this());
  publishers_.push_back(publisher);
  return publisher;
}

std::shared_ptr<Subscriber> DomainParticipantImpl::CreateSubscriber() {
  absl::MutexLock lock(&mutex_);

  auto subscriber = std::make_shared<SubscriberImpl>(shared_from_this());
  subscribers_.push_back(subscriber);
  return subscriber;
}

std::shared_ptr<Topic> DomainParticipantImpl::CreateTopic(const std::string& topic_name,
                                                          const std::string& type_name) {
  absl::MutexLock lock(&mutex_);

  // Check if topic already exists
  auto it = topics_.find(topic_name);
  if (it != topics_.end()) {
    // Topic exists, check if type name matches
    if (it->second->GetTypeName() == type_name) {
      return it->second;
    } else {
      // Type mismatch, return nullptr
      return nullptr;
    }
  }

  // Create a new topic
  auto topic = std::make_shared<TopicImpl>(topic_name, type_name, shared_from_this());
  topics_[topic_name] = topic;
  return topic;
}

DomainId DomainParticipantImpl::GetDomainId() const { return domain_id_; }

const std::string& DomainParticipantImpl::GetName() const { return participant_name_; }

bool DomainParticipantImpl::SetTransportType(TransportType transport_type) {
  absl::MutexLock lock(&mutex_);

  // Check if we already have publishers or subscribers
  if (!publishers_.empty() || !subscribers_.empty()) {
    // Cannot change transport type after creating publishers or subscribers
    return false;
  }

  transport_type_ = transport_type;
  return true;
}

TransportType DomainParticipantImpl::GetTransportType() const {
  absl::MutexLock lock(&mutex_);
  return transport_type_;
}

}  // namespace core
}  // namespace tiny_dds