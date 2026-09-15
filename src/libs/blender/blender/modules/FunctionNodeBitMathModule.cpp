#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

auto FunctionNodeBitMathModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto op_c = out.GetConstant<std::string>("operation0");

  if (op_c == "NOT") {
    out + Out::RIGHT / "Value0"
     + "= ~"
     + Out::LEFT / "A0"
     + ";";

    return out.GetStatus();
  }

  if (op_c == "AND" || "OR" || "XOR") {
    std::string sign;

    if (op_c == "AND") {
      sign = "&";
    } else if (op_c == "OR") {
      sign = "|";
    } else if (op_c == "XOR") {
      sign = "^";
    }

    out + Out::RIGHT / "Value0"
     + "="
     + Out::LEFT / "A0"
     + sign
     + Out::LEFT / "B0"
     + ";";

    return out.GetStatus();
  }

  if (op_c == "SHIFT" || "ROTATE") {

    std::string function;

    if (op_c == "SHIFT") {
      function = "glsl_shift(";
    } else if (op_c == "ROTATE") {
      function = "glsl_rotate(";
    }

    out + Out::RIGHT / "Value0" 
      + "="
      + function
      + Out::LEFT / "A0"
      + ","
      + Out::LEFT / "Shift0"
      + ");";

    return out.GetStatus();
  }

  return absl::InvalidArgumentError(
      std::format("Illegal value of operand constant: '{}'", op_c));
}

} // namespace msk::blender
