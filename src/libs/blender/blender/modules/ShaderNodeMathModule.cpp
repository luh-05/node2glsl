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

auto ShaderNodeMath::GenerateTokenString(Out &&out) -> absl::Status {
  auto op = out.GetConstant<std::string>("operation0");
  bool use_clamp = (out.GetConstant<std::string>("use_clamp0") == "True");

  // 1. Zuweisung des Outputs starten
  out.legacy->AddTokenVector({
      WildcardToken(Out::RIGHT, "Value3"), 
      TextToken(" = ")
  });

  if (use_clamp) {
    out.legacy->AddTokenVector({ TextToken("clamp(") });
  }

  // 2. Den eigentlichen Ausdruck einfügen
  // Direct Arithmetic Operations
  if (op == "ADD") {
    out.legacy->AddTokenVector({WildcardToken(Out::LEFT, "Value0"), TextToken(" + "), WildcardToken(Out::LEFT, "Value1")});
  } else if (op == "SUBTRACT") {
    out.legacy->AddTokenVector({WildcardToken(Out::LEFT, "Value0"), TextToken(" - "), WildcardToken(Out::LEFT, "Value1")});
  } else if (op == "MULTIPLY") {
    out.legacy->AddTokenVector({WildcardToken(Out::LEFT, "Value0"), TextToken(" * "), WildcardToken(Out::LEFT, "Value1")});
  } else if (op == "DIVIDE") {
    out.legacy->AddTokenVector({WildcardToken(Out::LEFT, "Value0"), TextToken(" / "), WildcardToken(Out::LEFT, "Value1")});
  } else if (op == "MULTIPLY_ADD") {
    out.legacy->AddTokenVector({WildcardToken(Out::LEFT, "Value0"), TextToken(" * "), WildcardToken(Out::LEFT, "Value1"), TextToken(" + "), WildcardToken(Out::LEFT, "Value2")});
  }
  // Power & Logarithmic
  else if (op == "POWER") {
    out.legacy->AddTokenVector({TextToken("pow("), WildcardToken(Out::LEFT, "Value0"), TextToken(", "), WildcardToken(Out::LEFT, "Value1"), TextToken(")")});
  } else if (op == "LOGARITHM") {
    out.legacy->AddTokenVector({TextToken("log("), WildcardToken(Out::LEFT, "Value0"), TextToken(") / log("), WildcardToken(Out::LEFT, "Value1"), TextToken(")")});
  } else if (op == "SQRT") {
    out.legacy->AddTokenVector({TextToken("sqrt("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "INVERSE_SQRT") {
    out.legacy->AddTokenVector({TextToken("inversesqrt("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "EXPONENT") {
    out.legacy->AddTokenVector({TextToken("exp("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  }
  // Comparison & Min/Max
  else if (op == "MINIMUM") {
    out.legacy->AddTokenVector({TextToken("min("), WildcardToken(Out::LEFT, "Value0"), TextToken(", "), WildcardToken(Out::LEFT, "Value1"), TextToken(")")});
  } else if (op == "MAXIMUM") {
    out.legacy->AddTokenVector({TextToken("max("), WildcardToken(Out::LEFT, "Value0"), TextToken(", "), WildcardToken(Out::LEFT, "Value1"), TextToken(")")});
  } else if (op == "LESS_THAN") {
    out.legacy->AddTokenVector({TextToken("float("), WildcardToken(Out::LEFT, "Value0"), TextToken(" < "), WildcardToken(Out::LEFT, "Value1"), TextToken(")")});
  } else if (op == "GREATER_THAN") {
    out.legacy->AddTokenVector({TextToken("float("), WildcardToken(Out::LEFT, "Value0"), TextToken(" > "), WildcardToken(Out::LEFT, "Value1"), TextToken(")")});
  } else if (op == "COMPARE") {
    out.legacy->AddTokenVector({TextToken("float(abs("), WildcardToken(Out::LEFT, "Value0"), TextToken(" - "), WildcardToken(Out::LEFT, "Value1"), TextToken(") <= "), WildcardToken(Out::LEFT, "Value2"), TextToken(")")});
  }
  // Rounding & Modulo
  else if (op == "ABSOLUTE") {
    out.legacy->AddTokenVector({TextToken("abs("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "SIGN") {
    out.legacy->AddTokenVector({TextToken("sign("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "FLOOR") {
    out.legacy->AddTokenVector({TextToken("floor("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "CEIL") {
    out.legacy->AddTokenVector({TextToken("ceil("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "FRACT") {
    out.legacy->AddTokenVector({TextToken("fract("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "TRUNC") {
    out.legacy->AddTokenVector({TextToken("trunc("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "ROUND") {
    out.legacy->AddTokenVector({TextToken("floor("), WildcardToken(Out::LEFT, "Value0"), TextToken(" + 0.5)")});
  } else if (op == "MODULO" || op == "FLOORED_MODULO") {
    out.legacy->AddTokenVector({TextToken("mod("), WildcardToken(Out::LEFT, "Value0"), TextToken(", "), WildcardToken(Out::LEFT, "Value1"), TextToken(")")});
  } else if (op == "SNAP") {
    out.legacy->AddTokenVector({TextToken("floor("), WildcardToken(Out::LEFT, "Value0"), TextToken(" / "), WildcardToken(Out::LEFT, "Value1"), TextToken(") * "), WildcardToken(Out::LEFT, "Value1")});
  }
  // Trigonometry
  else if (op == "SINE") {
    out.legacy->AddTokenVector({TextToken("sin("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "COSINE") {
    out.legacy->AddTokenVector({TextToken("cos("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "TANGENT") {
    out.legacy->AddTokenVector({TextToken("tan("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "ARCSINE") {
    out.legacy->AddTokenVector({TextToken("asin("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "ARCCOSINE") {
    out.legacy->AddTokenVector({TextToken("acos("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "ARCTANGENT") {
    out.legacy->AddTokenVector({TextToken("atan("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "ARCTAN2") {
    out.legacy->AddTokenVector({TextToken("atan("), WildcardToken(Out::LEFT, "Value0"), TextToken(", "), WildcardToken(Out::LEFT, "Value1"), TextToken(")")});
  } else if (op == "SINH") {
    out.legacy->AddTokenVector({TextToken("sinh("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "COSH") {
    out.legacy->AddTokenVector({TextToken("cosh("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "TANH") {
    out.legacy->AddTokenVector({TextToken("tanh("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else if (op == "RADIANS") {
    out.legacy->AddTokenVector({TextToken("radians("), WildcardToken(Out::LEFT, "Value0"), TextToken(")")});
  } else {
    out.legacy->AddTokenVector({WildcardToken(Out::LEFT, "Value0")});
  }

  // 3. Statement beenden (inkl. eventuellem Clamp)
  if (use_clamp) {
    out.legacy->AddTokenVector({ TextToken(", 0.0, 1.0);") });
  } else {
    out.legacy->AddTokenVector({ TextToken(";") });
  }

  return out.GetStatus();
}

#undef TextToken
#undef WildcardToken

} // namespace msk::blender