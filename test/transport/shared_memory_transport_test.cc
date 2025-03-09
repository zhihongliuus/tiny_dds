#include <chrono>
#include <cstring>
#include <memory>
#include <string>
#include <thread>

#include "gtest/gtest.h"

#include "include/tiny_dds/types.h"
#include "include/tiny_dds/transport.h"
#include "include/tiny_dds/transport_types.h"
#include "src/transport/shared_memory_transport.h"

namespace tiny_dds {
namespace transport {
namespace {

class SharedMemoryTransportTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create transports for testing
        writer_transport_ = SharedMemoryTransport::Create(
            0, "writer_participant", 1024 * 1024, 64 * 1024);
        reader_transport_ = SharedMemoryTransport::Create(
            0, "reader_participant", 1024 * 1024, 64 * 1024);
        
        ASSERT_TRUE(writer_transport_ != nullptr);
        ASSERT_TRUE(reader_transport_ != nullptr);
    }

    void TearDown() override {
        // Clean up
        writer_transport_.reset();
        reader_transport_.reset();
    }

    std::shared_ptr<SharedMemoryTransport> writer_transport_;
    std::shared_ptr<SharedMemoryTransport> reader_transport_;
};

TEST_F(SharedMemoryTransportTest, BasicFunctionality) {
    const std::string topic_name = "TestTopic";
    
    // Writer advertises the topic
    EXPECT_TRUE(writer_transport_->Advertise(topic_name));
    
    // Reader subscribes to the topic
    EXPECT_TRUE(reader_transport_->Subscribe(topic_name));
    
    // Prepare test data
    const char test_data[] = "Hello, Shared Memory!";
    const size_t data_size = strlen(test_data) + 1;  // +1 for null terminator
    
    // Write data
    EXPECT_TRUE(writer_transport_->Send(topic_name, test_data, data_size));
    
    // Give some time for data to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Read data
    char buffer[1024] = {0};
    size_t bytes_received = 0;
    EXPECT_TRUE(reader_transport_->Receive(topic_name, buffer, sizeof(buffer), &bytes_received));
    
    // Verify data
    EXPECT_GT(bytes_received, 0);
    EXPECT_EQ(bytes_received, data_size);
    EXPECT_STREQ(buffer, test_data);
}

TEST_F(SharedMemoryTransportTest, TransportTypeCheck) {
    // Verify the transport type is correctly identified
    EXPECT_EQ(writer_transport_->GetType(), TransportType::SHARED_MEMORY);
    EXPECT_EQ(reader_transport_->GetType(), TransportType::SHARED_MEMORY);
}

}  // namespace
}  // namespace transport
}  // namespace tiny_dds 