#ifndef TINY_DDS_TOPIC_H_
#define TINY_DDS_TOPIC_H_

#include <memory>
#include <string>

namespace tiny_dds {

/**
 * @brief A Topic describes the type and structure of data exchanged between
 * participants.
 *
 * Topics are named channels that are strongly typed. Publishers produce data
 * for specific topics, and subscribers consume data from specific topics.
 */
class Topic {
 public:
  virtual ~Topic() = default;

  /**
   * @brief Gets the name of this topic.
   * @return The topic name.
   */
  virtual std::string GetName() const = 0;

  /**
   * @brief Gets the type name associated with this topic.
   * @return The type name.
   */
  virtual std::string GetTypeName() const = 0;
};

}  // namespace tiny_dds

#endif  // TINY_DDS_TOPIC_H_