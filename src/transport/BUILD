package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "transport",
    srcs = [
        "udp_transport.cc",
        "shared_memory_transport.cc",
        "transport_manager.cc",
    ],
    hdrs = [
        "udp_transport.h",
        "shared_memory_transport.h",
        "transport_manager.h",
    ],
    deps = [
        "//include/tiny_dds:transport",
        "//include/tiny_dds:transport_types",
        "//include/tiny_dds:types",
    ],
    visibility = ["//visibility:public"],
) 