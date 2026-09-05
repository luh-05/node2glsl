#include <mir/codegen.hpp>
#include <mir/node_graph/node_graph.hpp>
#include <string>

namespace msk::ir {

template <>
auto ContextProvider::GetConstant<const char *>(Node *n, std::string name)
    -> absl::StatusOr<const char *> {
  return "";
}

} // namespace msk::ir
