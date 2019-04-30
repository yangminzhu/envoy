licenses(["notice"])  # Apache 2

load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_test",
    "envoy_cc_test_library",
    "envoy_package",
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
