#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <fmt/base.h>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

auto FunctionNodeIntegerMath::GenerateTokenString(Out &&out) -> absl::Status {
  auto op_c = out.GetConstant<std::string>("operation0");

  if (op_c == "ADD") {
    out + Out::RIGHT / "Value3" + " = " + Out::LEFT / "Value0" + " + " +
        Out::LEFT / "Value1" + ";";
  } else if (op_c == "SUBTRACT") {
    out + Out::RIGHT / "Value3" + " = " + Out::LEFT / "Value0" + " - " +
        Out::LEFT / "Value1" + ";";
  } else if (op_c == "MULTIPLY") {
    out + Out::RIGHT / "Value3" + " = " + Out::LEFT / "Value0" + " * " +
        Out::LEFT / "Value1" + ";";
  } else if (op_c == "DIVIDE") {
    out + Out::RIGHT / "Value3" + " = " + Out::LEFT / "Value0" + " / " +
        Out::LEFT / "Value1" + ";";
  } else if (op_c == "POWER") {
    out + Out::RIGHT / "Value3" + " = int(pow(float(" + Out::LEFT / "Value0" +
        "), float(" + Out::LEFT / "Value1" + ")));";
  } else if (op_c == "MINIMUM") {
    out + Out::RIGHT / "Value3" + " = min(" + Out::LEFT / "Value0" + ", " +
        Out::LEFT / "Value1" + ");";
  } else if (op_c == "MAXIMUM") {
    out + Out::RIGHT / "Value3" + " = max(" + Out::LEFT / "Value0" + ", " +
        Out::LEFT / "Value1" + ");";
  }

  else if (op_c == "MULTIPLY_ADD") {
    out + Out::RIGHT / "Value3" + " = (" + Out::LEFT / "Value0" + " * " +
        Out::LEFT / "Value1" + ") + " + Out::LEFT / "Value2" + ";";
  }

  else if (op_c == "ABSOLUTE") {
    out + Out::RIGHT / "Value3" + " = abs(" + Out::LEFT / "Value0" + ");";
  } else if (op_c == "NEGATE") {
    out + Out::RIGHT / "Value3" + " = -(" + Out::LEFT / "Value0" + ");";
  } else if (op_c == "SIGN") {
    out + Out::RIGHT / "Value3" + " = int(sign(float(" + Out::LEFT / "Value0" +
        ")));";
  }

  else if (op_c == "DIVIDE_ROUND") {
    out + Out::RIGHT / "Value3" + " = int(round(float(" + Out::LEFT / "Value0" +
        ") / float(" + Out::LEFT / "Value1" + ")));";
  } else if (op_c == "DIVIDE_FLOOR") {
    out + Out::RIGHT / "Value3" + " = int(floor(float(" + Out::LEFT / "Value0" +
        ") / float(" + Out::LEFT / "Value1" + ")));";
  } else if (op_c == "DIVIDE_CEIL") {
    out + Out::RIGHT / "Value3" + " = int(ceil(float(" + Out::LEFT / "Value0" +
        ") / float(" + Out::LEFT / "Value1" + ")));";
  } else if (op_c == "MODULO") {
    out + Out::RIGHT / "Value3" + " = " + Out::LEFT / "Value0" + " % " +
        Out::LEFT / "Value1" + ";";
  } else if (op_c == "FLOORED_MODULO") {
    out + Out::RIGHT / "Value3" + " = ((" + Out::LEFT / "Value0" + " % " +
        Out::LEFT / "Value1" + ") + " + Out::LEFT / "Value1" + ") % " +
        Out::LEFT / "Value1" + ";";
  }

  else if (op_c == "GCD") {
    

    out + "{";
    out + "if(" + Out::LEFT / "Value0" + "= 0 or" + Out::LEFT / "Value1" +
        "= 0){";
    out + "return max(" + Out::LEFT / "Value0" + "," + Out::LEFT / "Value1" +
        ");";
    out + "}";
    out + "int result = min(" + Out::LEFT / "Value0" + "," +
        Out::LEFT / "Value1" + ");";

    out + "while (result>0){";
    out + "if(mod(" + Out::LEFT / "Value0" + ",result)==0 && mod(" +
        Out::LEFT / "Value1" + ",result)==0){";
    out + "break;";
    out + "}";
    out + "result--;";
    out + "}";
    out + Out::RIGHT / "Value3" + "= result;";
    out + "}";

  }

  else if (op_c == "LCM") {
    out + "{";

    out + "int lcm_a = abs(" + Out::LEFT / "Value0" + ");";

    out + "int lcm_b = abs(" + Out::LEFT / "Value1" + ");";

    out + "int lcm_x = lcm_a;" + "int lcm_y = lcm_b;";

    out + "while (lcm_y != 0) {" + "int lcm_tmp = lcm_x % lcm_y;" +
        "lcm_x = lcm_y;" + "lcm_y = lcm_tmp;" + "}";

    out + Out::RIGHT / "Value3" +
        " = (lcm_x == 0) ? 0 : abs((lcm_a / lcm_x) * lcm_b);";
    out + "}";

    return out.GetStatus();
  } else {
    return absl::InvalidArgumentError(std::format(
        "Illegal value of operation constant in Integer Math: '{}'", op_c));
  }

  return out.GetStatus();
}

} // namespace msk::blender
