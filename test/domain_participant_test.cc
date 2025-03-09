#include <memory>
#include <string>

#include "gtest/gtest.h"

#include "include/tiny_dds/domain_participant.h"
#include "include/tiny_dds/publisher.h"
#include "include/tiny_dds/subscriber.h"
#include "include/tiny_dds/topic.h"
#include "include/tiny_dds/types.h"

namespace tiny_dds {
namespace {

class DomainParticipantTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a domain participant for testing
        participant_ = DomainParticipant::Create(42, "test_participant");
    }

    void TearDown() override {
        // Clean up
        participant_.reset();
    }

    std::shared_ptr<DomainParticipant> participant_;
};

TEST_F(DomainParticipantTest, CreateParticipant) {
    ASSERT_NE(participant_, nullptr);
    EXPECT_EQ(participant_->GetDomainId(), 42);
    EXPECT_EQ(participant_->GetName(), "test_participant");
}

TEST_F(DomainParticipantTest, CreatePublisher) {
    auto publisher = participant_->CreatePublisher();
    ASSERT_NE(publisher, nullptr);
}

TEST_F(DomainParticipantTest, CreateSubscriber) {
    auto subscriber = participant_->CreateSubscriber();
    ASSERT_NE(subscriber, nullptr);
}

TEST_F(DomainParticipantTest, CreateTopic) {
    const std::string topic_name = "test_topic";
    const std::string type_name = "test_type";
    
    auto topic = participant_->CreateTopic(topic_name, type_name);
    ASSERT_NE(topic, nullptr);
    EXPECT_EQ(topic->GetName(), topic_name);
    EXPECT_EQ(topic->GetTypeName(), type_name);
}

TEST_F(DomainParticipantTest, CreateDuplicateTopic) {
    const std::string topic_name = "test_topic";
    const std::string type_name = "test_type";
    
    auto topic1 = participant_->CreateTopic(topic_name, type_name);
    ASSERT_NE(topic1, nullptr);
    
    // Creating a topic with the same name and type should return the same topic
    auto topic2 = participant_->CreateTopic(topic_name, type_name);
    ASSERT_NE(topic2, nullptr);
    EXPECT_EQ(topic1, topic2);
    
    // Creating a topic with the same name but different type should fail
    auto topic3 = participant_->CreateTopic(topic_name, "different_type");
    EXPECT_EQ(topic3, nullptr);
}

} // namespace
} // namespace tiny_dds 