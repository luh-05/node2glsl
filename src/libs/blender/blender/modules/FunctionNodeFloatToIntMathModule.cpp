#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>
#include <string>

namespace msk::blender {

auto FunctionNodeFloatToIntMathModule::GenerateTokenString(Out &&out)
    -> absl::Status {
  auto mode = out.GetConstant<std::string>("rounding_mode0");
  std::string function;

  if (mode == "ROUND") {
    function = "round";
  } else if (mode == "FLOOR") {
    function = "floor";
  } else if (mode == "CEILING") {
    function = "ceil";
  } else if (mode == "TRUNCATE") {
    function = "trunc";
  } else {
    return absl::InvalidArgumentError(
        std::format("Unknown operation: '{}'", mode));
  }

  out + Out::RIGHT / "Integer" 
    + " = int(" 
    + function 
    + "(" 
    + Out::LEFT / "Float" 
    + "));";

  return out.GetStatus();
}

} // namespace msk::blender
