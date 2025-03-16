#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/time/time.h"
#include "include/tiny_dds/auto_config.h"
#include "include/tiny_dds/data_reader.h"
#include "include/tiny_dds/data_writer.h"
#include "include/tiny_dds/domain_participant.h"
#include "include/tiny_dds/publisher.h"
#include "include/tiny_dds/subscriber.h"
#include "include/tiny_dds/topic.h"
#include "include/tiny_dds/transport_types.h"
#include "include/tiny_dds/types.h"

ABSL_FLAG(std::string, config_file, "", "Path to the YAML configuration file");
ABSL_FLAG(int, num_messages, 5, "Number of messages to publish");
ABSL_FLAG(int, publish_interval_ms, 1000, "Interval between published messages in milliseconds");
ABSL_FLAG(int, message_size, 100, "Size of each message in bytes");
ABSL_FLAG(std::string, transport, "SHARED_MEMORY", "Transport to use (SHARED_MEMORY or UDP)");

// Simple message handler
void HandleMessage(tiny_dds::DomainId domain_id, const std::string& topic_name, const void* data,
                   size_t size) {
  std::cout << "Received message on topic '" << topic_name << "' in domain " << domain_id
            << " (size: " << size << " bytes)" << std::endl;

  // Print first few bytes of the message
  const uint8_t* bytes = static_cast<const uint8_t*>(data);
  std::cout << "Message data: ";
  for (size_t i = 0; i < std::min(size, static_cast<size_t>(16)); ++i) {
    std::cout << static_cast<int>(bytes[i]) << " ";
  }
  std::cout << "..." << std::endl;
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  // Get command line arguments
  std::string config_file = absl::GetFlag(FLAGS_config_file);
  int num_messages = absl::GetFlag(FLAGS_num_messages);
  int publish_interval_ms = absl::GetFlag(FLAGS_publish_interval_ms);
  int message_size = absl::GetFlag(FLAGS_message_size);
  std::string transport_str = absl::GetFlag(FLAGS_transport);

  if (config_file.empty()) {
    std::cerr << "Error: config_file flag is required." << std::endl;
    return 1;
  }

  // Load configuration
  auto loader = tiny_dds::auto_config::AutoConfigLoader::Create();
  if (!loader->LoadFromFile(config_file)) {
    std::cerr << "Error: Failed to load configuration from file: " << config_file << std::endl;
    return 1;
  }

  // Determine which participant to use based on transport
  tiny_dds::TransportType transport_type = tiny_dds::StringToTransportType(transport_str);
  std::string participant_name;
  std::string topic_name;

  if (transport_type == tiny_dds::TransportType::SHARED_MEMORY) {
    participant_name = "ExampleParticipant";
    topic_name = "Example Topic";
  } else {
    participant_name = "UdpParticipant";
    topic_name = "UDP Topic";
  }

  // Get participant
  auto participant = loader->GetParticipant(participant_name);
  if (!participant) {
    std::cerr << "Error: Failed to get participant: " << participant_name << std::endl;
    return 1;
  }

  // Get publisher and subscriber
  auto publisher = loader->GetPublisher(participant_name,
                                        (transport_type == tiny_dds::TransportType::SHARED_MEMORY)
                                            ? "ExamplePublisher"
                                            : "UdpPublisher");
  auto subscriber = loader->GetSubscriber(participant_name,
                                          (transport_type == tiny_dds::TransportType::SHARED_MEMORY)
                                              ? "ExampleSubscriber"
                                              : "UdpSubscriber");

  if (!publisher || !subscriber) {
    std::cerr << "Error: Failed to get publisher or subscriber" << std::endl;
    return 1;
  }

  // Get topic
  auto topic = loader->GetTopic(participant_name, topic_name);
  if (!topic) {
    std::cerr << "Error: Failed to get topic: " << topic_name << std::endl;
    return 1;
  }

  // Create data writer and reader
  auto writer = publisher->CreateDataWriter(topic);
  auto reader = subscriber->CreateDataReader(topic);
  if (!writer || !reader) {
    std::cerr << "Error: Failed to create data writer or reader" << std::endl;
    return 1;
  }

  // Register a callback for received messages
  reader->SetDataCallback(HandleMessage);

  // Generate random data to publish
  std::vector<uint8_t> data(message_size);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 255);

  for (auto& byte : data) {
    byte = static_cast<uint8_t>(distrib(gen));
  }

  std::cout << "Using transport: " << transport_str << std::endl;
  std::cout << "Publishing " << num_messages << " messages of size " << message_size
            << " bytes every " << publish_interval_ms << "ms..." << std::endl;

  // Publish messages
  for (int i = 0; i < num_messages; ++i) {
    // Update the first byte to indicate message number
    data[0] = static_cast<uint8_t>(i);

    // Publish the message
    if (writer->Write(data.data(), data.size())) {
      std::cout << "Published message " << i << std::endl;
    } else {
      std::cerr << "Failed to publish message " << i << std::endl;
    }

    // Wait for the specified interval
    std::this_thread::sleep_for(std::chrono::milliseconds(publish_interval_ms));
  }

  std::cout << "Done publishing messages. Waiting for a moment to receive any remaining messages..."
            << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));

  return 0;
}