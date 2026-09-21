#include <format>
#include <mir/codegen.hpp>
#include <mir/node_graph/node_graph.hpp>

namespace msk::ir {
std::string WildcardToken::GetString() {
  auto right_port = this->port;
  if (this->port->IsLeft()) {
    right_port = std::get<Port::ConnectionPointer>(this->port->connection)
                     .get()
                     ->right_port;
  }

  return std::format("val_{}", reinterpret_cast<void *>(right_port));
}
} // namespace msk::ir
