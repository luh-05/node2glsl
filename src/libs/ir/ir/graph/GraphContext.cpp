#include <absl/status/status.h>
#include <mir/node_graph/GraphContext.hpp>
#include <mir/node_graph/node_graph.hpp>

namespace msk::ir {
auto ConstantStore::AddConstant(Node *node, std::string_view name,
                                std::string value) -> absl::Status {
  if (this->map.contains(node)) {
    if (this->map[node].contains(name)) {
      return absl::AlreadyExistsError(std::format(
          "Tried adding constant '{0}' with value '{1}' to node, but "
          "constant '{0}' already exists",
          name, value));
    }
  } else {
    if (auto r = this->map.try_emplace(
            node, std::flat_map<std::string, std::string, std::less<>>());
        !r.second) {
      return absl::InternalError(std::format(
          "std::map::try_emplace failed to add new node-scope lookup"));
    }
  }

  if (auto r = this->map.at(node).try_emplace(std::string(name), value);
      !r.second) {
    return absl::DataLossError(std::format(
        "Failed to add constant '{0}' with value '{1}'", name, value));
  }

  return absl::OkStatus();
}

ContextProvider::ContextProvider(std::shared_ptr<GraphContext> context) {
  this->context = context;
}
} // namespace msk::ir
