#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <string>
#include <mir/codegen.hpp>

namespace msk::blender {

#define TextToken(text) out.legacy->CreateMTT(text)
#define WildcardToken(p, name) out.legacy->CreateMWT(p, name)

auto FunctionNodeCompareModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto op = out.GetConstant<std::string>("operation0");
  auto dt = out.GetConstant<std::string>("data_type0");

  // Route correct ports based on data type
  std::string port_a = "A0";
  std::string port_b = "B0";

  if (dt == "INT") {
    port_a = "A1";
    port_b = "B1";
  } else if (dt == "VECTOR") {
    port_a = "A2";
    port_b = "B2";
  } else if (dt == "COLOR") {
    port_a = "A3";
    port_b = "B3";
  } else if (dt == "STRING") {
    port_a = "A4";
    port_b = "B4";
  }

  // Edge case: Blender's Float comparisons for EQUAL / NOT_EQUAL use Epsilon
  if (dt == "FLOAT" && (op == "EQUAL" || op == "NOT_EQUAL")) {
    std::string comp_operator = (op == "EQUAL") ? " <= " : " > ";

    out.legacy->AddTokenVector({
        WildcardToken(Out::RIGHT, "Result0"),
        TextToken(" = abs("),
        WildcardToken(Out::LEFT, port_a),
        TextToken(" - "),
        WildcardToken(Out::LEFT, port_b),
        TextToken(")"),
        TextToken(comp_operator),
        WildcardToken(Out::LEFT, "Epsilon0"),
        TextToken(";")
    });

    return out.GetStatus();
  }

  // Standard operator configuration
  std::string infix = " == ";

  if (op == "LESS_THAN") {
    infix = " < ";
  } else if (op == "LESS_EQUAL") {
    infix = " <= ";
  } else if (op == "GREATER_THAN") {
    infix = " > ";
  } else if (op == "GREATER_EQUAL") {
    infix = " >= ";
  } else if (op == "NOT_EQUAL") {
    infix = " != ";
  }

  out.legacy->AddTokenVector({
      WildcardToken(Out::RIGHT, "Result0"),
      TextToken(" = "),
      WildcardToken(Out::LEFT, port_a),
      TextToken(infix),
      WildcardToken(Out::LEFT, port_b),
      TextToken(";")
  });

  return out.GetStatus();
}

#undef TextToken
#undef WildcardToken

} // namespace msk::blender