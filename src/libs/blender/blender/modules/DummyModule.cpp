#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <mir/codegen.hpp>
#include <mir/codegen_utils.hpp>

namespace msk::blender {
auto DummyModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto a = out.GetConstant<uint32_t>("a");
  auto b = out.GetConstant<bool>("b");
  // auto c = out.GetConstant<uint32_t>("foo");

  out + "Constant 'a' is: " + a = 1;
  out + "bla" = 1;
  out + "Constant 'a' == 4: " + (a == 4) = 1;
  out + "Left port 'value0' resolves to: " + Out::LEFT / "value0" = 1;
  out + "I'm not flushed!" = 2;

  out + "Constant 'b' is " + b = 1;

  return out.GetStatus();
}
} // namespace msk::blender
