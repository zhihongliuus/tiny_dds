#ifndef TINY_DDS_PUBLISHER_H_
#define TINY_DDS_PUBLISHER_H_

#include <memory>
#include <string>

// Forward declarations
namespace tiny_dds {
class DataWriter;
class Topic;

/**
 * @brief A Publisher is an entity used to create DataWriter objects.
 *
 * The Publisher acts as a factory for DataWriter objects and handles the
 * distribution of data produced by its associated DataWriters.
 */
class Publisher {
 public:
  virtual ~Publisher() = default;

  /**
   * @brief Creates a DataWriter for a specific topic.
   * @param topic The topic to publish data on.
   * @return A shared pointer to the created DataWriter.
   */
  virtual std::shared_ptr<DataWriter> CreateDataWriter(std::shared_ptr<Topic> topic) = 0;
};

}  // namespace tiny_dds

#endif  // TINY_DDS_PUBLISHER_H_