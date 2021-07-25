package(default_visibility = ["//visibility:private"])

TEST_COPTS = [
    "-std=c++17",
]

cc_test(
    name = "jsonlib_test",
    size = "small",
    testonly = 1,
    srcs = [
        "test_json.cc",
        "test_parser.cc",
        "test_tokenizer.cc",
    ],
    copts = TEST_COPTS,
    includes = ["include/"],
    deps = [
        "//:libjson",
        "@testrunner//:lib",
        "@testrunner//:main",
    ],
)
