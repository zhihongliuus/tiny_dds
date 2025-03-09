#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "include/tiny_dds/domain_participant.h"
#include "include/tiny_dds/publisher.h"
#include "include/tiny_dds/subscriber.h"
#include "include/tiny_dds/topic.h"
#include "include/tiny_dds/transport_types.h"
#include "include/tiny_dds/data_reader.h"
#include "include/tiny_dds/data_writer.h"

// Message reception callback
void HandleMessage(tiny_dds::DomainId domain_id, const std::string& topic_name, 
                  const void* data, size_t size) {
    std::cout << "Received message on topic '" << topic_name << "' in domain " 
              << domain_id << " (size: " << size << " bytes)" << std::endl;
    
    // Print first few bytes of the message
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    std::cout << "Message data: ";
    for (size_t i = 0; i < std::min(size, static_cast<size_t>(16)); ++i) {
        std::cout << static_cast<int>(bytes[i]) << " ";
    }
    std::cout << "..." << std::endl;
}

int main() {
    const std::string topic_name = "SharedMemoryTopic";
    const int domain_id = 0;
    const int num_messages = 5;
    const int publish_interval_ms = 1000;
    const int message_size = 100;
    
    std::cout << "Starting shared memory transport example" << std::endl;
    std::cout << "Domain ID: " << domain_id << std::endl;
    std::cout << "Topic name: " << topic_name << std::endl;
    std::cout << "Number of messages: " << num_messages << std::endl;
    std::cout << "Message size: " << message_size << " bytes" << std::endl;
    std::cout << "Publish interval: " << publish_interval_ms << " ms" << std::endl;
    
    // Create domain participant
    auto participant = tiny_dds::DomainParticipant::Create(domain_id, "SharedMemoryParticipant");
    
    // Set transport type to shared memory
    participant->SetTransportType(tiny_dds::TransportType::SHARED_MEMORY);
    
    // Create a topic
    auto topic = participant->CreateTopic(topic_name, "RawData");
    
    // Create publisher and subscriber
    auto publisher = participant->CreatePublisher();
    auto subscriber = participant->CreateSubscriber();
    
    // Create data writer and reader
    auto writer = publisher->CreateDataWriter(topic);
    auto reader = subscriber->CreateDataReader(topic);
    
    // Set up data reception callback
    reader->SetDataCallback(HandleMessage);
    
    // Generate random data to publish
    std::vector<uint8_t> data(message_size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 255);
    
    for (auto& byte : data) {
        byte = static_cast<uint8_t>(distrib(gen));
    }
    
    // Publish messages
    for (int i = 0; i < num_messages; ++i) {
        // Update the first byte to indicate message number
        data[0] = static_cast<uint8_t>(i);
        
        // Publish the message
        std::cout << "Publishing message " << i << std::endl;
        if (writer->Write(data.data(), data.size())) {
            std::cout << "Published successfully" << std::endl;
        } else {
            std::cerr << "Failed to publish message" << std::endl;
        }
        
        // Wait before publishing the next message
        std::this_thread::sleep_for(std::chrono::milliseconds(publish_interval_ms));
    }
    
    // Wait for any remaining messages to be received
    std::cout << "Waiting for any remaining messages..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::cout << "Example completed successfully" << std::endl;
    return 0;
} 