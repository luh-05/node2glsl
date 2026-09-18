#include "blender/modules/modules.hpp"
#include "mir/node_graph/node_graph.hpp"
#include "blender/modules/modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

auto FunctionNodeInputRotation::GenerateTokenString(Out &&out) -> absl::Status {
  auto rotation_var = out.GetConstant<std::string>("");


  out + Out::RIGHT / "Rotation0"
    + "= vec3("
    + Out::LEFT / "Value0"
    + ","
    + Out::LEFT / "Value1"
    + ","
    + Out::LEFT / "Value2"
    + ");";

  return out.GetStatus();
}

} // namespace msk::blender
