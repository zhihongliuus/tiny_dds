package(default_visibility = ["//visibility:public"])

cc_library(
    name = "config",
    srcs = glob(["*.cc"], allow_empty = True),
    hdrs = glob(["*.h"], allow_empty = True),
    deps = [
        "//include/tiny_dds:tiny_dds_headers",
        "@abseil-cpp//absl/base:core_headers",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/strings",
        "@yaml_cpp//:yaml-cpp",
    ],
) 