
#include "mir/codegen.hpp"
#include "mir/node_graph/GraphContext.hpp"
#include "mir/node_graph/node_graph.hpp"
#include "mollusk/evaluation/evaluator.hpp"
#include <absl/status/status.h>
#include <format>
#include <iterator>
#include <memory>
namespace msk {

auto ForwardEvaluationStrategy::evalGraph(ContextPointer cxt, ir::Graph &graph)
    -> absl::Status {
  // for (auto it = cxt->graph->GetSubnodesIt();
  //      it != cxt->graph->GetSubnodesItEnd(); it++) {
  //   auto value = &it->second;
  //
  //   if (auto &g = std::get_if<ir::Graph>(value)) {
  //     if (g) {
  //       if (auto s = evalGraph(cxt, *g); !s.ok()) {
  //         return s;
  //       }
  //     } else {
  //       return absl::NotFoundError(
  //           std::format("The graph '{}', is nullptr!", it->first));
  //     }
  //   } else if (auto &m = std::get_if<ir::Module>(value)) {
  //     if (m) {
  //       if (auto s = evalModule(cxt, m); !s.ok()) {
  //         return s;
  //       }
  //     } else {
  //       return absl::NotFoundError(
  //           std::format("The node '{}', is nullptr!", it->first));
  //     }
  //   } else {
  //     return absl::InternalError(
  //         "Could not determine variant type to be neither Graph nor
  //         Module!");
  //   }
  // }
  auto &subnodes = *graph.GetSubnodes();
  for (auto &[key, value] : subnodes) {

    if (auto *g = std::get_if<ir::Graph>(&value)) {
      if (g) {
        if (auto s = evalGraph(cxt, *g); !s.ok()) {
          return s;
        }
      } else {
        return absl::NotFoundError(
            std::format("The graph '{}', is nullptr!", key));
      }
    } else if (auto *m = std::get_if<ir::Module>(&value)) {
      if (m) {
        if (auto s = evalModule(cxt, *m); !s.ok()) {
          return s;
        }
      } else {
        return absl::NotFoundError(
            std::format("The node '{}', is nullptr!", key));
      }
    } else {
      return absl::InternalError(
          "Could not determine variant type to be neither Graph nor Module!");
    }
  }

  return absl::OkStatus();
}

auto ForwardEvaluationStrategy::evalModule(ContextPointer cxt,
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

auto ForwardEvaluationStrategy::GenerateTokens(ContextPointer cxt)
    -> absl::Status {
  this->cxt_prov = std::make_shared<ir::ContextProvider>(cxt);
  if (auto s = this->evalGraph(cxt, *cxt->graph.get()); !s.ok()) {
    return s;
  }
  return absl::OkStatus();
}

auto ForwardEvaluationStrategy::EvaluateTokens(ContextPointer cxt,
                                               std::string &out)
    -> absl::Status {
  for (auto it = tokens.begin(); it != tokens.end(); it++) {
    // static int i = 0;
    auto token = &*it;
    std::string token_string;
    if (auto *t = std::get_if<msk::ir::TextToken>(token)) {
      token_string = t->GetString();
    } else if (auto *t = std::get_if<msk::ir::WildcardToken>(token)) {
      token_string = t->GetString();
    } else {
      return absl::InvalidArgumentError(
          std::format("Invalid Token Type in Token Evaluation: '{}'",
                      typeid(token).name()));
    }
    out += token_string;
  }

  return absl::OkStatus();
}

} // namespace msk
