package(default_visibility = ["//visibility:public"])

load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_proto_library", "cc_binary")

proto_library(
    name = "example_message_proto",
    srcs = ["example_message.proto"],
)

cc_proto_library(
    name = "example_message_cc_proto",
    deps = [":example_message_proto"],
)

cc_binary(
    name = "simple_pub_sub",
    srcs = ["simple_pub_sub.cc"],
    deps = [
        ":example_message_cc_proto",
        "//include/tiny_dds:headers",
        "//src/api",
        "//src/core",
        "//src/serialization",
        "//src/transport",
    ],
)

cc_binary(
    name = "protobuf_pub_sub",
    srcs = ["protobuf_pub_sub.cc"],
    deps = [
        ":example_message_cc_proto",
        "//include/tiny_dds:tiny_dds_headers",
        "//src/api",
        "//src/core",
        "//src/serialization",
        "//src/transport",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/time",
        "@protobuf//:protobuf",
    ],
)

cc_binary(
    name = "publisher_example",
    srcs = ["publisher_example.cc"],
    deps = [
        "//src/api:api",
    ],
)

cc_binary(
    name = "subscriber_example",
    srcs = ["subscriber_example.cc"],
    deps = [
        "//src/api:api",
    ],
)

cc_binary(
    name = "yaml_config_example",
    srcs = ["yaml_config_example.cc"],
    deps = [
        "//src/api:api",
    ],
    data = [
        "//examples/config:dds_config.yaml",
    ],
)

cc_binary(
    name = "shared_memory_example",
    srcs = ["shared_memory_example.cc"],
    deps = [
        "//src/api:api",
    ],
) 