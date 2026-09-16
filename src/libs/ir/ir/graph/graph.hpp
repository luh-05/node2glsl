
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <flat_map>
#include <mir/node_graph/node_graph.hpp>
#pragma once

namespace msk::ir {
class ConstantStore {
private:
  std::map<Node *, std::flat_map<std::string, std::string, std::less<>>> map;

public:
  auto AddConstant(Node *node, std::string_view name, std::string value)
      -> absl::Status;

  template <class T>
  auto GetConstant(Node *node, std::string_view name) -> absl::StatusOr<T>;
};

class GraphContext {
public:
  ConstantStore const_store;

public:
  template <class T>
  auto GetConstant(Node *n, std::string_view &&name) -> absl::StatusOr<T>;

  inline auto AddConstant(Node *n, std::string_view name, std::string value)
      -> absl::Status {
    return this->const_store.AddConstant(n, name, value);
  }
};
} // namespace msk::ir
