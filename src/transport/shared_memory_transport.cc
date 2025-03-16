#include "src/transport/shared_memory_transport.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <sstream>

namespace tiny_dds::transport {

auto SharedMemoryTransport::Create(DomainId domain_id, std::string participant_name,
                                   size_t buffer_size, size_t max_message_size)
    -> std::shared_ptr<SharedMemoryTransport> {
  return std::shared_ptr<SharedMemoryTransport>(new SharedMemoryTransport(
      domain_id, std::move(participant_name), buffer_size, max_message_size));
}

SharedMemoryTransport::SharedMemoryTransport(DomainId domain_id, std::string participant_name,
                                             size_t buffer_size, size_t max_message_size)
    : domain_id_(domain_id),
      participant_name_(std::move(participant_name)),
      buffer_size_(buffer_size),
      max_message_size_(max_message_size),
      initialized_(false) {}

SharedMemoryTransport::~SharedMemoryTransport() {
  // Close all shared memory segments
  for (auto& pair : segments_) {
    CloseSegment(pair.second);
  }
}

auto SharedMemoryTransport::Initialize() -> bool {
  std::lock_guard<std::mutex> lock(mutex_);

  if (initialized_) {
    return true;
  }

  // Nothing to do here for now, initialization happens when topics are advertised or subscribed

  initialized_ = true;
  return true;
}

auto SharedMemoryTransport::Advertise(const std::string& topic_name) -> bool {
  std::lock_guard<std::mutex> lock(mutex_);

  // Check if we already have this segment
  auto it = segments_.find(topic_name);
  if (it != segments_.end()) {
    return true;  // Already advertised
  }

  // Create a new shared memory segment for this topic
  SharedMemorySegment segment(topic_name, buffer_size_);
  if (!CreateOrOpenSegment(topic_name, segment)) {
    std::cerr << "Failed to create shared memory segment for topic: " << topic_name << std::endl;
    return false;
  }

  // Initialize the ring buffer
  auto* buffer = static_cast<RingBuffer*>(segment.memory);
  buffer->write_index.store(0, std::memory_order_relaxed);
  buffer->read_index.store(0, std::memory_order_relaxed);
  buffer->buffer_size = static_cast<uint32_t>(buffer_size_);
  buffer->max_message_size = static_cast<uint32_t>(max_message_size_);

  // Store the segment
  segments_[topic_name] = segment;

  return true;
}

auto SharedMemoryTransport::Subscribe(const std::string& topic_name) -> bool {
  std::lock_guard<std::mutex> lock(mutex_);

  // Check if we already have this segment
  auto it = segments_.find(topic_name);
  if (it != segments_.end()) {
    return true;  // Already subscribed
  }

  // Create a new shared memory segment for this topic
  SharedMemorySegment segment(topic_name, buffer_size_);
  if (!CreateOrOpenSegment(topic_name, segment)) {
    std::cerr << "Failed to open shared memory segment for topic: " << topic_name << std::endl;
    return false;
  }

  // Store the segment
  segments_[topic_name] = segment;

  return true;
}

auto SharedMemoryTransport::Send(const std::string& topic_name, const void* data, size_t size)
    -> bool {
  if (size > max_message_size_) {
    std::cerr << "Message size exceeds maximum allowed size" << std::endl;
    return false;
  }

  std::lock_guard<std::mutex> lock(mutex_);

  // Find the segment for this topic
  auto it = segments_.find(topic_name);
  if (it == segments_.end()) {
    std::cerr << "Topic not found: " << topic_name << std::endl;
    return false;
  }

  // Get the ring buffer
  auto* buffer = static_cast<RingBuffer*>(it->second.memory);

  // Write the message to the ring buffer
  return WriteToRingBuffer(buffer, topic_name, data, size);
}

auto SharedMemoryTransport::Receive(const std::string& topic_name, void* buffer, size_t buffer_size,
                                    size_t* bytes_received) -> bool {
  std::lock_guard<std::mutex> lock(mutex_);

  // Find the segment for this topic
  auto it = segments_.find(topic_name);
  if (it == segments_.end()) {
    std::cerr << "Topic not found: " << topic_name << std::endl;
    return false;
  }

  // Get the ring buffer
  auto* ring_buffer = static_cast<RingBuffer*>(it->second.memory);

  // Read a message from the ring buffer
  return ReadFromRingBuffer(ring_buffer, topic_name, buffer, buffer_size, bytes_received);
}

auto SharedMemoryTransport::CreateOrOpenSegment(const std::string& topic_name,
                                                SharedMemorySegment& segment) -> bool {
  // Generate a unique name for the shared memory segment
  std::stringstream ss;
  ss << "/tiny_dds_" << domain_id_ << "_" << topic_name;
  std::string shm_name = ss.str();

  // Replace any invalid characters in the name
  for (char& c : shm_name) {
    if (isalnum(c) == 0 && c != '_' && c != '/') {
      c = '_';
    }
  }

  // Try to create the shared memory segment
  int fd = shm_open(shm_name.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    std::cerr << "Failed to open shared memory: " << strerror(errno) << std::endl;
    return false;
  }

  // Set the size of the shared memory segment
  size_t total_size = sizeof(RingBuffer) + segment.size;
  if (ftruncate(fd, static_cast<off_t>(total_size)) == -1) {
    std::cerr << "Failed to set shared memory size: " << strerror(errno) << std::endl;
    close(fd);
    return false;
  }

  // Map the shared memory segment
  void* memory = mmap(nullptr, total_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (memory == nullptr) {
    std::cerr << "Failed to map shared memory: " << strerror(errno) << std::endl;
    close(fd);
    return false;
  }

  // Close the file descriptor (the mapping remains valid)
  close(fd);

  // Update the segment
  segment.name = shm_name;
  segment.memory = memory;
  segment.size = total_size;

  return true;
}

void SharedMemoryTransport::CloseSegment(SharedMemorySegment& segment) {
  if (segment.memory != nullptr) {
    // Unmap the shared memory
    munmap(segment.memory, segment.size);
    segment.memory = nullptr;

    // Unlink the shared memory object
    shm_unlink(segment.name.c_str());
  }
}

auto SharedMemoryTransport::WriteToRingBuffer(RingBuffer* buffer, const std::string& topic_name,
                                              const void* data, size_t size) -> bool {
  // Calculate the total size needed for the message (header + data)
  size_t total_size = sizeof(MessageHeader) + size;
  if (total_size > buffer->max_message_size) {
    std::cerr << "Message size exceeds maximum allowed size" << std::endl;
    return false;
  }

  // Get the current write index
  uint32_t write_index = buffer->write_index.load(std::memory_order_relaxed);

  // Calculate the position in the buffer
  uint32_t position = write_index % buffer->buffer_size;

  // Check if there's enough space in the buffer
  uint32_t read_index = buffer->read_index.load(std::memory_order_acquire);
  uint32_t available_space;

  if (write_index >= read_index) {
    available_space = buffer->buffer_size - (write_index - read_index);
  } else {
    available_space = read_index - write_index;
  }

  if (available_space <= total_size) {
    std::cerr << "Not enough space in the ring buffer" << std::endl;
    return false;
  }

  // Prepare the message header
  MessageHeader header{};
  header.magic = MAGIC_NUMBER;
  header.sequence = write_index;
  header.size = static_cast<uint32_t>(size);
  header.checksum = 0;  // TODO(fliu): Implement checksum
  header.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count();

  // Copy the topic name and sender name
  const char* topic_name_cstr = topic_name.c_str();
  std::copy_n(topic_name_cstr, std::min(topic_name.length(), sizeof(header.topic_name) - 1),
              header.topic_name);
  header.topic_name[sizeof(header.topic_name) - 1] = '\0';

  const char* participant_name_cstr = participant_name_.c_str();
  std::copy_n(participant_name_cstr,
              std::min(participant_name_.length(), sizeof(header.sender_name) - 1),
              header.sender_name);
  header.sender_name[sizeof(header.sender_name) - 1] = '\0';

  // Write the header and data to the buffer
  size_t write_offset = position;

  // Copy header
  std::memcpy(&buffer->data[write_offset], &header, sizeof(header));

  // Copy data
  std::memcpy(&buffer->data[write_offset + sizeof(header)], data, size);

  // Update the write index
  buffer->write_index.store(write_index + total_size, std::memory_order_release);

  return true;
}

auto SharedMemoryTransport::ReadFromRingBuffer(RingBuffer* buffer, const std::string& topic_name,
                                               void* data, size_t buffer_size, size_t* bytes_read)
    -> bool {
  // Get the current read index
  uint32_t read_index = buffer->read_index.load(std::memory_order_relaxed);

  // Get the current write index
  uint32_t write_index = buffer->write_index.load(std::memory_order_acquire);

  // Check if there's data to read
  if (read_index == write_index) {
    return false;  // No data available
  }

  // Calculate the position in the buffer
  uint32_t position = read_index % buffer->buffer_size;

  // Read the header using indexing instead of pointer arithmetic
  size_t read_offset = position;

  MessageHeader header{};
  std::memcpy(&header, &buffer->data[read_offset], sizeof(header));

  // Verify the magic number
  if (header.magic != MAGIC_NUMBER) {
    std::cerr << "Invalid message header (magic number mismatch)" << std::endl;
    // Skip this message
    buffer->read_index.store(read_index + sizeof(header), std::memory_order_release);
    return false;
  }

  // Verify the topic name
  // Use std::string comparison instead of strncmp to avoid array decay
  std::string header_topic(header.topic_name,
                           strnlen(header.topic_name, sizeof(header.topic_name)));
  if (header_topic != topic_name) {
    // This message is for a different topic, skip it
    buffer->read_index.store(read_index + sizeof(header) + header.size, std::memory_order_release);
    return false;
  }

  // Check if the buffer is large enough
  if (buffer_size < header.size) {
    std::cerr << "Buffer too small to receive message" << std::endl;
    return false;
  }

  // Read the data using indexing instead of pointer arithmetic
  std::memcpy(data, &buffer->data[read_offset + sizeof(header)], header.size);

  // Update the read index
  buffer->read_index.store(read_index + sizeof(header) + header.size, std::memory_order_release);

  // Set the number of bytes read
  if (bytes_read != nullptr) {
    *bytes_read = header.size;
  }

  return true;
}

}  // namespace tiny_dds::transport