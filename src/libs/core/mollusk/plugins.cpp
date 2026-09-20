#include "mir/node_graph/node_graph.hpp"
#include "plugin_abi/plugin_abi.h"
#include <absl/status/status.h>
#include <mollusk/plugins/plugins.hpp>

namespace msk {

auto ModuleStore::FetchPlugin() -> absl::Status {
  EnumerateModules(
      [](const char *name, ModuleFunc func, void *userdata) {
        auto &l = *static_cast<LookupType *>(userdata);
        l.try_emplace(std::string(name), func);
      },
      &this->lookup);

  return absl::OkStatus();
}
} // namespace msk
