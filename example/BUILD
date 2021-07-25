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

cc_binary(
    name = "hello_json",
    srcs = ["hello_json.cc"],
    copts = COPTS,
    deps = ["//:libjson"],
)
