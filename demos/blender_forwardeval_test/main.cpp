#include "blender/root.hpp"
#include "mir/node_graph/GraphContext.hpp"
#include "mir/node_graph/node_graph.hpp"
#include "mollusk/evaluation/evaluator.hpp"
#include <blender/modules/modules.hpp>
#include <memory>
#include <mir/codegen.hpp>
#include <spdlog/spdlog.h>

int main() {
  auto context = std::make_shared<msk::ir::GraphContext>();

  msk::ir::Module *mod;
  if (auto s = context->graph->AddModule(
          "foo",
          msk::blender::FuncWrapper<msk::blender::GenerateTokenStringDummy>);
      !s.ok()) {
    spdlog::error(s.status().ToString());
    return 1;
  } else {
    mod = *s;
  }

  if (auto s = context->AddConstant(mod, "a", "4"); !s.ok()) {
    spdlog::error(s.ToString());
    return 1;
  }

  msk::Evaluator eval(context,
                      std::make_unique<msk::ForwardEvaluationStrategy>());

  std::string c;
  if (auto s = eval.Evaluate(c); !s.ok()) {
    spdlog::error(s.ToString());
    return 1;
  }

  spdlog::warn(std::format("Output: \n{}\n", c));

  return 0;
}
