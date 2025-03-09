#include "src/core/publisher_impl.h"

#include "src/core/data_writer_impl.h"
#include "src/core/domain_participant_impl.h"
#include "src/core/topic_impl.h"

namespace tiny_dds {
namespace core {

PublisherImpl::PublisherImpl(std::shared_ptr<DomainParticipantImpl> participant)
    : participant_(participant) {
    // Initialize any resources needed for the publisher
}

PublisherImpl::~PublisherImpl() {
    // Clean up resources
    absl::MutexLock lock(&mutex_);
    data_writers_.clear();
}

std::shared_ptr<DataWriter> PublisherImpl::CreateDataWriter(
    std::shared_ptr<Topic> topic) {
    // Store the participant pointer locally to avoid locking during DataWriterImpl construction
    std::shared_ptr<DomainParticipantImpl> participant;
    {
        absl::MutexLock lock(&mutex_);
        participant = participant_;
    }
    
    // Create a new data writer
    auto data_writer = std::make_shared<DataWriterImpl>(
        topic, shared_from_this());
    
    // Add it to our map
    {
        absl::MutexLock lock(&mutex_);
        data_writers_[topic->GetName()] = data_writer;
    }
    
    return data_writer;
}

std::shared_ptr<DomainParticipantImpl> PublisherImpl::GetParticipant() const {
    absl::MutexLock lock(&mutex_);
    return participant_;
}

} // namespace core
} // namespace tiny_dds 