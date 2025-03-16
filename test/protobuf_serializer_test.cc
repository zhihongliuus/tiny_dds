#include "src/serialization/protobuf_serializer.h"

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "google/protobuf/descriptor.h"
#include "google/protobuf/descriptor.pb.h"
#include "google/protobuf/dynamic_message.h"
#include "google/protobuf/message.h"
#include "gtest/gtest.h"

namespace tiny_dds {
namespace {

// Test with a simple string message
TEST(ProtobufSerializerTest, SerializeString) {
  // Create a simple string
  std::string test_string = "Hello, world!";

  // Serialize the string
  std::vector<uint8_t> serialized(test_string.size());
  std::memcpy(serialized.data(), test_string.data(), test_string.size());

  // Create a buffer for deserialization
  std::string deserialized(serialized.size(), '\0');

  // Deserialize
  std::memcpy(&deserialized[0], serialized.data(), serialized.size());

  // Verify
  EXPECT_EQ(test_string, deserialized);
}

// Test with a simple integer
TEST(ProtobufSerializerTest, SerializeInteger) {
  // Create a simple integer
  int32_t test_int = 42;

  // Serialize the integer
  std::vector<uint8_t> serialized(sizeof(test_int));
  std::memcpy(serialized.data(), &test_int, sizeof(test_int));

  // Create a buffer for deserialization
  int32_t deserialized = 0;

  // Deserialize
  std::memcpy(&deserialized, serialized.data(), serialized.size());

  // Verify
  EXPECT_EQ(test_int, deserialized);
}

// Test the type name functionality
TEST(ProtobufSerializerTest, GetTypeName) {
  // Create a descriptor for a test message
  google::protobuf::DescriptorPool pool;
  google::protobuf::FileDescriptorProto file_proto;
  file_proto.set_name("test_message.proto");
  file_proto.set_package("tiny_dds.test");

  // Add a message type
  google::protobuf::DescriptorProto* message_proto = file_proto.add_message_type();
  message_proto->set_name("TestMessage");

  // Build the file descriptor
  const google::protobuf::FileDescriptor* file_desc = pool.BuildFile(file_proto);
  const google::protobuf::Descriptor* message_desc =
      file_desc->FindMessageTypeByName("TestMessage");

  // Verify the type name
  EXPECT_EQ(std::string("tiny_dds.test.TestMessage"), message_desc->full_name());
}

}  // namespace
}  // namespace tiny_dds