#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>
#include <string>

namespace msk::blender {

auto ShaderNodeMath::GenerateTokenString(Out &&out) -> absl::Status {
  auto op_c = out.GetConstant<std::string>("operation0");

// Clamp Tickbox
// vielleicht falsch? ich glaube das ist clamp_factor0
// und vielleicht habe ich das clamp auch falsch benutzt?
TODO:
  bool use_clamp = (out.GetConstant<std::string>("use_clamp0") == "True");

  if (op_c == "ADD" || op_c == "SUBTRACT" || op_c == "MULTIPLY" ||
      op_c == "DIVIDE" || op_c == "LESS_THAN" || op_c == "GREATER_THAN") {
    std::string sign;

    if (op_c == "ADD") {
      sign = "+";
    } else if (op_c == "SUBTRACT") {
      sign = "-";
    } else if (op_c == "MULTIPLY") {
      sign = "*";
    } else if (op_c == "DIVIDE") {
      sign = "/";
    } else if (op_c == "LESS_THAN") //<
    {
      sign = "<";
    } else if (op_c == "GREATER_THAN") //>
    {
      sign = ">";
    }

    if (use_clamp) {
      out + Out::RIGHT / "Value0" + "=" + "clamp(" + Out::LEFT / "A0" + sign +
          Out::LEFT / "B0" + ", " + ", 0.0, 1.0)" + ";";
    } else {
      out + Out::RIGHT / "Value0" + "=" + Out::LEFT / "A0" + sign +
          Out::LEFT / "B0" + ";";
    }

    return out.GetStatus();
  }

  

  // alle mit function und einer variable

  else if (op_c == "SQRT" || op_c == "INVERSE_SQRT" || op_c == "EXPONENT" ||
           op_c == "ABSOLUTE" || op_c == "FLOOR" || op_c == "SIGN" ||
           op_c == "CEIL" || op_c == "FRACT" || op_c == "TRUNC" ||
           op_c == "ROUND" || op_c == "SINE" || op_c == "COSINE" ||
           op_c == "TANGENT" || op_c == "ARCSINE" || op_c == "ARCCOSINE" ||
           op_c == "ARCTANGENT" || op_c == "SINH" || op_c == "COSH" ||
           op_c == "TANH" || op_c == "RADIANS") {

    std::string function;

    if (op_c == "SQRT") // sqrt(x)
    {
      function = "sqrt(";
    } else if (op_c == "INVERSE_SQRT") // inverseqrt(x)
    {
      function = "inversesqrt(";

    } else if (op_c == "EXPONENT") // euler^x 2.718281828459045
    {
      function = "pow(2.718281828459045, ";
    }

    else if (op_c == "ABSOLUTE") {
      function = "abs(";
    } else if (op_c == "FLOOR") {
      function = "floor(";
    } else if (op_c == "SIGN") {
      function = "sign(";
    } else if (op_c == "CEIL") {
      function = "ceil(";
    } else if (op_c == "FRACT") {
      function = "fract(";
    } else if (op_c == "TRUNC") {
      function = "trunc(";
    } else if (op_c == "ROUND") {
      function = "round(";
    } else if (op_c == "SINE") {
      function = "sin(";
    } else if (op_c == "COSINE") {
      function = "cos(";
    } else if (op_c == "TANGENT") {
      function = "tan(";
    } else if (op_c == "ARCSINE") {
      function = "asin(";
    } else if (op_c == "ARCCOSINE") {
      function = "acos(";
    } else if (op_c == "ARCTANGENT") {
      function = "atan(";
    } else if (op_c == "SINH") {
      function = "sinh(";
    } else if (op_c == "COSH") {
      function = "cosh(";
    } else if (op_c == "TANH") {
      function = "tanh(";
    } else if (op_c == "RADIANS") {
      function = "radians(";
    }

    if (use_clamp) {
      out + Out::RIGHT / "Value0" + "=" + "clamp(" + function +
          Out::LEFT / "A0" + ")" + ", 0.0, 1.0)" + ";";
    } else {

      out + Out::RIGHT / "Value0" + "=" + function + Out::LEFT / "A0" + ")" +
          ";";
    }
    return out.GetStatus();

  
  }

  // alle mit function und 2 variablen
  else if (op_c == "POWER" || op_c == "MINIMUM" || op_c == "MAXIMUM" ||
           op_c == "ARCTAN2") {
    std::string function;

    if (op_c == "POWER") // pow(x,y) = x^y A = base B = exponent
    {
      function = "pow(";
    }

    else if (op_c == "MINIMUM") // min(x,y)
    {
      function = "min(";
    }
    else if (op_c == "MAXIMUM") // max(x,y)
    {
      function = "max(";
    }

    else if (op_c == "ARCTAN2") {
      function = "atan(";
    }

    if (use_clamp) {
      out + Out::RIGHT / "Value0" + "=" + "clamp(" + function +
          Out::LEFT / "A0" + ", " + Out::LEFT / "B0" + ")" + ", 0.0, 1.0)" + ";";
    } else {
      out + Out::RIGHT / "Value0" + "=" + function + Out::LEFT / "A0" + ", " +
          Out::LEFT / "B0" + ")" + ";";
    }

    return out.GetStatus();
  }


  // alle die ganz anders aufgebaut sind

  else if (op_c == "LOGARITHM" || op_c == "COMPARE" || op_c == "MULTIPLY_ADD") {

    if (op_c == "LOGARITHM") // es gibt nur natürlichen logarithmus und log2
    /*log_b(x) = log_2(x) / log_2(b)
    bzw log_b(x) = log(x) / log(b)*/
    {
      if (use_clamp) {
        out + Out::RIGHT / "Value0" + "=" + "clamp(" + "log(" +
            Out::LEFT / "A0" + ") /" + "log(" + Out::LEFT / "B0" + ")" + ", 0.0. 1.0);";
      } else {
        out + Out::RIGHT / "Value0" + "=" + "log(" + Out::LEFT / "A0" + ") /" +
            "log(" + Out::LEFT / "B0" + ");";
      }
    }

    else if (op_c == "COMPARE") {
      /*The compare node outputs either 0 or 1.
      It outputs 1 if the difference of the two input values are less than
      epsilon. Used to check if two values are equal within a certain
      tolerance.
      A0 = Epsilon
      B0, C0 = Inputs to be compared

      abs(B0-C0)<=A0
      */

      if (use_clamp) {
        out + Out::RIGHT / "Value0" + "=" + "clamp(float(abs(" + Out::LEFT / "B0" + 
        " - " + Out::LEFT / "C0" + ") <= " + Out::LEFT / "A0" + "), 0.0, 1.0)" + ";";
      } else {
        out + Out::RIGHT / "Value0" + "=" + "float(abs(" + Out::LEFT / "B0" + 
        " - " + Out::LEFT / "C0" + ") <= " + Out::LEFT / "A0" + ")" + ";";
}
    }

    else if (op_c == "MULTIPLY_ADD") {
      // a * b + c

      if (use_clamp) {
        out + Out::RIGHT / "Value0" + "=" + " clamp(" // clamp
            + Out::LEFT / "A0" + "*" + Out::LEFT / "B0" + "+" +
            Out::LEFT / "C0" + ", 0.0, 1.0)" // clamp
            + ";";
      } else {
        out + Out::RIGHT / "Value0" + "=" + Out::LEFT / "A0" + "*" +
            Out::LEFT / "B0" + "+" + Out::LEFT / "C0" + ";";
      }
    }
    return out.GetStatus();
  }

  // left out
  else if (op_c == "SNAP") {

    return absl::UnimplementedError(std::format(
        "Given Operation has not been implemented yet: '{}’", op_c));
  }
  else {
    return absl::InvalidArgumentError(
        std::format("Illegal value of operand constant: '{}'", op_c));
  }
}
} // namespace msk::blender
