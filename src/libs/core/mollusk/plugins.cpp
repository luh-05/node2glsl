#include "mir/codegen.hpp"
#include "mir/node_graph/node_graph.hpp"
#include "plugin_abi/plugin_abi.h"
#include <absl/status/status.h>
#include <mollusk/plugins/plugins.hpp>
#include <spdlog/spdlog.h>
#include <vector>

namespace msk {

auto PluginStore::FetchPlugin() -> absl::Status {
  if (!EnumerateModules(
          [](const char *name, ModuleFunc func, void *userdata) {
            auto &l = *static_cast<LookupType *>(userdata);
            return l.try_emplace(std::string(name), func).second;
          },
          &this->lookup)) {
    return absl::InternalError("Failed Enumerating Plugin Modules");
  }

  EnumerateDefinitions(
      [](const char *def, void *userdata) {
        auto &vec = *static_cast<std::vector<std::string> *>(userdata);
        vec.push_back(std::string(def));
      },
      &this->definitions);

  EnumerateCasts(
      [](const char *from, const char *to, const char *pattern,
         void *userdata) {
        auto &vec = *static_cast<std::vector<msk::ir::CastPolicy> *>(userdata);
        vec.push_back(
            {std::string(from), std::string(to), std::string(pattern)});
      },
      &this->cast_policies);

  return absl::OkStatus();
}
} // namespace msk
