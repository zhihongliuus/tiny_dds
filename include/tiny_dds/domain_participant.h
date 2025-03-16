#ifndef TINY_DDS_DOMAIN_PARTICIPANT_H_
#define TINY_DDS_DOMAIN_PARTICIPANT_H_

#include <memory>
#include <string>

#include "include/tiny_dds/publisher.h"
#include "include/tiny_dds/subscriber.h"
#include "include/tiny_dds/topic.h"
#include "include/tiny_dds/transport_types.h"
#include "include/tiny_dds/types.h"

namespace tiny_dds {

/**
 * @brief Entry point for DDS communication.
 *
 * A DomainParticipant represents a connection to a DDS domain, which is a
 * logical communication channel. It is used to create publishers, subscribers,
 * and topics.
 */
class DomainParticipant {
 public:
  /**
   * @brief Creates a DomainParticipant.
   *
   * @param domain_id The domain ID to connect to.
   * @param name The name of the participant.
   * @return A shared pointer to the created DomainParticipant.
   */
  static std::shared_ptr<DomainParticipant> Create(DomainId domain_id, const std::string& name);

  /**
   * @brief Destructor.
   */
  virtual ~DomainParticipant() = default;

  /**
   * @brief Creates a Publisher.
   *
   * @return A shared pointer to the created Publisher.
   */
  virtual std::shared_ptr<Publisher> CreatePublisher() = 0;

  /**
   * @brief Creates a Subscriber.
   *
   * @return A shared pointer to the created Subscriber.
   */
  virtual std::shared_ptr<Subscriber> CreateSubscriber() = 0;

  /**
   * @brief Creates a Topic.
   *
   * @param name The name of the topic.
   * @param type_name The name of the data type.
   * @return A shared pointer to the created Topic.
   */
  virtual std::shared_ptr<Topic> CreateTopic(const std::string& name,
                                             const std::string& type_name) = 0;

  /**
   * @brief Gets the domain ID.
   *
   * @return The domain ID.
   */
  virtual DomainId GetDomainId() const = 0;

  /**
   * @brief Gets the participant name.
   *
   * @return The participant name.
   */
  virtual const std::string& GetName() const = 0;

  /**
   * @brief Sets the transport type for this participant.
   *
   * @param transport_type The transport type to use.
   * @return true if the transport type was set successfully, false otherwise.
   */
  virtual bool SetTransportType(TransportType transport_type) = 0;

  /**
   * @brief Gets the transport type for this participant.
   *
   * @return The transport type.
   */
  virtual TransportType GetTransportType() const = 0;
};

}  // namespace tiny_dds

#endif  // TINY_DDS_DOMAIN_PARTICIPANT_H_