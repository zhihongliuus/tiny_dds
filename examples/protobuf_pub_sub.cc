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

#include "src/serialization/protobuf_serializer.h"

// Include the generated protobuf header
#include "examples/example_message.pb.h"

ABSL_FLAG(int, domain_id, 0, "Domain ID to use for the example");
ABSL_FLAG(std::string, topic_name, "example_topic", "Topic name to use for the example");
ABSL_FLAG(int, num_messages, 10, "Number of messages to publish");
ABSL_FLAG(int, publish_interval_ms, 1000, "Interval between publishing messages in milliseconds");

int main(int argc, char* argv[]) {
    // Parse command line flags
    absl::ParseCommandLine(argc, argv);
    
    // Get flag values
    int domain_id = absl::GetFlag(FLAGS_domain_id);
    std::string topic_name = absl::GetFlag(FLAGS_topic_name);
    int num_messages = absl::GetFlag(FLAGS_num_messages);
    int publish_interval_ms = absl::GetFlag(FLAGS_publish_interval_ms);
    
    std::cout << "Starting Protocol Buffers publish/subscribe example" << std::endl;
    std::cout << "Domain ID: " << domain_id << std::endl;
    std::cout << "Topic name: " << topic_name << std::endl;
    std::cout << "Number of messages: " << num_messages << std::endl;
    std::cout << "Publish interval: " << publish_interval_ms << " ms" << std::endl;
    
    // Create domain participants
    auto publisher_participant = tiny_dds::DomainParticipant::Create(
        domain_id, "publisher_participant");
    auto subscriber_participant = tiny_dds::DomainParticipant::Create(
        domain_id, "subscriber_participant");
    
    // Create publisher and subscriber
    auto publisher = publisher_participant->CreatePublisher();
    auto subscriber = subscriber_participant->CreateSubscriber();
    
    // Get the type name for the ExampleMessage
    tiny_dds::examples::ExampleMessage example_message;
    std::string type_name = tiny_dds::serialization::ProtobufSerializer::GetTypeName(example_message);
    
    // Create topics
    auto publisher_topic = publisher_participant->CreateTopic(
        topic_name, type_name);
    auto subscriber_topic = subscriber_participant->CreateTopic(
        topic_name, type_name);
    
    // Create data writer and reader
    auto data_writer = publisher->CreateDataWriter(publisher_topic);
    auto data_reader = subscriber->CreateDataReader(subscriber_topic);
    
    // Set up data received callback
    data_reader->SetDataReceivedCallback(
        [](const void* data, size_t size, const tiny_dds::SampleInfo& info) {
            std::cout << "Received data of size " << size << " bytes" << std::endl;
            
            // Deserialize the message
            tiny_dds::examples::ExampleMessage message;
            if (tiny_dds::serialization::ProtobufSerializer::Deserialize(data, size, &message)) {
                std::cout << "Deserialized message:" << std::endl;
                std::cout << "  ID: " << message.id() << std::endl;
                std::cout << "  Text: " << message.text() << std::endl;
                std::cout << "  Value: " << message.value() << std::endl;
                std::cout << "  Data: [";
                for (int i = 0; i < message.data_size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << message.data(i);
                }
                std::cout << "]" << std::endl;
            } else {
                std::cout << "Failed to deserialize message" << std::endl;
            }
        });
    
    // Publish messages
    for (int i = 0; i < num_messages; ++i) {
        // Create and populate a message
        tiny_dds::examples::ExampleMessage message;
        message.set_id(i + 1);
        message.set_text("Message #" + std::to_string(i + 1));
        message.set_value(3.14159 * (i + 1));
        
        // Add some data
        for (int j = 0; j < 5; ++j) {
            message.add_data((i + 1) * 10 + j);
        }
        
        // Serialize the message
        std::vector<uint8_t> serialized = 
            tiny_dds::serialization::ProtobufSerializer::Serialize(message);
        
        // Write the data
        std::cout << "Publishing message " << (i + 1) << " of " << num_messages << std::endl;
        if (data_writer->Write(serialized.data(), serialized.size())) {
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