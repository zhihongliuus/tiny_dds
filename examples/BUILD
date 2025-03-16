package(default_visibility = ["//visibility:public"])

load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@protobuf//bazel:cc_proto_library.bzl", "cc_proto_library")

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
        "//include/tiny_dds:headers",
        "//src/api",
        "//src/core",
        "//src/serialization",
        "//src/transport",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/time",
    ],
)

cc_binary(
    name = "protobuf_pub_sub",
    srcs = ["protobuf_pub_sub.cc"],
    deps = [
        ":example_message_cc_proto",
        "//include/tiny_dds:headers",
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
    name = "yaml_config_example",
    srcs = ["yaml_config_example.cc"],
    deps = [
        "//src/api:api",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/time",
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