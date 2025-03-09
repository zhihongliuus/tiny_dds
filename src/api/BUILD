load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "api",
    srcs = glob(["*.cc"], allow_empty = True),
    hdrs = glob(["*.h"], allow_empty = True),
    visibility = ["//visibility:public"],
    deps = [
        "//include/tiny_dds:headers",
        "//src/core",
        "//src/config",
        "//src/serialization",
        "//src/transport",
    ],
) 