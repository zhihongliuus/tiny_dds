#ifndef TINY_DDS_CORE_SUBSCRIBER_IMPL_H_
#define TINY_DDS_CORE_SUBSCRIBER_IMPL_H_

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/synchronization/mutex.h"
#include "include/tiny_dds/subscriber.h"

namespace tiny_dds {
namespace core {

// Forward declarations
class DataReaderImpl;
class DomainParticipantImpl;

/**
 * @brief Implementation of the Subscriber interface.
 */
class SubscriberImpl : public tiny_dds::Subscriber,
                       public std::enable_shared_from_this<SubscriberImpl> {
 public:
  /**
   * @brief Constructor for SubscriberImpl.
   * @param participant The domain participant that created this subscriber.
   */
  explicit SubscriberImpl(std::shared_ptr<DomainParticipantImpl> participant);

  /**
   * @brief Destructor for SubscriberImpl.
   */
  ~SubscriberImpl() override;

  /**
   * @brief Creates a DataReader for a specific topic.
   * @param topic The topic to subscribe to.
   * @return A shared pointer to the created DataReader.
   */
  std::shared_ptr<tiny_dds::DataReader> CreateDataReader(
      std::shared_ptr<tiny_dds::Topic> topic) override;

  /**
   * @brief Gets the domain participant that created this subscriber.
   * @return A shared pointer to the domain participant.
   */
  std::shared_ptr<DomainParticipantImpl> GetParticipant() const;

 private:
  // The domain participant that created this subscriber
  std::shared_ptr<DomainParticipantImpl> participant_;

  // Map of data readers by topic name
  absl::flat_hash_map<std::string, std::shared_ptr<DataReaderImpl>> data_readers_;

  // Mutex for thread safety
  mutable absl::Mutex mutex_;
};

}  // namespace core
}  // namespace tiny_dds

#endif  // TINY_DDS_CORE_SUBSCRIBER_IMPL_H_