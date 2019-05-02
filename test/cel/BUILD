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

envoy_cc_test(
    name = "cel_test",
    srcs = ["cel_test.cc"],
    external_deps = [
        "abseil_strings",
        "abseil_time",
        "cc_expr_v1beta1",
        "cc_expr_v1alpha1",
        "cel_eval_activation",
        "cel_eval_builtin_func_registrar",
        "cel_eval_cel_expr_builder_factory",
        "cel_eval_cel_expression",
        "cel_eval_cel_value",
    ],
    deps = [
        "//source/common/protobuf",
    ]
)

envoy_proto_library(
    name = "cel_ast_proto",
    srcs = [":cel_ast.proto"],
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
        "cc_expr_v1beta1",
        "cc_expr_v1alpha1",
        "cel_eval_activation",
        "cel_eval_activation_bind_helper",
        "cel_eval_builtin_func_registrar",
        "cel_eval_cel_expr_builder_factory",
        "cel_eval_cel_expression",
        "cel_eval_cel_value",
    ],
    deps = [
        ":cel_ast",
        ":cel_ast_proto_cc",
        "//source/common/protobuf",
        "//source/common/common:assert_lib",
        "//source/extensions/filters/common/rbac:engine_lib",
        "//test/mocks/network:network_mocks",
        "//test/mocks/ssl:ssl_mocks",
        "//test/test_common:utility_lib",
    ],
)
