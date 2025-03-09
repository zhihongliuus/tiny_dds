#ifndef TINY_DDS_TRANSPORT_TYPES_H_
#define TINY_DDS_TRANSPORT_TYPES_H_

#include <string>

namespace tiny_dds {

/**
 * @brief Enumeration of transport types supported by Tiny DDS.
 */
enum class TransportType {
    UDP,             ///< UDP transport (default)
    SHARED_MEMORY,   ///< Shared memory transport for local communication
    // Add more transport types as needed
};

/**
 * @brief Convert a string to a transport type.
 * 
 * @param str The string representation.
 * @return TransportType The corresponding transport type.
 */
inline TransportType StringToTransportType(const std::string& str) {
    if (str == "SHARED_MEMORY") {
        return TransportType::SHARED_MEMORY;
    } else {
        // Default to UDP for unknown strings
        return TransportType::UDP;
    }
}

/**
 * @brief Get a string representation of a transport type.
 * 
 * @param type The transport type.
 * @return std::string The string representation.
 */
inline std::string TransportTypeToString(TransportType type) {
    switch (type) {
        case TransportType::UDP:
            return "UDP";
        case TransportType::SHARED_MEMORY:
            return "SHARED_MEMORY";
        default:
            return "UNKNOWN";
    }
}

} // namespace tiny_dds

#endif // TINY_DDS_TRANSPORT_TYPES_H_ 