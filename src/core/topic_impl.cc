#include "src/core/topic_impl.h"

#include "src/core/domain_participant_impl.h"

namespace tiny_dds::core {

TopicImpl::TopicImpl(
    std::string topic_name, 
    std::string type_name,
    std::shared_ptr<DomainParticipantImpl> participant)
    : topic_name_(std::move(topic_name)),
      type_name_(std::move(type_name)),
      participant_(std::move(participant)) {
    // Initialize any resources needed for the topic
}

TopicImpl::~TopicImpl() {
    // Clean up resources
}

std::string TopicImpl::GetName() const {
    absl::MutexLock lock(&mutex_);
    return topic_name_;
}

std::string TopicImpl::GetTypeName() const {
    absl::MutexLock lock(&mutex_);
    return type_name_;
}

std::shared_ptr<DomainParticipantImpl> TopicImpl::GetParticipant() const {
    absl::MutexLock lock(&mutex_);
    return participant_;
}

} // namespace tiny_dds::core 