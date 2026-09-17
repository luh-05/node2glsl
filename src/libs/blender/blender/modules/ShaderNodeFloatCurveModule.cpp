#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <string>
#include <mir/codegen.hpp>

namespace msk::blender {

auto ShaderNodeFloatCurveModule::GenerateTokenString(Out &&out) -> absl::Status {
  
  //FIXME: implement GLSL Helperfunction
  out + Out::RIGHT / "Value1"
      + " = evaluate_float_curve("
      + Out::LEFT / "Value0"
      + ", "
      + Out::LEFT / "Factor0"
      + ");";

  return out.GetStatus();
}

} // namespace msk::blender
