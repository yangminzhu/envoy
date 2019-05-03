#include "absl/strings/match.h"
#include "benchmark/benchmark.h"
#include "common/common/assert.h"
#include "common/protobuf/protobuf.h"
#include "google/protobuf/text_format.h"
#include "google/api/expr/v1alpha1/syntax.pb.h"
#include "test/cel/cel_ast.h"
#include "test/cel/cel_ast.pb.h"

#include "extensions/filters/common/rbac/engine_impl.h"
#include "test/mocks/network/mocks.h"
#include "test/mocks/ssl/mocks.h"
#include "gmock/gmock.h"
#include <unordered_set>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"

#include "eval/public/activation.h"
#include "eval/public/activation_bind_helper.h"
#include "eval/public/builtin_func_registrar.h"
#include "eval/public/cel_expr_builder_factory.h"
#include "eval/public/cel_expression.h"
#include "eval/public/cel_value.h"

#pragma GCC diagnostic pop

namespace Envoy {
namespace {

using google::protobuf::Arena;
using google::protobuf::TextFormat;
using google::api::expr::v1alpha1::Expr;
using google::api::expr::v1alpha1::SourceInfo;
using google::api::expr::runtime::CelExpressionBuilder;
using google::api::expr::runtime::Activation;
using google::api::expr::runtime::BindProtoToActivation;
using google::api::expr::runtime::CelValue;
using google::api::expr::runtime::CreateCelExpressionBuilder;
using google::api::expr::runtime::util::IsOk;
using google::api::expr::runtime::CelExpression;
using Envoy::Extensions::Filters::Common::RBAC::RoleBasedAccessControlEngineImpl;

static bool NativeCheck(const cel::AttributesContext& ac,
    const std::unordered_set<std::string>& blacklists,
    const std::unordered_set<std::string>& whitelists) {
  const auto& ip = ac.headers().at("ip");
  if (blacklists.find(ip) != blacklists.end()) {
    return false;
  }
  const auto& path = ac.headers().at("path");
  const auto& token = ac.headers().at("token");
  if (absl::StartsWith(path, "v1")) {
    if (token == "v1" || token == "v2" || token == "admin") {
      return true;
    }
  } else if (absl::StartsWith(path, "v2")) {
    if (token == "v2" || token == "admin") {
      return true;
    }
  } else if (absl::StartsWith(path, "/admin")) {
    if (token == "admin") {
      if (whitelists.find(ip) != whitelists.end()) {
        return true;
      }
    }
  }
  return false;
}
static void Native(benchmark::State& state) {
  cel::AttributesContext ac = AttributesContext();
  const auto blacklists = std::unordered_set<std::string>{"10.0.1.4", "10.0.1.5", "10.0.1.6"};
  const auto whitelists = std::unordered_set<std::string>{"10.0.1.1", "10.0.1.2", "10.0.1.3"};
  for (auto _ : state) {
    auto result = NativeCheck(ac, blacklists, whitelists);
    RELEASE_ASSERT(result, "");
  }
}
BENCHMARK(Native);

static void RBAC(benchmark::State& state) {
  envoy::config::rbac::v2alpha::RBAC filterConfig;
  TextFormat::ParseFromString(RBACFilterConfig(), &filterConfig);

  // Use headers to mock the AttributesContext()
  // Use ":authority" to mock ip.
  Envoy::Http::TestHeaderMapImpl headers{
      {":authority", "10.0.1.2"},
      {":path", "/admin/edit"},
      {"token", "admin"},
  };

  RoleBasedAccessControlEngineImpl engine(filterConfig);
  const auto& conn = Envoy::Network::MockConnection();
  auto metadata = envoy::api::v2::core::Metadata();
  for (auto _ : state) {
    auto result = engine.allowed(conn, headers, metadata, nullptr);
    RELEASE_ASSERT(result, "");
  }
}
BENCHMARK(RBAC);

static void CEL(benchmark::State& state) {
  Expr expr;
  SourceInfo source_info;
  TextFormat::ParseFromString(CELAst(), &expr);

  // Obtain CEL Expression builder.
  std::unique_ptr<CelExpressionBuilder> builder = CreateCelExpressionBuilder();

  // Builtin registration.
  RELEASE_ASSERT(IsOk(RegisterBuiltinFunctions(builder->GetRegistry())), "");

  // Create CelExpression from AST (Expr object).
  auto cel_expression_status = builder->CreateExpression(&expr, &source_info);

  RELEASE_ASSERT(IsOk(cel_expression_status), "");

  auto cel_expression = std::move(cel_expression_status.ValueOrDie());

  Activation activation;

  cel::AttributesContext ac = AttributesContext();
  Arena ac_arena;
  BindProtoToActivation(&ac, &ac_arena, &activation);

  Arena arena;
  for (auto _ : state) {
    // Run evaluation.
    auto eval_status = cel_expression->Evaluate(activation, &arena);
    CelValue result = eval_status.ValueOrDie();
    RELEASE_ASSERT(result.BoolOrDie(), "");
  }
}
BENCHMARK(CEL);

static void CEL_FlattenedMap(benchmark::State& state) {
  Expr expr;
  SourceInfo source_info;
  TextFormat::ParseFromString(CELAstFlattenedMap(), &expr);

  // Obtain CEL Expression builder.
  std::unique_ptr<CelExpressionBuilder> builder = CreateCelExpressionBuilder();

  // Builtin registration.
  RELEASE_ASSERT(IsOk(RegisterBuiltinFunctions(builder->GetRegistry())), "");

  // Create CelExpression from AST (Expr object).
  auto cel_expression_status = builder->CreateExpression(&expr, &source_info);

  RELEASE_ASSERT(IsOk(cel_expression_status), "");

  auto cel_expression = std::move(cel_expression_status.ValueOrDie());

  Activation activation;

  cel::AttributesContext ac = AttributesContext();
  activation.InsertValue("ip", CelValue::CreateString(&(ac.headers().at("ip"))));
  activation.InsertValue("path", CelValue::CreateString(&(ac.headers().at("path"))));
  activation.InsertValue("token", CelValue::CreateString(&(ac.headers().at("token"))));

  Arena arena;
  for (auto _ : state) {
    // Run evaluation.
    auto eval_status = cel_expression->Evaluate(activation, &arena);
    CelValue result = eval_status.ValueOrDie();
    RELEASE_ASSERT(result.BoolOrDie(), "");
  }
}
BENCHMARK(CEL_FlattenedMap);

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

