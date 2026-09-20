#pragma once

#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <map>
#include <plugin_abi/plugin_abi.h>
#include <string>
#include <vector>

namespace msk {

class ModuleStore {
public:
  using LookupType = std::map<std::string, ModuleFunc>;
  LookupType lookup;

public:
  /// TODO: Make this an actual plugin implementation
  /// Currently mock implemention, only uses Blender "plugin"
  auto FetchPlugin() -> absl::Status;
};

} // namespace msk
