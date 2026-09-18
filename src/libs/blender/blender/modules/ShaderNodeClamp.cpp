#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <string>
#include <mir/codegen.hpp>

namespace msk::blender {

auto ShaderNodeClamp::GenerateTokenString(Out &&out) -> absl::Status {
  auto clamp_type = out.GetConstant<std::string>("clamp_type0");

  if (clamp_type == "MINMAX") {
    out + Out::RIGHT / "Result0"
        + " = clamp("
        + Out::LEFT / "Value0"
        + ", "
        + Out::LEFT / "Min0"
        + ", "
        + Out::LEFT / "Max0"
        + ");";

    return out.GetStatus();
  } 
  
  else if (clamp_type == "RANGE") {
    out + Out::RIGHT / "Result0"
        + " = clamp("
        + Out::LEFT / "Value0"
        + ", min(" + Out::LEFT / "Min0" + ", " + Out::LEFT / "Max0" + ")"
        + ", max(" + Out::LEFT / "Min0" + ", " + Out::LEFT / "Max0" + "));";

    return out.GetStatus();
  } 
  
  else {
    return absl::InvalidArgumentError(
      std::format("Illegal value of clamp_type constant: '{}'", clamp_type));
  }

}

} // namespace msk::blender
