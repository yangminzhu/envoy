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

#include "common/common/logger.h"
#include "spdlog/spdlog.h"
#include "extensions/filters/http/lua/lua_filter.h"
#include "test/mocks/thread_local/mocks.h"
#include "test/mocks/upstream/mocks.h"

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
using google::api::expr::runtime::CelExpression;
using Envoy::Extensions::Filters::Common::RBAC::RoleBasedAccessControlEngineImpl;

// Use headers to mock the AttributesContext()
// Use ":authority" to mock ip.
static Envoy::Http::TestHeaderMapImpl request_headers{
      {":authority", "10.0.1.2"},
      {":path", "/admin/edit"},
      {"token", "admin"},
};

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
  envoy::config::rbac::v2::RBAC filterConfig;
  TextFormat::ParseFromString(RBACFilterConfig(), &filterConfig);

  RoleBasedAccessControlEngineImpl engine(filterConfig);
  const auto& conn = Envoy::Network::MockConnection();
  auto metadata = envoy::api::v2::core::Metadata();
  for (auto _ : state) {
    auto result = engine.allowed(conn, request_headers, metadata, nullptr);
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
  RELEASE_ASSERT(RegisterBuiltinFunctions(builder->GetRegistry()).ok(), "");

  // Create CelExpression from AST (Expr object).
  auto cel_expression_status = builder->CreateExpression(&expr, &source_info);

  RELEASE_ASSERT(cel_expression_status.ok(), "");

  auto cel_expression = std::move(cel_expression_status.ValueOrDie());

  Activation activation;

  cel::AttributesContext ac = AttributesContext();
  Arena ac_arena;
  RELEASE_ASSERT(BindProtoToActivation(&ac, &ac_arena, &activation).ok(), "");

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
  RELEASE_ASSERT(RegisterBuiltinFunctions(builder->GetRegistry()).ok(), "");

  // Create CelExpression from AST (Expr object).
  auto cel_expression_status = builder->CreateExpression(&expr, &source_info);

  RELEASE_ASSERT(cel_expression_status.ok(), "");

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

static void LuaJIT(benchmark::State& state) {
  /*
   * !(headers.ip in ["10.0.1.4", "10.0.1.5", "10.0.1.6"]) &&
   * ((headers.path.startsWith("v1") && headers.token in ["v1", "v2", "admin"]) ||
   *  (headers.path.startsWith("v2") && headers.token in ["v2", "admin"]) ||
   *  (headers.path.startsWith("/admin") && headers.token == "admin" && headers.ip in ["10.0.1.1", "10.0.1.2", "10.0.1.3"]))
   */
  const std::string luaCode{R"EOF(
    function envoy_on_request(request_handle)
      local ip = request_handle:headers():get(":authority")
      local ip_blacklist = {
        ["10.0.1.4"]=true,
        ["10.0.1.5"]=true,
        ["10.0.1.6"]=true
      }
      if ip_blacklist[ip] == true then
        print("should never happen: returned when checking ip in blacklist")
        return
      end

      local path = request_handle:headers():get(":path")
      local token = request_handle:headers():get("token")
      local allowed_v1_tokens = {["v1"]=true, ["v2"]=true, ["admin"]=true}
      local allowed_v2_tokens = {["v2"]=true, ["admin"]=true}
      local admin_ip_whitelist = {
        ["10.0.1.1"]=true,
        ["10.0.1.2"]=true,
        ["10.0.1.3"]=true
      }

      if path:find("v1") and allowed_v1_tokens[token] == true then
        print("should never happen: returned when checking v1 path")
        return
      elseif path:find("v2") and allowed_v2_tokens[token] == true then
        print("should never happen: returned when checking v2 path")
        return
      elseif path:find("/admin") and token == "admin" and admin_ip_whitelist[ip] == true then
        return
      end

      print("should never happen: not returned from the /admin path checking")
    end
  )EOF"};

  // The mock should never be called during the benchmark.
  NiceMock<ThreadLocal::MockInstance> tls;
  Upstream::MockClusterManager cluster_manager;

  Logger::Registry::setLogLevel(spdlog::level::off);
  std::shared_ptr<Extensions::HttpFilters::Lua::FilterConfig> config(
      new Extensions::HttpFilters::Lua::FilterConfig(luaCode, tls, cluster_manager));
  Extensions::HttpFilters::Lua::Filter filter(config);

  auto& callbacks = filter.getDecoderCallbacks();
  Extensions::Filters::Common::Lua::CoroutinePtr coroutine = config->createCoroutine();
  Extensions::Filters::Common::Lua::LuaDeathRef<Extensions::HttpFilters::Lua::StreamHandleWrapper> handle;
  for (auto _ : state) {
    handle.reset(
        Extensions::HttpFilters::Lua::StreamHandleWrapper::create(
            coroutine->luaState(), *coroutine, request_headers, true, filter, callbacks),
        true);
    // In order to avoid mocking the StreamDecoderFilterCallbacks, we don't return
    // 403 in the lua code, in other words it always return 200.
    // We use print() function in the lua code to make sure all the lua code is
    // executed and it's returned from the last "elseif" statement.
    // If any of the "should never happen" is printed during the benchmark, it means
    // something wrong in the lua code or request header and the benchmark result
    // is not comparable to other cases.
    auto result = handle.get()->start(config->requestFunctionRef());
    handle.markDead();
    RELEASE_ASSERT(result == Http::FilterHeadersStatus::Continue, "");
  }
}
BENCHMARK(LuaJIT);

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

