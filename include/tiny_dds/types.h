#ifndef TINY_DDS_TYPES_H_
#define TINY_DDS_TYPES_H_

#include <cstdint>
#include <string>

namespace tiny_dds {

// Domain identifier type
using DomainId = std::uint32_t;

// Quality of Service related types
enum class ReliabilityKind { BEST_EFFORT, RELIABLE };

enum class DurabilityKind { VOLATILE, TRANSIENT_LOCAL, TRANSIENT, PERSISTENT };

// Common status types
struct SubscriptionMatchedStatus {
  std::int32_t total_count = 0;
  std::int32_t total_count_change = 0;
  std::int32_t current_count = 0;
  std::int32_t current_count_change = 0;
};

struct PublicationMatchedStatus {
  std::int32_t total_count = 0;
  std::int32_t total_count_change = 0;
  std::int32_t current_count = 0;
  std::int32_t current_count_change = 0;
};

// Sample information
struct SampleInfo {
  bool valid_data = false;
  // Add other relevant fields like timestamp, sample state, etc.
};

}  // namespace tiny_dds

#endif  // TINY_DDS_TYPES_H_