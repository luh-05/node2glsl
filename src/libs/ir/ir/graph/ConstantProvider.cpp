#include "ir/graph/graph.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
// #include <mir/codegen.hpp>

namespace msk::ir {

template <typename T>
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
} // namespace msk::ir
