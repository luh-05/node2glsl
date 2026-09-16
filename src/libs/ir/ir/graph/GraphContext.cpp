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

// Sanitizes floats (removes f suffix)
template <class T> auto convertString(std::string_view s) -> absl::StatusOr<T> {
  if constexpr (std::is_floating_point_v<T>) {
    if (!s.empty() && (s.back() == 'f' || s.back() == 'F'))
      s.remove_suffix(1);
  }

  T value{};
  auto [ptr, err] = std::from_chars(s.data(), s.data() + s.size(), value);

  if (err != std::errc{} || ptr != s.data() + s.size()) {
    return absl::InvalidArgumentError(std::format(
        "Could not convert '{0}' to type '{1}'{2}", s, typeid(T).name(),
        err != std::errc{} ? std::format(" (code: {})", err) : ""));
  }

  return value;
}

// TODO: Maybe change to boost:lexical_cast in the future?
template <class T>
auto ConstantStore::GetConstant(Node *node, std::string_view name)
    -> absl::StatusOr<T> {
  if (!this->map.contains(node)) {
    return absl::NotFoundError(
        std::format("Node not registered in constant store!"));
  }

  if (!this->map.at(node).contains(name)) {
    return absl::NotFoundError(std::format("Constant '{}' not found!", name));
  }

  auto value_string = this->map.at(node).at(name);

  if constexpr (std::is_assignable_v<T, std::string> ||
                std::is_assignable_v<T, std::string_view> ||
                std::is_assignable_v<T, const char *>) {
    return value_string;
  }
  if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
    return convertString<T>(value_string);
  }
  if constexpr (std::is_same_v<T, bool>) {
    auto value_lower = std::string(value_string);
    std::transform(value_lower.begin(), value_lower.end(), value_lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if (value_lower == "true" || value_lower == "1") {
      return true;
    } else if (value_lower == "false" || value_lower == "0") {
      return false;
    }
    return absl::InvalidArgumentError(std::format(
        "Could not convert constant '{}' with value '{}' to boolean!", name,
        value_string));
  }

  return absl::InvalidArgumentError(
      std::format("Type '{}' not supported!", typeid(T).name()));
}

template <class T>
auto GraphContext::GetConstant(Node *n, std::string_view &&name)
    -> absl::StatusOr<T> {
  return this->const_store.GetConstant<T>(n, name);
}

ContextProvider::ContextProvider(std::shared_ptr<GraphContext> context) {
  this->context = context;
}

template <class T> auto Module::Out::GetConstant(std::string name) -> T {
  // if (auto s = cxt->GetConstant<T>(static_cast<Node *>(&parent), name);
  //     !s.ok()) {
  //   if (this->status.ok())
  //     this->status = s.status();
  //   return 0;
  // } else
  //   return s.value();
  return (T)0;
}

} // namespace msk::ir
