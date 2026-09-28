
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <exception>
#include <flat_map>
#include <map>
#include <memory>
#include <optional>
#include <tuple>
#include <vector>
#pragma once

namespace msk::ir {
/*
 * @brief Store for constants. Internally stores std::strings, but converts to
 * templated type on retrieval
 */
class ConstantStore {
private:
  std::map<Node *, std::flat_map<std::string, std::string, std::less<>>> map;
  template <class T>
  auto convertString(std::string_view s) -> absl::StatusOr<T> {
    if constexpr (std::is_floating_point_v<T>) {
      if (!s.empty() && (s.back() == 'f' || s.back() == 'F'))
        s.remove_suffix(1);
    }

    T value{};
    auto [ptr, err] = std::from_chars(s.data(), s.data() + s.size(), value);

    if (err != std::errc{} || ptr != s.data() + s.size()) {
      return absl::InvalidArgumentError(std::format(
          "Could not convert '{0}' to type '{1}'", s, typeid(T).name()));
    }

    return value;
  }

public:
  auto AddConstant(Node *node, std::string_view name, std::string value)
      -> absl::Status;

  template <class T>
  auto GetConstant(Node *node, std::string_view name) -> absl::StatusOr<T> {
    {
      if (!this->map.contains(node)) {
        return absl::NotFoundError(
            std::format("Node not registered in constant store!"));
      }

      if (!this->map.at(node).contains(name)) {
        return absl::NotFoundError(
            std::format("Constant '{}' not found!", name));
      }

      auto value_string = this->map.at(node).at(name);

      if constexpr (std::is_assignable_v<T, std::string> ||
                    std::is_assignable_v<T, std::string_view> ||
                    std::is_assignable_v<T, const char *>) {
        return value_string;
      }
      // if constexpr (std::is_same_v<T, bool>) {
      //   auto value_lower = std::string(value_string);
      //   std::transform(value_lower.begin(), value_lower.end(),
      //                  value_lower.begin(),
      //                  [](unsigned char c) { return std::tolower(c); });
      //   if (value_lower == "true" || value_lower == "1") {
      //     return true;
      //   } else if (value_lower == "false" || value_lower == "0") {
      //     return false;
      //   }
      //   return absl::InvalidArgumentError(std::format(
      //       "Could not convert constant '{}' with value '{}' to boolean!",
      //       name, value_string));
      // }
      if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
        return convertString<T>(value_string);
      }

      return absl::InvalidArgumentError(
          std::format("Type '{}' not supported!", typeid(T).name()));
    }
  }
};

/*
 * @brief Holds all context associated with a graph including ownership of the
 * Graph itself
 */
class GraphContext {
public:
  std::unique_ptr<ir::Graph> graph;
  ConstantStore const_store;

public:
  using PortSet = std::unordered_set<ir::Port *>;
  using PortAccessLog = std::pair<PortSet, PortSet>;
  std::map<Node *, PortAccessLog> access_info;

  // FIXME: Temporary
  std::vector<std::string> definitions;
  auto SetDefinitios(std::vector<std::string> defs) -> void {
    this->definitions = defs;
  }
  auto GetDefinitions() -> std::vector<std::string> & {
    return this->definitions;
  }

  GraphContext() { this->graph = std::make_unique<ir::Graph>(); }

  template <class T>
  auto GetConstant(Node *n, std::string_view name) -> absl::StatusOr<T> {
    return this->const_store.GetConstant<T>(n, name);
  }

  inline auto AddConstant(Node *n, std::string_view name, std::string value)
      -> absl::Status {
    return this->const_store.AddConstant(n, name, value);
  }

  inline auto LogPort(Node *n, Port *port, bool right) {
    auto entry = this->access_info.find(n);
    if (entry == this->access_info.end()) {
      auto r = this->access_info.try_emplace(n, std::move(PortAccessLog()));
      if (!r.second)
        std::terminate();
      entry = r.first;
      // FIXME:
      entry->second.first = PortSet();
      entry->second.second = PortSet();
    }

    if (right) {
      entry->second.second.insert(port);
    } else {
      entry->second.first.insert(port);
    }
  }

  // If there is at least one logged access for the given module, return the
  // associated ModuleAccessLog
  inline auto GetModuleInfo(Module *m) -> std::optional<PortAccessLog *> {
    auto info = this->access_info.find(m);
    if (info == this->access_info.end()) {
      return {};
    }

    return &info->second;
  }
};
} // namespace msk::ir
