#include "src/serialization/protobuf_serializer.h"

#include <cstdint>

namespace tiny_dds {
namespace serialization {

std::vector<uint8_t> ProtobufSerializer::Serialize(const google::protobuf::Message& message) {
    std::vector<uint8_t> result;
    
    // Reserve space for the serialized message
    const size_t size = message.ByteSizeLong();
    result.resize(size);
    
    // Serialize the message
    message.SerializeToArray(result.data(), static_cast<int>(size));
    
    return result;
}

bool ProtobufSerializer::Deserialize(const void* data, size_t size, google::protobuf::Message* message) {
    if (!data || !message) {
        return false;
    }
    
    // Deserialize the message
    return message->ParseFromArray(data, static_cast<int>(size));
}

std::string ProtobufSerializer::GetTypeName(const google::protobuf::Message& message) {
    return message.GetDescriptor()->full_name();
}

} // namespace serialization
} // namespace tiny_dds 