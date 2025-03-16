#ifndef TINY_DDS_SUBSCRIBER_H_
#define TINY_DDS_SUBSCRIBER_H_

#include <memory>
#include <string>

// Forward declarations
namespace tiny_dds {
class DataReader;
class Topic;

/**
 * @brief A Subscriber is an entity used to create DataReader objects.
 *
 * The Subscriber acts as a factory for DataReader objects and handles
 * the reception of data from DataReaders.
 */
class Subscriber {
 public:
  virtual ~Subscriber() = default;

  /**
   * @brief Creates a DataReader for a specific topic.
   * @param topic The topic to subscribe to.
   * @return A shared pointer to the created DataReader.
   */
  virtual std::shared_ptr<DataReader> CreateDataReader(std::shared_ptr<Topic> topic) = 0;
};

}  // namespace tiny_dds

#endif  // TINY_DDS_SUBSCRIBER_H_