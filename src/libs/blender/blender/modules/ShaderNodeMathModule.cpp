#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <string>
#include <mir/codegen.hpp>

namespace msk::blender {

auto ShaderNodeMathModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto op_c = out.GetConstant<std::string>("operation0");
  
  // Clamp Tickbox
  bool use_clamp = (out.GetConstant<std::string>("use_clamp0") == "True");


  if(op_c == "ADD" || "SUBTRACT" || "MULTIPLY" || "DIVIDE"){  
    std::string sign;

    if (op_c == "ADD") {
      sign = "+";
    
  } else if (op_c == "SUBTRACT") {
    sign = "-";
   } else if (op_c == "MULTIPLY") {
    sign = "*";
  } else if (op_c == "DIVIDE") {
    sign = "/";
  }
  out + Out::RIGHT / "Value0"
    + "="
    + Out::LEFT / "A0"
    + sign 
    + Out::LEFT / "B0"
    + ";";

  return.out.GetStatus();

  } else if (op == "MULTIPLY_ADD") {
      // a * b + c

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