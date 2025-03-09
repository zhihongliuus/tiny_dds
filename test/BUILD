package(default_visibility = ["//visibility:public"])

cc_test(
    name = "domain_participant_test",
    srcs = ["domain_participant_test.cc"],
    deps = [
        "//include/tiny_dds:tiny_dds_headers",
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
        "//include/tiny_dds:tiny_dds_headers",
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
        "//include/tiny_dds:tiny_dds_headers",
        "//src/serialization",
        "@googletest//:gtest_main",
        "@protobuf//:protobuf",
    ],
) 