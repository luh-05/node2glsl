#include <absl/status/statusor.h>
#include <mir/codegen.hpp>
#include <mir/node_graph/node_graph.hpp>
#include <string>

namespace msk::ir {

template <>
auto ContextProvider::GetConstant<const char *>(Node *n, std::string name)
    -> absl::StatusOr<const char *> {
  return "Hello World!";
}

template <>
auto ContextProvider::GetConstant<const bool>(Node *n, std::string name)
    -> absl::StatusOr<const bool> {
  return true;
}

template <>
auto ContextProvider::GetConstant<const uint32_t>(Node *n, std::string name)
    -> absl::StatusOr<const uint32_t> {
  return 4;
}

} // namespace msk::ir
