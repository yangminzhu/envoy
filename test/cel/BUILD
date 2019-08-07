licenses(["notice"])  # Apache 2

load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_test_binary",
    "envoy_cc_test",
    "envoy_cc_test_library",
    "envoy_package",
    "envoy_proto_library",
)

envoy_package()

envoy_proto_library(
    name = "cel_ast_proto",
    srcs = [":cel_ast.proto"],
    external_deps = [
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_cc_proto",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_proto",
        "@com_google_googleapis//google/api/expr/v1alpha1:syntax_py_proto",
    ],
)

envoy_cc_test(
    name = "cel_test",
    srcs = ["cel_test.cc"],
    external_deps = [
        "abseil_strings",
        "abseil_time",
    ],
    deps = [
        ":cel_ast_proto_cc",
        "//source/common/protobuf",
        "@com_google_cel_cpp//eval/public:builtin_func_registrar",
        "@com_google_cel_cpp//eval/public:cel_expr_builder_factory",
        "@com_google_cel_cpp//eval/public:cel_expression",
        "@com_google_cel_cpp//eval/public:cel_value",
    ]
)

envoy_cc_test_library(
    name = "cel_ast",
    hdrs = ["cel_ast.h"],
    srcs = ["cel_ast.cc"],
    deps = [
        ":cel_ast_proto_cc",
    ]
)

envoy_cc_test_binary(
    name = "benchmark_test",
    srcs = [
        "benchmark_test.cc",
    ],
    external_deps = [
        "abseil_strings",
        "benchmark",
    ],
    deps = [
        "//source/extensions/filters/http/lua:lua_filter_lib",
        "//test/mocks/thread_local:thread_local_mocks",
        "//test/mocks/upstream:upstream_mocks",
        ":cel_ast",
        ":cel_ast_proto_cc",
        "//source/common/protobuf",
        "//source/common/common:assert_lib",
        "//source/extensions/filters/common/rbac:engine_lib",
        "//test/mocks/network:network_mocks",
        "//test/mocks/ssl:ssl_mocks",
        "//test/test_common:utility_lib",
        "@com_google_cel_cpp//eval/public:activation_bind_helper",
        "@com_google_cel_cpp//eval/public:builtin_func_registrar",
        "@com_google_cel_cpp//eval/public:cel_expr_builder_factory",
        "@com_google_cel_cpp//eval/public:cel_expression",
        "@com_google_cel_cpp//eval/public:cel_value",
    ],
)
