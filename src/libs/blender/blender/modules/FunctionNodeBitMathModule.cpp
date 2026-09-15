#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

#define TextToken(text) out.legacy->CreateMTT(text)
#define WildcardToken(p, name) out.legacy->CreateMWT(p, name)
auto FunctionNodeBitMathModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto op_c = out.GetConstant<std::string>("operation0");

  if (op_c == "NOT") {
    out.legacy->AddTokenVector(
        {WildcardToken(Out::RIGHT, "Value0"), TextToken(" = ~"),
         WildcardToken(Out::LEFT, "A0"), TextToken(";")});

  } else if (op_c == "AND" || op_c == "OR" || op_c == "XOR") {
    std::string sign;
    if (op_c == "AND") {
      sign = "&";
    } else if (op_c == "OR") {
      sign = "|";
    } else {
      sign = "^";
    }

    out.legacy->AddTokenVector(
        {WildcardToken(Out::RIGHT, "Value0"), TextToken(" = "),
         WildcardToken(Out::LEFT, "A0"), TextToken(" " + sign + " "),
         WildcardToken(Out::LEFT, "B0"), TextToken(";")});

  } else if (op_c == "SHIFT") {
    // TODO: implement Helper Functions
    out.legacy->AddTokenVector(
        {WildcardToken(Out::RIGHT, "Value0"), TextToken(" = glsl_shift("),
         WildcardToken(Out::LEFT, "A0"), TextToken(", "),
         WildcardToken(Out::LEFT, "Shift0"), TextToken(");")});

  } else if (op_c == "ROTATE") {
    out.legacy->AddTokenVector(
        {WildcardToken(Out::RIGHT, "Value0"), TextToken(" = glsl_rotate("),
         WildcardToken(Out::LEFT, "A0"), TextToken(", "),
         WildcardToken(Out::LEFT, "Shift0"), TextToken(");")});

  } else {
    out.legacy->AddTokenVector(
        {TextToken("/* ERROR: Unknown Bitwise Operation */")});
  }

  return out.GetStatus();
}
#undef TextToken
#undef WildcardToken
} // namespace msk::blender
