#include <absl/status/statusor.h>
#include <format>
#include <mir/codegen.hpp>
#include <mir/node_graph/node_graph.hpp>

namespace msk::ir {
auto WildcardToken::GetString() -> absl::StatusOr<std::string> {
  return this->port->GetName();
}
} // namespace msk::ir
