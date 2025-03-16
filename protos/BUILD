package(default_visibility = ["//visibility:public"])

load("@rules_proto//proto:defs.bzl", "proto_library")
load("@protobuf//bazel:cc_proto_library.bzl", "cc_proto_library")

proto_library(
    name = "tiny_dds_messages_proto",
    srcs = glob(["*.proto"]),
)

cc_proto_library(
    name = "tiny_dds_messages_cc_proto",
    deps = [":tiny_dds_messages_proto"],
) 