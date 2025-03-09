# Tiny DDS

A lightweight, high-performance Data Distribution Service (DDS) implementation in C++.

## Overview

Tiny DDS is a from-scratch implementation of the Data Distribution Service (DDS) standard. It provides a publish-subscribe communication model for distributed systems with a focus on performance and reliability.

**Note:** This project is primarily for study purposes, designed to help understand the core concepts and architecture of DDS systems. It is not intended for production use without further development and testing.

## Features

- Efficient publish-subscribe messaging pattern
- Protocol Buffers (protobuf) message serialization
- Lightweight implementation with minimal dependencies
- High-performance data distribution
- Comprehensive test coverage
- YAML-based configuration for easy DDS entity setup

## Building

This project uses Bazel as its build system with bzlmod for dependency management.

```bash
# Build the entire project
bazel build //...

# Run all tests
bazel test //...
```

## Project Structure

- `src/` - Source code for the Tiny DDS implementation
  - `core/` - Core DDS functionality
  - `transport/` - Transport layer implementation
  - `serialization/` - Message serialization handling
  - `api/` - Public API
  - `config/` - YAML configuration and auto-loading functionality
- `include/` - Public headers
- `test/` - Test cases using Google Test
- `examples/` - Example applications
- `protos/` - Protocol Buffer definitions
- `third_party/` - Third-party dependencies and build files

## Usage Examples

### Basic Publisher-Subscriber

```cpp
// Create domain participants
auto participant = DomainParticipant::Create(0, "MyParticipant");

// Create publisher and subscriber
auto publisher = participant->CreatePublisher();
auto subscriber = participant->CreateSubscriber();

// Create a topic
auto topic = participant->CreateTopic("MyTopic", "MyDataType");

// Create data writer and reader
auto writer = publisher->CreateDataWriter(topic);
auto reader = subscriber->CreateDataReader(topic);

// Setup callback for data reception
reader->SetDataReceivedCallback([](const void* data, size_t size, const SampleInfo& info) {
    // Process received data
});

// Publish data
writer->Write(data_buffer, data_size);
```

### YAML Configuration

Tiny DDS supports configuring DDS entities using YAML files. This allows for easy setup and configuration without changing code.

Example YAML configuration:

```yaml
participants:
  - name: "Example Participant"
    domain_id: 0
    
    publishers:
      - name: "Example Publisher"
        qos:
          reliability: "RELIABLE"
          durability: "TRANSIENT_LOCAL"
        
        topics:
          - name: "Example Topic"
            type: "ExampleMessage"
            qos:
              reliability: "RELIABLE"
              durability: "TRANSIENT_LOCAL"
    
    subscribers:
      - name: "Example Subscriber"
        qos:
          reliability: "RELIABLE"
          durability: "TRANSIENT_LOCAL"
        
        topics:
          - name: "Example Topic"
            type: "ExampleMessage"
            qos:
              reliability: "RELIABLE"
              durability: "TRANSIENT_LOCAL"
```

Using the configuration in code:

```cpp
// Create auto config loader
auto config_loader = tiny_dds::auto_config::AutoConfigLoader::Create();

// Load DDS entities from YAML file
if (!config_loader->LoadFromFile("path/to/config.yaml")) {
    // Handle error
}

// Get entities by name
auto participant = config_loader->GetParticipant("Example Participant");
auto publisher = config_loader->GetPublisher("Example Participant", "Example Publisher");
auto subscriber = config_loader->GetSubscriber("Example Participant", "Example Subscriber");
auto topic = config_loader->GetTopic("Example Participant", "Example Topic");

// Create data writer and reader
auto data_writer = publisher->CreateDataWriter(topic);
auto data_reader = subscriber->CreateDataReader(topic);

// Use entities as usual
```

Run the YAML configuration example:

```bash
bazel run //examples:yaml_config_example -- --config_file=examples/config/dds_config.yaml
```

## License

MIT 