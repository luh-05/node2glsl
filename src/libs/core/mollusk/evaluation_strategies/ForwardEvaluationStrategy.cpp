
#include "mir/codegen.hpp"
#include "mir/node_graph/GraphContext.hpp"
#include "mir/node_graph/node_graph.hpp"
#include "mollusk/evaluation/evaluator.hpp"
#include <absl/status/status.h>
#include <format>
#include <iterator>
#include <memory>
namespace msk {

auto ForwardEvaluationStrategy::evalGraph(ir::GraphContext &cxt,
                                          ir::Graph &graph) -> absl::Status {
  for (auto it = cxt.graph->GetSubnodesIt();
       it != cxt.graph->GetSubnodesItEnd(); it++) {
    auto value = &it->second;

    if (auto *g = std::get_if<std::unique_ptr<ir::Graph>>(value)->get()) {
      if (auto s = evalGraph(cxt, *g); !s.ok()) {
        return s;
      }
    } else if (auto *m =
                   std::get_if<std::unique_ptr<ir::Module>>(value)->get()) {
      if (auto s = evalModule(cxt, *m); !s.ok()) {
        return s;
      }
    } else {
      return absl::InternalError(
          "Could not determine variant type to be neither Graph nor Module!");
    }
  }

  return absl::OkStatus();
}

auto ForwardEvaluationStrategy::evalModule(ir::GraphContext &cxt,
                                           ir::Module &module) -> absl::Status {
  auto inserter = std::back_inserter(this->tokens);

  *inserter = ir::TextToken("{\n");

  if (auto s = module.impl({this->cxt_prov, inserter, module}); !s.ok()) {
    return absl::AbortedError(
        std::format("Failed to evaluate Module: {}", s.ToString()));
  }

  *inserter = ir::TextToken("}\n");

  return absl::OkStatus();
}

auto ForwardEvaluationStrategy::GenerateTokens(ir::GraphContext &cxt)
    -> absl::Status {
  if (auto s = this->evalGraph(cxt, *cxt.graph.get()); !s.ok()) {
    return s;
  }
  return absl::OkStatus();
}

auto ForwardEvaluationStrategy::EvaluateTokens(ir::GraphContext &cxt,
                                               std::string &out)
    -> absl::Status {

  for (auto it = tokens.begin(); it != tokens.end(); it++) {
    static int i = 0;
    auto token = &*it;
    std::string token_string;
    if (auto *t = std::get_if<msk::ir::TextToken>(token)) {
      token_string = t->GetString();
    } else if (auto *t = std::get_if<msk::ir::WildcardToken>(token)) {
      token_string = t->GetString();
    }
    out += token_string;
  }

  return absl::OkStatus();
}

} // namespace msk
