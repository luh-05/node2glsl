#pragma once

#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <map>
#include <plugin_abi/plugin_abi.h>
#include <span>
#include <string>
#include <vector>

namespace msk {

class PluginStore {
public:
  using LookupType = std::map<std::string, ModuleFunc>;
  LookupType lookup;

  std::vector<std::string> definitions;

public:
  /// TODO: Make this an actual plugin implementation
  /// Currently mock implemention, only uses Blender "plugin"
  auto FetchPlugin() -> absl::Status;

  auto GetModuleFunc(std::string_view name) -> absl::StatusOr<ModuleFunc> {
    auto f = this->lookup.find(std::string(name));
    if (f == this->lookup.end()) {
      return absl::NotFoundError(std::format(
          "No Module implementation with name '{}' found in store!", name));
    }

    return f->second;
  }

  auto GetPPDefinitions() -> std::span<std::string> {
    return this->definitions;
  }
};

} // namespace msk
