#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <cstring>

#include "gtest/gtest.h"

#include "include/tiny_dds/data_reader.h"
#include "include/tiny_dds/data_writer.h"
#include "include/tiny_dds/domain_participant.h"
#include "include/tiny_dds/publisher.h"
#include "include/tiny_dds/subscriber.h"
#include "include/tiny_dds/topic.h"
#include "include/tiny_dds/types.h"

namespace tiny_dds {
namespace {

class PubSubTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create domain participants for testing
        publisher_participant_ = DomainParticipant::Create(42, "publisher_participant");
        subscriber_participant_ = DomainParticipant::Create(42, "subscriber_participant");
        
        // Create publisher and subscriber
        publisher_ = publisher_participant_->CreatePublisher();
        subscriber_ = subscriber_participant_->CreateSubscriber();
        
        // Create topics
        publisher_topic_ = publisher_participant_->CreateTopic("test_topic", "test_type");
        subscriber_topic_ = subscriber_participant_->CreateTopic("test_topic", "test_type");
        
        // Create data writer and reader
        data_writer_ = publisher_->CreateDataWriter(publisher_topic_);
        data_reader_ = subscriber_->CreateDataReader(subscriber_topic_);
    }

    void TearDown() override {
        // Clean up
        data_writer_.reset();
        data_reader_.reset();
        publisher_topic_.reset();
        subscriber_topic_.reset();
        publisher_.reset();
        subscriber_.reset();
        publisher_participant_.reset();
        subscriber_participant_.reset();
    }

    std::shared_ptr<DomainParticipant> publisher_participant_;
    std::shared_ptr<DomainParticipant> subscriber_participant_;
    std::shared_ptr<Publisher> publisher_;
    std::shared_ptr<Subscriber> subscriber_;
    std::shared_ptr<Topic> publisher_topic_;
    std::shared_ptr<Topic> subscriber_topic_;
    std::shared_ptr<DataWriter> data_writer_;
    std::shared_ptr<DataReader> data_reader_;
};

TEST_F(PubSubTest, CreateEntities) {
    ASSERT_NE(publisher_participant_, nullptr);
    ASSERT_NE(subscriber_participant_, nullptr);
    ASSERT_NE(publisher_, nullptr);
    ASSERT_NE(subscriber_, nullptr);
    ASSERT_NE(publisher_topic_, nullptr);
    ASSERT_NE(subscriber_topic_, nullptr);
    ASSERT_NE(data_writer_, nullptr);
    ASSERT_NE(data_reader_, nullptr);
}

TEST_F(PubSubTest, WriteAndRead) {
    // Test data
    const std::vector<uint8_t> test_data = {1, 2, 3, 4, 5};
    
    // Write data
    ASSERT_TRUE(data_writer_->Write(test_data.data(), test_data.size()));
    
    // Give some time for the data to be delivered
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Read data
    std::vector<uint8_t> read_buffer(test_data.size());
    SampleInfo info;
    int32_t bytes_read = data_reader_->Read(read_buffer.data(), read_buffer.size(), info);
    
    // Verify data
    ASSERT_GT(bytes_read, 0);
    EXPECT_EQ(static_cast<size_t>(bytes_read), test_data.size());
    EXPECT_TRUE(info.valid_data);
    EXPECT_EQ(read_buffer, test_data);
}

TEST_F(PubSubTest, WriteAndTake) {
    // Test data
    const std::vector<uint8_t> test_data = {6, 7, 8, 9, 10};
    
    // Write data
    ASSERT_TRUE(data_writer_->Write(test_data.data(), test_data.size()));
    
    // Give some time for the data to be delivered
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Take data
    std::vector<uint8_t> take_buffer(test_data.size());
    SampleInfo info;
    int32_t bytes_read = data_reader_->Take(take_buffer.data(), take_buffer.size(), info);
    
    // Verify data
    ASSERT_GT(bytes_read, 0);
    EXPECT_EQ(static_cast<size_t>(bytes_read), test_data.size());
    EXPECT_TRUE(info.valid_data);
    EXPECT_EQ(take_buffer, test_data);
    
    // Try to take again, should be no data
    bytes_read = data_reader_->Take(take_buffer.data(), take_buffer.size(), info);
    EXPECT_EQ(bytes_read, -1);
}

TEST_F(PubSubTest, Callback) {
    // Test data
    const std::vector<uint8_t> test_data = {11, 12, 13, 14, 15};
    
    // Variables to capture callback data
    bool callback_called = false;
    std::vector<uint8_t> callback_data;
    SampleInfo callback_info;
    
    // Set callback
    data_reader_->SetDataReceivedCallback(
        [&](const void* data, size_t size, const SampleInfo& info) {
            callback_called = true;
            callback_data.resize(size);
            memcpy(callback_data.data(), data, size);
            callback_info = info;
        });
    
    // Write data
    ASSERT_TRUE(data_writer_->Write(test_data.data(), test_data.size()));
    
    // Give some time for the callback to be called
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Verify callback was called with correct data
    EXPECT_TRUE(callback_called);
    EXPECT_EQ(callback_data, test_data);
    EXPECT_TRUE(callback_info.valid_data);
}

} // namespace
} // namespace tiny_dds 