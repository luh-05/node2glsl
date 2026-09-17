#pragma once

#include "ir/graph/graph.hpp"
#include "mir/codegen.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <memory>
#include <vector>
namespace msk {

class Evaluator {
private:
  using GraphPointer = std::shared_ptr<ir::Graph>;
  using ContextPointer = std::shared_ptr<ir::GraphContext>;
  GraphPointer graph;
  ContextPointer context;

  using TokenVector = std::vector<ir::CodegenToken>;
  TokenVector tokens;

  // auto GenerateTokens()
public:
  Evaluator(GraphPointer graph, ContextPointer context)
      : graph(graph), context(context) {}

  auto Evaluate(std::string &out) -> absl::Status;
};

} // namespace msk
