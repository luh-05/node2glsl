
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <flat_map>
#include <mir/node_graph/node_graph.hpp>
#pragma once

namespace msk::ir {
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

class GraphContext {
public:
  ConstantStore const_store;

public:
  template <class T>
  auto GetConstant(Node *n, std::string_view name) -> absl::StatusOr<T> {
    return this->const_store.GetConstant<T>(n, name);
  }

  inline auto AddConstant(Node *n, std::string_view name, std::string value)
      -> absl::Status {
    return this->const_store.AddConstant(n, name, value);
  }
};
} // namespace msk::ir
