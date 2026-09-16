#include "ir/graph/graph.hpp"
#include <absl/status/status.h>
#include <cctype>
#include <charconv>
#include <mir/node_graph/node_graph.hpp>
#include <type_traits>

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

// template <class T> auto Module::Out::GetConstant(std::string_view name) -> T
// {
//   if (auto s = cxt->GetConstant<T>(static_cast<Node *>(&parent), name);
//       !s.ok()) {
//     if (this->status.ok())
//       this->status = s.status();
//     return {};
//   } else
//     return s.value();
//   // return (T)0;
// }
// template auto Module::Out::GetConstant<uint64_t>(std::string_view name)
//     -> uint64_t;
// template auto Module::Out::GetConstant<uint32_t>(std::string_view name)
//     -> uint32_t;
// template auto Module::Out::GetConstant<uint16_t>(std::string_view name)
//     -> uint16_t;
// template auto Module::Out::GetConstant<uint8_t>(std::string_view name)
//     -> uint8_t;
// template auto Module::Out::GetConstant<int64_t>(std::string_view name)
//     -> int64_t;
// template auto Module::Out::GetConstant<int32_t>(std::string_view name)
//     -> int32_t;
// template auto Module::Out::GetConstant<int16_t>(std::string_view name)
//     -> int16_t;
// template auto Module::Out::GetConstant<int8_t>(std::string_view name) ->
// int8_t; template auto Module::Out::GetConstant<float>(std::string_view name)
// -> float; template auto Module::Out::GetConstant<double>(std::string_view
// name) -> double; template auto
// Module::Out::GetConstant<std::string>(std::string_view name)
//     -> std::string;
// template auto Module::Out::GetConstant<const char *>(std::string_view name)
//     -> const char *;

} // namespace msk::ir
