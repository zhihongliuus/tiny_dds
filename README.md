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
- `include/` - Public headers
- `test/` - Test cases using Google Test
- `examples/` - Example applications
- `protos/` - Protocol Buffer definitions

## License

MIT 