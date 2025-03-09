# Tiny DDS

A lightweight Data Distribution Service (DDS) implementation for educational purposes. This project is intended for learning and is not designed for production use.

## Features

- Simple implementation of core DDS concepts (Domain, Participant, Topic, Publisher, Subscriber)
- Support for multiple transport methods:
  - UDP for network communication
  - Shared memory for high-speed local communication
  - Local-only for in-process communication
- Configurable QoS (Quality of Service) settings
- YAML-based configuration for easy setup

## Usage

### Basic Usage

```cpp
#include "include/tiny_dds/domain_participant.h"
#include "include/tiny_dds/publisher.h"
#include "include/tiny_dds/subscriber.h"
#include "include/tiny_dds/topic.h"

// Create a domain participant
auto participant = tiny_dds::DomainParticipant::Create(0, "MyParticipant");

// Create a topic
auto topic = participant->CreateTopic("MyTopic", "MyType");

// Create publisher and subscriber
auto publisher = participant->CreatePublisher();
auto subscriber = participant->CreateSubscriber();

// Create data writer and reader
auto writer = publisher->CreateDataWriter(topic);
auto reader = subscriber->CreateDataReader(topic);

// Set up data reception callback
reader->SetDataCallback([](tiny_dds::DomainId domain_id, const std::string& topic_name, 
                         const void* data, size_t size) {
    // Handle received data
});

// Write data
std::vector<uint8_t> data = {1, 2, 3, 4, 5};
writer->Write(data.data(), data.size());
```

### YAML Configuration

You can define your entire DDS application structure in a YAML file:

```yaml
participants:
  - name: "ExampleParticipant"
    domain_id: 0
    
    topics:
      - name: "Example Topic"
        type_name: "ExampleMessage"
        qos:
          reliability: "RELIABLE"
          durability: "TRANSIENT_LOCAL"
    
    publishers:
      - name: "ExamplePublisher"
        qos:
          reliability: "RELIABLE"
          durability: "TRANSIENT_LOCAL"
        transport:
          type: "SHARED_MEMORY"
          buffer_size: 1048576  # 1MB buffer
          max_message_size: 65536  # 64KB max message size
        topic_names:
          - "Example Topic"
    
    subscribers:
      - name: "ExampleSubscriber"
        qos:
          reliability: "RELIABLE"
          durability: "TRANSIENT_LOCAL"
        transport:
          type: "SHARED_MEMORY"
          buffer_size: 1048576  # 1MB buffer
          max_message_size: 65536  # 64KB max message size
        topic_names:
          - "Example Topic"
```

Then load and use the configuration in your code:

```cpp
#include "include/tiny_dds/auto_config.h"

// Load DDS entities from YAML file
auto loader = tiny_dds::auto_config::AutoConfigLoader::Create();
loader->LoadFromFile("config.yaml");

// Get participant
auto participant = loader->GetParticipant("ExampleParticipant");

// Get publisher and subscriber
auto publisher = loader->GetPublisher("ExampleParticipant", "ExamplePublisher");
auto subscriber = loader->GetSubscriber("ExampleParticipant", "ExampleSubscriber");

// Get topic
auto topic = loader->GetTopic("ExampleParticipant", "Example Topic");

// Create data writer and reader
auto writer = publisher->CreateDataWriter(*topic);
auto reader = subscriber->CreateDataReader(*topic);

// Use writer and reader as normal...
```

### Transport Types

Tiny DDS supports multiple transport mechanisms:

1. **UDP** - Network communication using UDP sockets
   - Suitable for distributed applications
   - Configurable via address and port settings

2. **SHARED_MEMORY** - High-speed local communication
   - Much faster than UDP for processes on the same machine
   - Uses shared memory regions and semaphores for synchronization
   - Configurable buffer sizes for performance tuning

3. **LOCAL_ONLY** - In-process communication
   - Fastest option when publishers and subscribers are in the same process
   - No serialization overhead

You can specify the transport type in YAML configuration:

```yaml
transport:
  type: "SHARED_MEMORY"  # or "UDP" or "LOCAL_ONLY"
  buffer_size: 1048576   # for SHARED_MEMORY (1MB)
  max_message_size: 65536  # for SHARED_MEMORY (64KB)
  address: "127.0.0.1"   # for UDP
  port: 7400             # for UDP
```

Or in code:

```cpp
// When using the auto-configuration approach
loader->LoadFromFile("config.yaml");

// When creating entities manually
auto participant = tiny_dds::DomainParticipant::Create(0, "MyParticipant");
participant->SetTransportType(tiny_dds::TransportType::SHARED_MEMORY);
```

## Running Examples

```bash
# Run the YAML configuration example with shared memory transport
bazel run //examples:yaml_config_example -- --config_file=examples/config/dds_config.yaml --transport=SHARED_MEMORY

# Run the YAML configuration example with UDP transport
bazel run //examples:yaml_config_example -- --config_file=examples/config/dds_config.yaml --transport=UDP
```

## Building

```bash
bazel build //...
```

## Testing

```bash
bazel test //test:all
```

## License

This project is licensed under the MIT License - see the LICENSE file for details. 