#include "blender/modules/modules.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

auto GenerateTokenStringFunctionNodeInputRotation(Out &&out) -> absl::Status {
  auto rotation_value0 = out.GetConstant<std::string>("Value0");
  auto rotation_value1 = out.GetConstant<std::string>("Value1");
  auto rotation_value2 = out.GetConstant<std::string>("Value2");
  auto rotation_value3 = out.GetConstant<std::string>("Value3");

  out + Out::RIGHT / "Rotation0" + "= vec4(" + rotation_value0 + "," +
      rotation_value1 + "," + rotation_value2 + "," + rotation_value3 + ");";

  return out.GetStatus();
}

} // namespace msk::blender
