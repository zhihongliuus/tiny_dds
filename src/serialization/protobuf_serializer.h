#ifndef TINY_DDS_SERIALIZATION_PROTOBUF_SERIALIZER_H_
#define TINY_DDS_SERIALIZATION_PROTOBUF_SERIALIZER_H_

#include <memory>
#include <string>
#include <vector>

#include "google/protobuf/message.h"

namespace tiny_dds {
namespace serialization {

/**
 * @brief Serializer for Protocol Buffers messages.
 * 
 * This class provides methods to serialize and deserialize Protocol Buffers
 * messages for use with the Tiny DDS system.
 */
class ProtobufSerializer {
public:
    /**
     * @brief Serializes a Protocol Buffers message to a byte vector.
     * @param message The message to serialize.
     * @return A vector containing the serialized message.
     */
    static std::vector<uint8_t> Serialize(const google::protobuf::Message& message);

    /**
     * @brief Deserializes a byte vector to a Protocol Buffers message.
     * @param data Pointer to the serialized data.
     * @param size Size of the serialized data.
     * @param message The message to deserialize into.
     * @return True if deserialization was successful, false otherwise.
     */
    static bool Deserialize(const void* data, size_t size, google::protobuf::Message* message);

    /**
     * @brief Gets the type name for a Protocol Buffers message.
     * @param message The message to get the type name for.
     * @return The type name of the message.
     */
    static std::string GetTypeName(const google::protobuf::Message& message);
};

} // namespace serialization
} // namespace tiny_dds

#endif // TINY_DDS_SERIALIZATION_PROTOBUF_SERIALIZER_H_ 