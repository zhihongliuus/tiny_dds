package(default_visibility = ["//visibility:public"])

load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_proto_library")

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
        "//include/tiny_dds:tiny_dds_headers",
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
    name = "yaml_config_example",
    srcs = ["yaml_config_example.cc"],
    deps = [
        "//include/tiny_dds:tiny_dds_headers",
        "//src/api",
        "//src/config",
        "//src/core",
        "//src/serialization",
        "//src/transport",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/time",
    ],
    data = [
        "config/dds_config.yaml",
    ],
) 