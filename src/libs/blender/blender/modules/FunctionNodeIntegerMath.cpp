#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <fmt/base.h>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

auto FunctionNodeIntegerMath::GenerateTokenString(Out &&out)
    -> absl::Status {
  auto op_c = out.GetConstant<std::string>("operation0");

  if (op_c == "ADD") {
    out + Out::RIGHT / "Value3"
        + " = "
        + Out::LEFT / "Value0"
        + " + "
        + Out::LEFT / "Value1"
        + ";";
  } 
  else if (op_c == "SUBTRACT") {
    out + Out::RIGHT / "Value3"
        + " = "
        + Out::LEFT / "Value0"
        + " - "
        + Out::LEFT / "Value1"
        + ";";
  } 
  else if (op_c == "MULTIPLY") {
    out + Out::RIGHT / "Value3"
        + " = "
        + Out::LEFT / "Value0"
        + " * "
        + Out::LEFT / "Value1"
        + ";";
  } 
  else if (op_c == "DIVIDE") {
    out + Out::RIGHT / "Value3"
        + " = "
        + Out::LEFT / "Value0"
        + " / "
        + Out::LEFT / "Value1"
        + ";";
  } 
  else if (op_c == "POWER") {
    out + Out::RIGHT / "Value3"
        + " = int(pow(float("
        + Out::LEFT / "Value0"
        + "), float("
        + Out::LEFT / "Value1"
        + ")));";
  } 
  else if (op_c == "MINIMUM") {
    out + Out::RIGHT / "Value3"
        + " = min("
        + Out::LEFT / "Value0"
        + ", "
        + Out::LEFT / "Value1"
        + ");";
  } 
  else if (op_c == "MAXIMUM") {
    out + Out::RIGHT / "Value3"
        + " = max("
        + Out::LEFT / "Value0"
        + ", "
        + Out::LEFT / "Value1"
        + ");";
  }

  else if (op_c == "MULTIPLY_ADD") {
    out + Out::RIGHT / "Value3"
        + " = ("
        + Out::LEFT / "Value0"
        + " * "
        + Out::LEFT / "Value1"
        + ") + "
        + Out::LEFT / "Value2"
        + ";";
  }

  else if (op_c == "ABSOLUTE") {
    out + Out::RIGHT / "Value3"
        + " = abs("
        + Out::LEFT / "Value0"
        + ");";
  } 
  else if (op_c == "NEGATE") {
    out + Out::RIGHT / "Value3"
        + " = -("
        + Out::LEFT / "Value0"
        + ");";
  } 
  else if (op_c == "SIGN") {
    out + Out::RIGHT / "Value3"
        + " = int(sign(float("
        + Out::LEFT / "Value0"
        + ")));";
  }

  else if (op_c == "DIVIDE_ROUND") {
    out + Out::RIGHT / "Value3"
        + " = int(round(float("
        + Out::LEFT / "Value0"
        + ") / float("
        + Out::LEFT / "Value1"
        + ")));";
  } 
  else if (op_c == "DIVIDE_FLOOR") {
    out + Out::RIGHT / "Value3"
        + " = int(floor(float("
        + Out::LEFT / "Value0"
        + ") / float("
        + Out::LEFT / "Value1"
        + ")));";
  } 
  else if (op_c == "DIVIDE_CEIL") {
    out + Out::RIGHT / "Value3"
        + " = int(ceil(float("
        + Out::LEFT / "Value0"
        + ") / float("
        + Out::LEFT / "Value1"
        + ")));";
  } 
  else if (op_c == "MODULO") {
    out + Out::RIGHT / "Value3"
        + " = "
        + Out::LEFT / "Value0"
        + " % "
        + Out::LEFT / "Value1"
        + ";";
  } 
  else if (op_c == "FLOORED_MODULO") {
    out + Out::RIGHT / "Value3"
        + " = (("
        + Out::LEFT / "Value0"
        + " % "
        + Out::LEFT / "Value1"
        + ") + "
        + Out::LEFT / "Value1"
        + ") % "
        + Out::LEFT / "Value1"
        + ";";
  }

  else if (op_c == "GCD" || op_c == "LCM") {
    // FIXME: Implement GLSL Helperfunctions
    std::string func_name = (op_c == "GCD") ? "integer_gcd" : "integer_lcm";
    out + Out::RIGHT / "Value3"
        + " = "
        + func_name
        + "("
        + Out::LEFT / "Value0"
        + ", "
        + Out::LEFT / "Value1"
        + ");";
  } 
  else {
    return absl::InvalidArgumentError(
      std::format("Illegal value of operation constant in Integer Math: '{}'", op_c));
  }

  return out.GetStatus();
}

} // namespace msk::blender
