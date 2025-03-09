load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "serialization",
    srcs = glob(["*.cc"], allow_empty = True),
    hdrs = glob(["*.h"], allow_empty = True),
    visibility = ["//visibility:public"],
    deps = [
        "//include/tiny_dds:headers",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/strings",
        "@protobuf//:protobuf",
    ],
) 