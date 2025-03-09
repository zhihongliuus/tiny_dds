package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_test")

test_suite(
    name = "all",
    tests = [
        ":domain_participant_test",
        ":pub_sub_test",
        ":protobuf_serializer_test",
        "//test/transport:shared_memory_transport_test",
    ],
)

cc_test(
    name = "domain_participant_test",
    srcs = ["domain_participant_test.cc"],
    deps = [
        "//include/tiny_dds:headers",
        "//src/api",
        "//src/core",
        "//src/serialization",
        "//src/transport",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "pub_sub_test",
    srcs = ["pub_sub_test.cc"],
    deps = [
        "//include/tiny_dds:headers",
        "//src/api",
        "//src/core",
        "//src/serialization",
        "//src/transport",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "protobuf_serializer_test",
    srcs = ["protobuf_serializer_test.cc"],
    deps = [
        "//include/tiny_dds:headers",
        "//src/serialization",
        "@googletest//:gtest_main",
        "@protobuf//:protobuf",
    ],
) 