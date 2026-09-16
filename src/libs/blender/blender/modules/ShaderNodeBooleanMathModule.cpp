#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <string>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

auto ShaderNodeBooleanMathModule::GenerateTokenString(Out &&out) -> absl::Status {
  // Lese die Konstante für die Operation aus
  auto op_c = out.GetConstant<std::string>("operation0");

  // Single-input operation edge case
  if (op_c == "NOT") {
    out + Out::RIGHT / "Boolean2" 
    + " = !" + Out::LEFT / "Boolean0" 
    + ";" = 1;
    return out.GetStatus();
  }

  // Two-input operations configuration
  std::string prefix = " ";
  std::string infix = " && ";
  std::string suffix = ";";

  if (op_c == "AND") {
    infix = " && ";
  } else if (op_c == "OR") {
    infix = " || ";
  } else if (op_c == "NAND" || op_c == "NOT AND") {
    prefix = " !(";
    infix = " && ";
    suffix = ");";
  } else if (op_c == "NOR") {
    prefix = " !(";
    infix = " || ";
    suffix = ");";
  } else if (op_c == "XOR" || op_c == "NOT EQUAL") {
    infix = " != ";
  } else if (op_c == "XNOR" || op_c == "EQUAL") {
    infix = " == ";
  } else if (op_c == "IMPLY") {
    prefix = " (!";
    infix = " || ";
    suffix = ");";
  } else if (op_c == "NIMPLY" || op_c == "SUBTRACT") {
    prefix = " (";
    infix = " && !";
    suffix = ");";
  }

  // Token String generieren und Zeile abschließen ( = 1 )
  out + Out::RIGHT / "Boolean2" + " =" + prefix + 
        Out::LEFT / "Boolean0" + infix + 
        Out::LEFT / "Boolean1" + suffix = 1;

  return out.GetStatus();
}

} // namespace msk::blender