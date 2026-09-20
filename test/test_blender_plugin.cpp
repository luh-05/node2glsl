#include "mir/node_graph/node_graph.hpp"
#include "mollusk/plugins/plugins.hpp"
#include <absl/status/status_matchers.h>
#include <blender/modules/modules.hpp>
#include <gtest/gtest.h>
#include <mir/codegen.hpp>
#include <spdlog/spdlog.h>

TEST(BLENDER_PLUGIN_TEST, LOAD_BLENDER_PLUGIN) {
  msk::ModuleStore store;
  ABSL_EXPECT_OK(store.FetchPlugin());
}
