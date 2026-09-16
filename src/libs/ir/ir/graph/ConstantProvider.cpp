#include "ir/graph/graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
// #include <mir/codegen.hpp>

namespace msk::ir {

template <typename T>
auto ContextProvider::GetConstant(Node *n, std::string_view name)
    -> absl::StatusOr<T> {
  return 0;
  // return this->context.get()->GetConstant<T>(n, name);
}
} // namespace msk::ir
