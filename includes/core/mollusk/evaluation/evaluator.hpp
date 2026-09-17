#pragma once

#include "mir/node_graph/GraphContext.hpp"
#include <memory>
#include <vector>
namespace msk {

class EvaluationStrategy {
private:
  using TokenVector = std::vector<msk::ir::Module::Token>;
  TokenVector tokens;

public:
  EvaluationStrategy() { tokens = TokenVector(); };
};

class Evaluator {
private:
  using ContextPointer = std::shared_ptr<ir::GraphContext>;
  ContextPointer context;

  EvaluationStrategy strat;

public:
  Evaluator(ContextPointer context, EvaluationStrategy strat)
      : context(context), strat(strat) {}

  auto Evaluate(std::string &out) -> absl::Status;
};

} // namespace msk
