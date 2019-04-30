#include "common/protobuf/protobuf.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "google/api/expr/v1alpha1/syntax.pb.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"

#include "eval/public/activation.h"
#include "eval/public/builtin_func_registrar.h"
#include "eval/public/cel_expr_builder_factory.h"
#include "eval/public/cel_expression.h"
#include "eval/public/cel_value.h"

#pragma GCC diagnostic pop

namespace Envoy {

using ::google::protobuf::Arena;
using google::api::expr::v1alpha1::Expr;
using google::api::expr::v1alpha1::SourceInfo;
using google::api::expr::runtime::CelExpressionBuilder;
using google::api::expr::runtime::Activation;
using google::api::expr::runtime::CelValue;
using google::api::expr::runtime::CreateCelExpressionBuilder;
using google::api::expr::runtime::util::IsOk;

// DEMO integrating CEL engine with Envoy
// To run this test, simply run: bazel test  //test/cel:cel_test

// Simple end-to-end test, which also serves as usage example.
TEST(EndToEndTest, Simple) {
  // AST CEL equivalent of "1+var"
  constexpr char kExpr0[] = R"(
    call_expr: <
      function: "_+_"
      args: <
        ident_expr: <
          name: "var"
        >
      >
      args: <
        const_expr: <
          int64_value: 1
        >
      >
    >
)";

  Expr expr;
  SourceInfo source_info;
  Protobuf::TextFormat::ParseFromString(kExpr0, &expr);

  // Obtain CEL Expression builder.
  std::unique_ptr<CelExpressionBuilder> builder = CreateCelExpressionBuilder();

  // Builtin registration.
  ASSERT_TRUE(IsOk(RegisterBuiltinFunctions(builder->GetRegistry())));

  // Create CelExpression from AST (Expr object).
  auto cel_expression_status = builder->CreateExpression(&expr, &source_info);

  ASSERT_TRUE(IsOk(cel_expression_status));

  auto cel_expression = std::move(cel_expression_status.ValueOrDie());

  Activation activation;

  // Bind value to "var" parameter.
  activation.InsertValue("var", CelValue::CreateInt64(1));

  Arena arena;

  // Run evaluation.
  auto eval_status = cel_expression->Evaluate(activation, &arena);

  ASSERT_TRUE(IsOk(eval_status));

  CelValue result = eval_status.ValueOrDie();

  ASSERT_TRUE(result.IsInt64());
  EXPECT_EQ(result.Int64OrDie(), 2);
}

}
