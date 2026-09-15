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
auto FunctionNodeIntegerMathModule::GenerateTokenString(Out &&out)
    -> absl::Status {
  auto op_c = out.GetConstant<std::string>("operation0");

  if (op_c == "ABSOLUTE" || op_c == "SIGN") {
    std::string glsl_func = (op_c == "ABSOLUTE") ? "abs(" : "sign(";
    out.legacy->AddTokenVector(
        {WildcardToken(Out::RIGHT, "Value3"), TextToken(" = " + glsl_func),
         WildcardToken(Out::LEFT, "Value0"), TextToken(");")});

  } else if (op_c == "NEGATE") {
    out.legacy->AddTokenVector(
        {WildcardToken(Out::RIGHT, "Value3"), TextToken(" = -"),
         WildcardToken(Out::LEFT, "Value0"), TextToken(";")});

  } else if (op_c == "MULTIPLY_ADD") {
    out.legacy->AddTokenVector(
        {WildcardToken(Out::RIGHT, "Value3"), TextToken(" = ("),
         WildcardToken(Out::LEFT, "Value0"), TextToken(" * "),
         WildcardToken(Out::LEFT, "Value1"), TextToken(") + "),
         WildcardToken(Out::LEFT, "Value2"), TextToken(";")});

  } else if (op_c == "MINIMUM" || op_c == "MAXIMUM" || op_c == "POWER" ||
             op_c == "DIVIDE_ROUND" || op_c == "DIVIDE_FLOOR" ||
             op_c == "DIVIDE_CEIL" || op_c == "FLOORED_MODULO" ||
             op_c == "MODULO" || op_c == "GCD" || op_c == "LCM") {
    std::string func_name;
    if (op_c == "MINIMUM") {
      func_name = "min";
    } else if (op_c == "MAXIMUM") {
      func_name = "max";
    } else if (op_c == "POWER") {
      func_name = "glsl_int_pow";
    } else if (op_c == "DIVIDE_ROUND") {
      func_name = "glsl_idiv_round";
    } else if (op_c == "DIVIDE_FLOOR") {
      func_name = "glsl_idiv_floor";
    } else if (op_c == "DIVIDE_CEIL") {
      func_name = "glsl_idiv_ceil";
    } else if (op_c == "FLOORED_MODULO") {
      func_name = "glsl_floored_modulo";
    } else if (op_c == "MODULO") {
      func_name = "glsl_modulo";
    } else if (op_c == "GCD") {
      func_name = "glsl_gcd";
    } else {
      func_name = "glsl_lcm";
    }

    out.legacy->AddTokenVector(
        {WildcardToken(Out::RIGHT, "Value3"),
         TextToken(" = " + func_name + "("), WildcardToken(Out::LEFT, "Value0"),
         TextToken(", "), WildcardToken(Out::LEFT, "Value1"), TextToken(");")});

  } else if (op_c == "ADD" || op_c == "SUBTRACT" || op_c == "MULTIPLY" ||
             op_c == "DIVIDE") {
    std::string sign;
    if (op_c == "ADD") {
      sign = "+";
    } else if (op_c == "SUBTRACT") {
      sign = "-";
    } else if (op_c == "MULTIPLY") {
      sign = "*";
    } else {
      sign = "/";
    }

    out.legacy->AddTokenVector(
        {WildcardToken(Out::RIGHT, "Value3"), TextToken(" = "),
         WildcardToken(Out::LEFT, "Value0"), TextToken(" " + sign + " "),
         WildcardToken(Out::LEFT, "Value1"), TextToken(";")});

  } else {
    out.legacy->AddTokenVector(
        {TextToken("/* ERROR: Unknown Integer Math Operation */")});
  }

  return out.GetStatus();
}
#undef TextToken
#undef WildcardToken
} // namespace msk::blender
