#include "mir/node_graph/GraphContext.hpp"
#include "mir/node_graph/node_graph.hpp"
#include "mollusk/evaluation/evaluator.hpp"
#include <blender/modules/modules.hpp>
#include <iterator>
#include <memory>
// #include <mir/mollusk_ir.hpp>
#include <absl/status/status_matchers.h>
#include <gtest/gtest.h>
#include <mir/codegen.hpp>
#include <spdlog/spdlog.h>
#include <variant>
#include <vector>

TEST(BLENDER_IR_TEST, DUMMY_IMPLEMENTATION) {
  EXPECT_NO_THROW(auto mod =
                      msk::ir::Module(msk::blender::GenerateTokenStringDummy););
}

TEST(BLENDER_IR_TEST, DUMMY_EVALUATION) {
  auto context = std::make_shared<msk::ir::GraphContext>();

  auto context_provider = std::make_shared<msk::ir::ContextProvider>(context);

  auto mod = msk::ir::Module(msk::blender::GenerateTokenStringDummy);
  auto a = context->AddConstant(&mod, "a", "4");
  ABSL_EXPECT_OK(a);

  auto tokens = std::vector<msk::ir::CodegenToken>();
  if (auto status =
          mod.impl({context_provider, std::back_inserter(tokens), mod});
      !status.ok()) {
    // spdlog::error(status.message());
    ABSL_EXPECT_OK(status);
  }
}

TEST(BLENDER_IR_TEST, DUMMY_FORWARD_EVAL) {
  auto context = std::make_shared<msk::ir::GraphContext>();

  msk::ir::Module *mod;
  if (auto s = context->graph->AddModule(
          "foo", msk::blender::GenerateTokenStringDummy);
      !s.ok()) {
    // spdlog::error(s.status().ToString());
    ABSL_EXPECT_OK(s);
  } else {
    mod = *s;
  }

  if (auto s = context->AddConstant(mod, "a", "4"); !s.ok()) {
    // spdlog::error(s.ToString());
    ABSL_EXPECT_OK(s);
  }

  msk::Evaluator eval(context,
                      std::make_unique<msk::ForwardEvaluationStrategy>());

  std::string c;
  if (auto s = eval.Evaluate(c); !s.ok()) {
    // spdlog::error(s.ToString());
    ABSL_EXPECT_OK(s);
  }

  // spdlog::warn(std::format("Output: \n{}\n", c));
}
