#include "blender/root.hpp"
#include "mir/node_graph/GraphContext.hpp"
#include "mir/node_graph/node_graph.hpp"
#include "mollusk/evaluation/evaluator.hpp"
#include "mollusk/plugins/plugins.hpp"
#include <blender/modules/modules.hpp>
#include <memory>
#include <mir/codegen.hpp>
#include <spdlog/spdlog.h>

int main() {
  msk::PluginStore store;
  if (auto s = store.FetchPlugin(); !s.ok()) {
    spdlog::error(s.ToString());
    return 1;
  }

  for (auto p : store.lookup) {
    spdlog::warn("{}", p.first);
  }

  return 0;
}
