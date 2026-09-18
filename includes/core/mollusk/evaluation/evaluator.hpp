#pragma once

#include "mir/node_graph/GraphContext.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <memory>
#include <variant>
#include <vector>
namespace msk {

using ContextPointer = std::shared_ptr<ir::GraphContext>;
using WeakContextPointer = std::weak_ptr<ir::GraphContext>;
using NodeVariant = std::variant<ir::Graph *, ir::Module *>;

class EvaluationStrategy {
public:
  using TokenVector = std::vector<msk::ir::Module::Token>;
  TokenVector tokens;

  virtual auto GenerateTokens(ContextPointer cxt) -> absl::Status {
    return absl::InternalError("Tried using base EvaluationStrategy!");
  }
  virtual auto EvaluateTokens(ContextPointer cxt, std::string &out)
      -> absl::Status {
    return absl::InternalError("Tried using base EvaluationStrategy!");
  }
};

class ForwardEvaluationStrategy : public EvaluationStrategy {
private:
  std::shared_ptr<ir::ContextProvider> cxt_prov;

  auto evalGraph(ContextPointer cxt, ir::Graph &graph) -> absl::Status;
  auto evalModule(ContextPointer cxt, ir::Module &module) -> absl::Status;

public:
  auto GenerateTokens(ContextPointer cxt) -> absl::Status override;
  auto EvaluateTokens(ContextPointer cxt, std::string &out)
      -> absl::Status override;
};

class Evaluator {
private:
  ContextPointer context;

  std::unique_ptr<EvaluationStrategy> strat;

public:
  Evaluator(ContextPointer context, std::unique_ptr<EvaluationStrategy> &&strat)
      : context(context), strat(std::move(strat)) {}

  inline auto Evaluate(std::string &out) -> absl::Status {
    if (auto s = this->strat->GenerateTokens(this->context); !s.ok()) {
      return absl::AbortedError(
          std::format("Failed to generate CodegenTokens: {}", s.ToString()));
    }
    out = "";
    if (auto s = this->strat->EvaluateTokens(this->context, out); !s.ok()) {
      return absl::AbortedError(
          std::format("Failed to evaluate CodegenTokens: {}", s.ToString()));
    }

    return absl::OkStatus();
  }
};

} // namespace msk
