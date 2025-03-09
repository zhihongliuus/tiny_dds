load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "shared_memory_transport_test",
    srcs = ["shared_memory_transport_test.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/transport",
        "@googletest//:gtest_main",
    ],
) 