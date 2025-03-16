#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/time/time.h"
#include "include/tiny_dds/data_reader.h"
#include "include/tiny_dds/data_writer.h"
#include "include/tiny_dds/domain_participant.h"
#include "include/tiny_dds/publisher.h"
#include "include/tiny_dds/subscriber.h"
#include "include/tiny_dds/topic.h"
#include "include/tiny_dds/types.h"

ABSL_FLAG(int, domain_id, 0, "Domain ID to use for the example");
ABSL_FLAG(std::string, topic_name, "example_topic", "Topic name to use for the example");
ABSL_FLAG(int, num_messages, 10, "Number of messages to publish");
ABSL_FLAG(int, message_size, 100, "Size of each message in bytes");
ABSL_FLAG(int, publish_interval_ms, 1000, "Interval between publishing messages in milliseconds");

int main(int argc, char* argv[]) {
  // Parse command line flags
  absl::ParseCommandLine(argc, argv);

  // Get flag values
  int domain_id = absl::GetFlag(FLAGS_domain_id);
  std::string topic_name = absl::GetFlag(FLAGS_topic_name);
  int num_messages = absl::GetFlag(FLAGS_num_messages);
  int message_size = absl::GetFlag(FLAGS_message_size);
  int publish_interval_ms = absl::GetFlag(FLAGS_publish_interval_ms);

  std::cout << "Starting simple publish/subscribe example" << std::endl;
  std::cout << "Domain ID: " << domain_id << std::endl;
  std::cout << "Topic name: " << topic_name << std::endl;
  std::cout << "Number of messages: " << num_messages << std::endl;
  std::cout << "Message size: " << message_size << " bytes" << std::endl;
  std::cout << "Publish interval: " << publish_interval_ms << " ms" << std::endl;

  // Create domain participants
  auto publisher_participant =
      tiny_dds::DomainParticipant::Create(domain_id, "publisher_participant");
  auto subscriber_participant =
      tiny_dds::DomainParticipant::Create(domain_id, "subscriber_participant");

  // Create publisher and subscriber
  auto publisher = publisher_participant->CreatePublisher();
  auto subscriber = subscriber_participant->CreateSubscriber();

  // Create topics
  auto publisher_topic = publisher_participant->CreateTopic(topic_name, "raw_data");
  auto subscriber_topic = subscriber_participant->CreateTopic(topic_name, "raw_data");

  // Create data writer and reader
  auto data_writer = publisher->CreateDataWriter(publisher_topic);
  auto data_reader = subscriber->CreateDataReader(subscriber_topic);

  // Set up data received callback
  data_reader->SetDataReceivedCallback(
      [](const void* data, size_t size, const tiny_dds::SampleInfo& info) {
        std::cout << "Received data of size " << size << " bytes" << std::endl;

        // Print the first few bytes of the data
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        std::cout << "Data: ";
        for (size_t i = 0; i < std::min(size, size_t(16)); ++i) {
          std::cout << static_cast<int>(bytes[i]) << " ";
        }
        std::cout << std::endl;
      });

  // Create a buffer for the test data
  std::vector<uint8_t> buffer(message_size);

  // Publish messages
  for (int i = 0; i < num_messages; ++i) {
    // Fill the buffer with some data
    for (int j = 0; j < message_size; ++j) {
      buffer[j] = static_cast<uint8_t>((i + j) % 256);
    }

    // Write the data
    std::cout << "Publishing message " << (i + 1) << " of " << num_messages << std::endl;
    if (data_writer->Write(buffer.data(), buffer.size())) {
      std::cout << "Published successfully" << std::endl;
    } else {
      std::cout << "Failed to publish" << std::endl;
    }

    // Wait before publishing the next message
    std::this_thread::sleep_for(std::chrono::milliseconds(publish_interval_ms));
  }

  // Wait a bit to ensure all messages are received
  std::cout << "Waiting for all messages to be received..." << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(publish_interval_ms));

  std::cout << "Example completed successfully" << std::endl;
  return 0;
}