#include "src/core/topic_impl.h"

#include "src/core/domain_participant_impl.h"

namespace tiny_dds {
namespace core {

TopicImpl::TopicImpl(
    const std::string& topic_name, 
    const std::string& type_name,
    std::shared_ptr<DomainParticipantImpl> participant)
    : topic_name_(topic_name),
      type_name_(type_name),
      participant_(participant) {
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

} // namespace core
} // namespace tiny_dds 