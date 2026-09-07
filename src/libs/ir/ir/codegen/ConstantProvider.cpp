#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <format>
#include <mir/codegen.hpp>
#include <mir/node_graph/node_graph.hpp>
#include <string>

namespace msk::ir {

template <>
auto ContextProvider::GetConstant<const char *>(Node *n, std::string_view name)
    -> absl::StatusOr<const char *> {
  return "Hello World!";
}

template <>
auto ContextProvider::GetConstant<const bool>(Node *n, std::string_view name)
    -> absl::StatusOr<const bool> {
  return true;
}

template <>
auto ContextProvider::GetConstant<const uint32_t>(Node *n,
                                                  std::string_view name)
    -> absl::StatusOr<const uint32_t> {
  if (name == "foo")
    return absl::NotFoundError(std::format("Constant '{}' not found!", name));
  return 4;
}

} // namespace msk::ir
