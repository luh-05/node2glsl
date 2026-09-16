
#include "blender/ir_shim/ir_shim.hpp"
#include <absl/status/statusor.h>
#include <spdlog/spdlog.h>

using GraphShim = msk::blender::GraphShim;

#define CHECK_OK(var, stmt)                                                    \
  auto var##_s = stmt;                                                         \
  if (!var##_s.ok()) {                                                         \
    spdlog::error(var##_s.status().ToString());                                \
    return 1;                                                                  \
  }                                                                            \
  auto var = *var##_s;

int main() {
  GraphShim g;
  CHECK_OK(foo, g.AddModule(g.GetGraph(), "foo.0", "FooModule"));
  CHECK_OK(bar, g.AddSubGraph(g.GetGraph(), "bar.0"));

  CHECK_OK(foo_again, g.GetModule(g.GetGraph(), "foo.0"));
  CHECK_OK(foo_value0,
           g.AddPort(foo_again, GraphShim::LEFT, "value0", "float"));
  CHECK_OK(bar_value0, g.AddPort(bar, GraphShim::RIGHT, "value1", "float"));

  if (auto conn_c = g.ConnectPorts(foo_value0, bar_value0); !conn_c.ok()) {
    spdlog::error(conn_c.ToString());
    return 1;
  }

  return 0;
}
