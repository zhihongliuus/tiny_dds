load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "config",
    srcs = glob(["*.cc"], allow_empty = True),
    hdrs = glob(["*.h"], allow_empty = True),
    visibility = ["//visibility:public"],
    deps = [
        "//include/tiny_dds:headers",
        "//src/core",
        "//src/serialization",
        "//src/transport",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/strings",
        "@yaml-cpp//:yaml-cpp",
    ],
) 