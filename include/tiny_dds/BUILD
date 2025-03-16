package(default_visibility = ["//visibility:public"])

cc_library(
    name = "headers",
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "transport_types",
    hdrs = ["transport_types.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "transport",
    hdrs = ["transport.h"],
    deps = [
        ":transport_types",
        ":types",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "domain_participant",
    hdrs = ["domain_participant.h"],
    deps = [
        ":publisher",
        ":subscriber",
        ":topic",
        ":transport_types",
        ":types",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "publisher",
    hdrs = ["publisher.h"],
    deps = [
        ":data_writer",
        ":topic",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "subscriber",
    hdrs = ["subscriber.h"],
    deps = [
        ":data_reader",
        ":topic",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "topic",
    hdrs = ["topic.h"],
    deps = [
        ":types",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "data_writer",
    hdrs = ["data_writer.h"],
    deps = [
        ":topic",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "data_reader",
    hdrs = ["data_reader.h"],
    deps = [
        ":topic",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "config",
    hdrs = ["config.h"],
    deps = [
        ":transport_types",
        ":types",
        "@yaml-cpp//:yaml-cpp",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "auto_config",
    hdrs = ["auto_config.h"],
    deps = [
        ":config",
        ":domain_participant",
        ":publisher",
        ":subscriber",
        ":topic",
    ],
    visibility = ["//visibility:public"],
) 