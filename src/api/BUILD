package(default_visibility = ["//visibility:public"])

cc_library(
    name = "api",
    srcs = glob(["*.cc"], allow_empty = True),
    hdrs = glob(["*.h"], allow_empty = True),
    deps = [
        "//include/tiny_dds:tiny_dds_headers",
        "//src/core",
        "//src/serialization",
        "//src/transport",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/strings",
    ],
) 