#include "mir/node_graph/GraphContext.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <string_view>
// #include <mir/codegen.hpp>

namespace msk::ir {

template <>
auto ContextProvider::GetConstant(Node *n, std::string_view name)
    -> absl::StatusOr<bool> {
  auto s = this->context.get()->GetConstant<std::string>(n, name);
  if (!s.ok())
    return s.status();

  if (*s == "True" || *s == "true" || *s == "1") {
    return true;
  }
  if (*s == "False" || *s == "false" || *s == "0") {
    return false;
  }

  return absl::InvalidArgumentError(std::format(
      "Constant '{}' with value '{}' cannot be interpreted as boolean!", name,
      *s));
}

template <class T>
auto ContextProvider::GetConstant(Node *n, std::string_view name)
    -> absl::StatusOr<T> {
  return this->context.get()->GetConstant<T>(n, name);
}
template auto ContextProvider::GetConstant<uint32_t>(Node *n,
                                                     std::string_view name)
    -> absl::StatusOr<uint32_t>;
template auto ContextProvider::GetConstant<int32_t>(Node *n,
                                                    std::string_view name)
    -> absl::StatusOr<int32_t>;
template auto ContextProvider::GetConstant<uint64_t>(Node *n,
                                                     std::string_view name)
    -> absl::StatusOr<uint64_t>;
template auto ContextProvider::GetConstant<int64_t>(Node *n,
                                                    std::string_view name)
    -> absl::StatusOr<int64_t>;
template auto ContextProvider::GetConstant<uint16_t>(Node *n,
                                                     std::string_view name)
    -> absl::StatusOr<uint16_t>;
template auto ContextProvider::GetConstant<int16_t>(Node *n,
                                                    std::string_view name)
    -> absl::StatusOr<int16_t>;
template auto ContextProvider::GetConstant<uint8_t>(Node *n,
                                                    std::string_view name)
    -> absl::StatusOr<uint8_t>;
template auto ContextProvider::GetConstant<int8_t>(Node *n,
                                                   std::string_view name)
    -> absl::StatusOr<int8_t>;
template auto ContextProvider::GetConstant<float>(Node *n,
                                                  std::string_view name)
    -> absl::StatusOr<float>;
template auto ContextProvider::GetConstant<double>(Node *n,
                                                   std::string_view name)
    -> absl::StatusOr<double>;
template auto ContextProvider::GetConstant<std::string>(Node *n,
                                                        std::string_view name)
    -> absl::StatusOr<std::string>;
template auto ContextProvider::GetConstant<const char *>(Node *n,
                                                         std::string_view name)
    -> absl::StatusOr<const char *>;
// template auto ContextProvider::GetConstant<bool>(Node *n, std::string_view
// name)
//     -> absl::StatusOr<bool>;

auto ContextProvider::LogPort(Node *n, Port *port, bool right) -> void {
  this->context->LogPort(n, port, right);
}
} // namespace msk::ir
