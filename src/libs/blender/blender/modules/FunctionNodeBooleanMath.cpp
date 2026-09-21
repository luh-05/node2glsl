#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <memory>
#include <mir/codegen.hpp>
#include <string>

namespace msk::blender {

auto GenerateTokenStringFunctionNodeBooleanMath(Out &out) -> absl::Status {
  auto op_c = out.GetConstant<std::string>("operation0");

  if (op_c == "NOT") {
    out + Out::RIGHT / "Boolean2" + " = !" + Out::LEFT / "Boolean0" + ";";
    return out.GetStatus();
  }

  std::string prefix = "";
  std::string infix = "";
  std::string suffix = ";";

  if (op_c == "AND") {
    infix = " && ";
  } else if (op_c == "OR") {
    infix = " || ";
  } else if (op_c == "NAND") {
    prefix = "!(";
    infix = " && ";
    suffix = ");";
  } else if (op_c == "NOR") {
    prefix = "!(";
    infix = " || ";
    suffix = ");";
  } else if (op_c == "XOR") {
    infix = " != ";
  } else if (op_c == "XNOR") {
    infix = " == ";
  } else if (op_c == "IMPLY") {
    prefix = "(!";
    infix = " || ";
    suffix = ");";
  } else if (op_c == "NIMPLY") {
    prefix = "(";
    infix = " && !";
    suffix = ");";
  } else {
    return absl::InvalidArgumentError(
        std::format("Illegal value of operation constant: '{}'", op_c));
  }

  out + Out::RIGHT / "Boolean2" + " = " + prefix + Out::LEFT / "Boolean0" +
      infix + Out::LEFT / "Boolean1" + suffix;

  return out.GetStatus();
}

} // namespace msk::blender
