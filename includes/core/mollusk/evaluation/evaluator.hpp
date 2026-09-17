#pragma once

#include "mir/node_graph/GraphContext.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <memory>
#include <variant>
#include <vector>
namespace msk {

using NodeVariant = std::variant<ir::Graph *, ir::Module *>;

class EvaluationStrategy {
public:
  using TokenVector = std::vector<msk::ir::Module::Token>;
  TokenVector tokens;

  virtual auto GenerateTokens(ir::GraphContext &cxt) -> absl::Status {
    return absl::InternalError("Tried using base EvaluationStrategy!");
  }
  virtual auto EvaluateTokens(ir::GraphContext &cxt, std::string &out)
      -> absl::Status {
    return absl::InternalError("Tried using base EvaluationStrategy!");
  }
};

class ForwardEvaluationStrategy : public EvaluationStrategy {
private:
  std::shared_ptr<ir::ContextProvider> cxt_prov;

  auto evalGraph(ir::GraphContext &cxt, ir::Graph &graph) -> absl::Status;
  auto evalModule(ir::GraphContext &cxt, ir::Module &module) -> absl::Status;

public:
  auto GenerateTokens(ir::GraphContext &cxt) -> absl::Status;
  auto EvaluateTokens(ir::GraphContext &cxt, std::string &out) -> absl::Status;
};

class Evaluator {
private:
  using ContextPointer = std::shared_ptr<ir::GraphContext>;
  ContextPointer context;

  EvaluationStrategy strat;

public:
  Evaluator(ContextPointer context, EvaluationStrategy strat)
      : context(context), strat(strat) {}

  inline auto Evaluate(std::string &out) -> absl::Status {
    if (auto s = this->strat.GenerateTokens(*this->context.get()); !s.ok()) {
      return absl::AbortedError(
          std::format("Failed to generate CodegenTokens: {}", s.ToString()));
    }
    if (auto s = this->strat.EvaluateTokens(*this->context.get(), out);
        !s.ok()) {
      return absl::AbortedError(
          std::format("Failed to evaluate CodegenTokens: {}", s.ToString()));
    }

    return absl::OkStatus();
  }
};

} // namespace msk
