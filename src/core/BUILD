load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "core",
    srcs = glob(["*.cc"], allow_empty = True),
    hdrs = glob(["*.h"], allow_empty = True),
    visibility = ["//visibility:public"],
    deps = [
        "//include/tiny_dds:headers",
        "//src/serialization",
        "//src/transport",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/synchronization",
    ],
) 