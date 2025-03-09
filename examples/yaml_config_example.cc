#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
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
#include "include/tiny_dds/types.h"

ABSL_FLAG(std::string, config_file, "", "Path to the YAML configuration file");
ABSL_FLAG(int, num_messages, 5, "Number of messages to publish");
ABSL_FLAG(int, publish_interval_ms, 1000, "Interval between published messages in milliseconds");
ABSL_FLAG(int, message_size, 100, "Size of each message in bytes");

int main(int argc, char* argv[]) {
    absl::ParseCommandLine(argc, argv);
    
    std::string config_file = absl::GetFlag(FLAGS_config_file);
    int num_messages = absl::GetFlag(FLAGS_num_messages);
    int publish_interval_ms = absl::GetFlag(FLAGS_publish_interval_ms);
    int message_size = absl::GetFlag(FLAGS_message_size);
    
    if (config_file.empty()) {
        std::cerr << "Error: Configuration file must be specified with --config_file" << std::endl;
        return 1;
    }
    
    std::cout << "Starting YAML configuration example" << std::endl;
    std::cout << "Configuration file: " << config_file << std::endl;
    std::cout << "Number of messages: " << num_messages << std::endl;
    std::cout << "Message size: " << message_size << " bytes" << std::endl;
    std::cout << "Publish interval: " << publish_interval_ms << " ms" << std::endl;
    
    // Create auto config loader
    auto config_loader = tiny_dds::auto_config::AutoConfigLoader::Create();
    
    // Load DDS entities from YAML file
    if (!config_loader->LoadFromFile(config_file)) {
        std::cerr << "Failed to load DDS entities from configuration file" << std::endl;
        return 1;
    }
    
    // Get first domain participant
    auto participants = config_loader->GetParticipants();
    if (participants.empty()) {
        std::cerr << "No domain participants found in configuration" << std::endl;
        return 1;
    }
    
    auto participant = participants[0];
    std::cout << "Using domain participant: " << participant->GetName() << std::endl;
    
    // Get first publisher
    auto publisher = config_loader->GetPublisher(participant->GetName(), "Example Publisher");
    if (!publisher) {
        std::cerr << "Publisher 'Example Publisher' not found in configuration" << std::endl;
        return 1;
    }
    
    // Get first subscriber
    auto subscriber = config_loader->GetSubscriber(participant->GetName(), "Example Subscriber");
    if (!subscriber) {
        std::cerr << "Subscriber 'Example Subscriber' not found in configuration" << std::endl;
        return 1;
    }
    
    // Get topic
    auto topic = config_loader->GetTopic(participant->GetName(), "Example Topic");
    if (!topic) {
        std::cerr << "Topic 'Example Topic' not found in configuration" << std::endl;
        return 1;
    }
    
    // Create data writer and reader
    auto data_writer = publisher->CreateDataWriter(topic);
    auto data_reader = subscriber->CreateDataReader(topic);
    
    // Set up data reader callback
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
    
    // Publish messages
    std::vector<uint8_t> message_data(message_size);
    for (int i = 0; i < num_messages; ++i) {
        std::cout << "Publishing message " << (i + 1) << " of " << num_messages << std::endl;
        
        // Fill message with data
        for (int j = 0; j < message_size; ++j) {
            message_data[j] = static_cast<uint8_t>((i + j) % 256);
        }
        
        // Publish the message
        bool success = data_writer->Write(message_data.data(), message_data.size());
        if (success) {
            std::cout << "Published successfully" << std::endl;
        } else {
            std::cerr << "Failed to publish message" << std::endl;
        }
        
        // Wait before publishing the next message
        if (i < num_messages - 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(publish_interval_ms));
        }
    }
    
    // Wait for all messages to be received
    std::cout << "Waiting for all messages to be received..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    std::cout << "Example completed successfully" << std::endl;
    return 0;
} 