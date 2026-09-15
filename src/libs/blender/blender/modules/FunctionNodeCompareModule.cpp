#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <string>
#include <mir/codegen.hpp>

namespace msk::blender {

auto FunctionNodeCompareModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto op_c = out.GetConstant<std::string>("operation0");
  auto data_type = out.GetConstant<std::string>("data_type0");
  std::string sign;

  if (op_c == "LESS_THAN") {
    sign = "<";
  } else if (op_c == "LESS_EQUAL") {
    sign = "<=";
  } else if (op_c == "GREATER_THAN") {
    sign = ">";
  } else if (op_c == "GREATER_EQUAL") {
    sign = ">=";
  } else if (op_c == "EQUAL") {
    sign = "==";
  } else if (op_c == "NOT_EQUAL") {
    sign = "!=";
  } else {
    return absl::InvalidArgumentError(
      std::format("Illegal value of operand constant: '{}'", op_c));
  }
  
  if (data_type == "INT") {

    out + Out::RIGHT / "Value0"
      + "="
      + Out::LEFT / "A0"
      + sign
      + Out::LEFT / "B0"
      + ";";

    return out.GetStatus();
  } 


  if (data_type == "FLOAT") {
    if (op_c == "EQUAL" || "NOT_EQUAL") {
      auto epsilon = out.GetConstant<std::string>("Epsilon0");
      op_c == "EQUAL" ? sign = "==" : sign = "!=";

      out + Out::RIGHT / "Value0"
        + "= abs("
        + Out::LEFT / "A0"
        + "-"
        + Out::LEFT / "B0"
        + ")"
        + "sign"
        + epsilon
        + ";";

      return out.GetStatus();
    }

    out + Out::RIGHT / "Value0"
      + "="
      + Out::LEFT / "A0"
      + sign
      + Out::LEFT / "B0"
      + ";";

    return out.GetStatus();
  } 

  if (data_type == "VECTOR") {
    auto mode = out.GetConstant<std::string>("mode0");

    //TODO: implement vectors

  }
}

} // namespace msk::blender
