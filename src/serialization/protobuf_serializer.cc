#include "src/serialization/protobuf_serializer.h"

#include <cstdint>

namespace tiny_dds::serialization {

auto ProtobufSerializer::Serialize(const google::protobuf::Message& message)
    -> std::vector<uint8_t> {
  std::vector<uint8_t> result;

  // Reserve space for the serialized message
  const size_t size = message.ByteSizeLong();
  result.resize(size);

  // Serialize the message
  message.SerializeToArray(result.data(), static_cast<int>(size));

  return result;
}

auto ProtobufSerializer::Deserialize(const void* data, size_t size,
                                     google::protobuf::Message* message) -> bool {
  if (data == nullptr || message == nullptr) {
    return false;
  }

  // Deserialize the message
  return message->ParseFromArray(data, static_cast<int>(size));
}

auto ProtobufSerializer::GetTypeName(const google::protobuf::Message& message) -> std::string {
  return message.GetDescriptor()->full_name();
}

}  // namespace tiny_dds::serialization