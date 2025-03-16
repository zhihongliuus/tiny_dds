package(default_visibility = ["//visibility:public"])

exports_files([
    "MODULE.bazel",
]) 

# Main library target that combines all components
cc_library(
    name = "tiny_dds",
    visibility = ["//visibility:public"],
    deps = [
        "//include/tiny_dds:headers",
        "//src/api",
        "//src/core",
        "//src/config",
        "//src/serialization",
        "//src/transport",
    ],
)

# # Build all binary targets in the project
# filegroup(
#     name = "binaries",
#     srcs = [
#         "//examples:simple_pub_sub",
#         "//examples:protobuf_pub_sub",
#         "//examples:yaml_config_example",
#         "//examples:shared_memory_example",
#     ],
# )

# # Build all test targets in the project
# filegroup(
#     name = "tests",
#     srcs = [
#         "//test:all",
#     ],
#     testonly = True,
# )

# # Build all targets in the project
# filegroup(
#     name = "all",
#     srcs = [
#         ":binaries",
#         ":tests",
#         ":tiny_dds",
#     ],
#     testonly = True,
# ) 
