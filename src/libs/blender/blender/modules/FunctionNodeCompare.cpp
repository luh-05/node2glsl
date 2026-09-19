#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>
#include <string>

namespace msk::blender {

auto GenerateTokenStringFunctionNodeCompare(Out &&out) -> absl::Status {
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

    out + Out::RIGHT / "Value0" + "=" + Out::LEFT / "A0" + sign +
        Out::LEFT / "B0" + ";";

    return out.GetStatus();
  }

  if (data_type == "FLOAT") {
    if (op_c == "EQUAL" || op_c == "NOT_EQUAL") {
      sign = (op_c == "EQUAL") ? "<=" : ">";

      out + Out::RIGHT / "Value0" + "= abs(" + Out::LEFT / "A0" + "-" +
          Out::LEFT / "B0" + ")" + sign + Out::LEFT / "Epsilon0" + ";";

      return out.GetStatus();
    }

    out + Out::RIGHT / "Value0" + "=" + Out::LEFT / "A0" + sign +
        Out::LEFT / "B0" + ";";

    return out.GetStatus();
  }

  if (data_type == "VECTOR") {
    auto mode = out.GetConstant<std::string>("mode0");

    // Hilfsvariablen für die Epsilon-Logik
    bool is_eps = (op_c == "EQUAL" || op_c == "NOT_EQUAL");
    std::string eps_sign = (op_c == "EQUAL") ? "<=" : ">";

    if (mode == "DOT_PRODUCT") {
      if (is_eps) {
        out + Out::RIGHT / "Value0" + "= abs(dot(" + Out::LEFT / "A0" + ", " +
            Out::LEFT / "B0" + ") - " + Out::LEFT / "C0" + ") " + eps_sign +
            " " + Out::LEFT / "Epsilon0" + ";";
      } else {
        out + Out::RIGHT / "Value0" + "= dot(" + Out::LEFT / "A0" + "," +
            Out::LEFT / "B0" + ") " + sign + " " + Out::LEFT / "C0" + ";";
      }
      return out.GetStatus();

    } else if (mode == "DIRECTION") {
      if (is_eps) {
        out + Out::RIGHT / "Value0" + "= abs(acos(clamp(dot(normalize(" +
            Out::LEFT / "A0" + "), normalize(" + Out::LEFT / "B0" +
            ")), -1.0, 1.0)) - " + Out::LEFT / "Angle0" + ") " + eps_sign +
            " " + Out::LEFT / "Epsilon0" + ";";
      } else {
        out + Out::RIGHT / "Value0" + "= acos(clamp(dot(normalize(" +
            Out::LEFT / "A0" + "), normalize(" + Out::LEFT / "B0" +
            ")), -1.0, 1.0)) " + sign + " " + Out::LEFT / "Angle0" + ";";
      }
      return out.GetStatus();

    } else if (mode == "ELEMENT") {
      if (is_eps) {
        std::string logical_op = (op_c == "EQUAL") ? " && " : " || ";
        out + Out::RIGHT / "Value0" + "= (abs(" + Out::LEFT / "A0" + ".x - " +
            Out::LEFT / "B0" + ".x) " + eps_sign + " " +
            Out::LEFT / "Epsilon0" + ")" + logical_op + "(abs(" +
            Out::LEFT / "A0" + ".y - " + Out::LEFT / "B0" + ".y) " + eps_sign +
            " " + Out::LEFT / "Epsilon0" + ")" + logical_op + "(abs(" +
            Out::LEFT / "A0" + ".z - " + Out::LEFT / "B0" + ".z) " + eps_sign +
            " " + Out::LEFT / "Epsilon0" + ");";
      } else {
        out + Out::RIGHT / "Value0" + "= (" + Out::LEFT / "A0" + ".x " + sign +
            " " + Out::LEFT / "B0" + ".x) && (" + Out::LEFT / "A0" + ".y " +
            sign + " " + Out::LEFT / "B0" + ".y) && (" + Out::LEFT / "A0" +
            ".z " + sign + " " + Out::LEFT / "B0" + ".z);";
      }
      return out.GetStatus();

    } else if (mode == "LENGTH") {
      if (is_eps) {
        out + Out::RIGHT / "Value0" + "= abs(length(" + Out::LEFT / "A0" +
            ") - length(" + Out::LEFT / "B0" + ")) " + eps_sign + " " +
            Out::LEFT / "Epsilon0" + ";";
      } else {
        out + Out::RIGHT / "Value0" + "= length(" + Out::LEFT / "A0" + ") " +
            sign + " length(" + Out::LEFT / "B0" + ");";
      }
      return out.GetStatus();

    } else if (mode == "AVERAGE") {
      if (is_eps) {
        out + Out::RIGHT / "Value0" + "= abs(((" + Out::LEFT / "A0" + ".x + " +
            Out::LEFT / "A0" + ".y + " + Out::LEFT / "A0" + ".z) / 3.0) - ((" +
            Out::LEFT / "B0" + ".x + " + Out::LEFT / "B0" + ".y + " +
            Out::LEFT / "B0" + ".z) / 3.0)) " + eps_sign + " " +
            Out::LEFT / "Epsilon0" + ";";
      } else {
        out + Out::RIGHT / "Value0" + "= ((" + Out::LEFT / "A0" + ".x + " +
            Out::LEFT / "A0" + ".y + " + Out::LEFT / "A0" + ".z) / 3.0) " +
            sign + " ((" + Out::LEFT / "B0" + ".x + " + Out::LEFT / "B0" +
            ".y + " + Out::LEFT / "B0" + ".z) / 3.0);";
      }
      return out.GetStatus();

    } else {
      return absl::InvalidArgumentError(
          std::format("Illegal value of mode constant: '{}'", mode));
    }
  }

  // FIXME: implement other data types
  return absl::UnimplementedError(std::format(
      "Given Datatype has not been implemented yet: '{}'", data_type));
}

} // namespace msk::blender
