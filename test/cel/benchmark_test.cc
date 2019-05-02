#include "benchmark/benchmark.h"
#include "common/protobuf/protobuf.h"
#include "google/protobuf/text_format.h"
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

#include "common/common/assert.h"

namespace Envoy {
namespace {

using google::protobuf::Arena;
using google::protobuf::TextFormat;
using google::api::expr::v1alpha1::Expr;
using google::api::expr::v1alpha1::SourceInfo;
using google::api::expr::runtime::CelExpressionBuilder;
using google::api::expr::runtime::Activation;
using google::api::expr::runtime::CelValue;
using google::api::expr::runtime::CreateCelExpressionBuilder;
using google::api::expr::runtime::util::IsOk;
using google::api::expr::runtime::CelExpression;

static void CEL(benchmark::State& state) {
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

  TextFormat::ParseFromString(kExpr0, &expr);

  // Obtain CEL Expression builder.
  std::unique_ptr<CelExpressionBuilder> builder = CreateCelExpressionBuilder();

  // Builtin registration.
  RELEASE_ASSERT(IsOk(RegisterBuiltinFunctions(builder->GetRegistry())), "");

  // Create CelExpression from AST (Expr object).
  auto cel_expression_status = builder->CreateExpression(&expr, &source_info);

  RELEASE_ASSERT(IsOk(cel_expression_status), "");

  auto cel_expression = std::move(cel_expression_status.ValueOrDie());

  Activation activation;

  // Bind value to "var" parameter.
  activation.InsertValue("var", CelValue::CreateInt64(1));

  Arena arena;

  for (auto _ : state) {
    // Run evaluation.
    auto eval_status = cel_expression->Evaluate(activation, &arena);

    RELEASE_ASSERT(IsOk(eval_status), "");

    CelValue result = eval_status.ValueOrDie();
    RELEASE_ASSERT(result.IsInt64(), "");
    RELEASE_ASSERT(result.Int64OrDie() == 2, "");
  }
}
BENCHMARK(CEL);

}  // namespace
}  // namespace Envoy

// Boilerplate main(), which discovers benchmarks in the same file and runs them.
int main(int argc, char** argv) {
  benchmark::Initialize(&argc, argv);

  if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
    return 1;
  }
  benchmark::RunSpecifiedBenchmarks();
}

