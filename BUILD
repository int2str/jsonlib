WARNING_COPTS = [
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Wconversion",
    "-Wunused",
    "-Wimplicit-fallthrough",
    "-Wshadow",
    "-Wold-style-cast",
    "-Wcast-align",
    "-Wnon-virtual-dtor",
]

COPTS = WARNING_COPTS + [
    "-std=c++17",
]

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "public_headers",
    srcs = [
        "include/jsonlib/json.h",
        "include/jsonlib/node.h",
    ],
)

cc_library(
    name = "libjson",
    srcs = [
        "consumable.h",
        "json.cc",
        "node.cc",
        "parser.cc",
        "parser.h",
        "token.cc",
        "token.h",
        "tokenizer.cc",
        "tokenizer.h",
        "validatable.h",
    ],
    hdrs = ["//:public_headers"],
    copts = COPTS,
    strip_include_prefix = "/include",
)
