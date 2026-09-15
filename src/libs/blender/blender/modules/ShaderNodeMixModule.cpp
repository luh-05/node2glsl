#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <string>
#include <vector>
#include <mir/codegen.hpp>

namespace msk::blender {

#define TextToken(text) out.legacy->CreateMTT(text)
#define WildcardToken(p, name) out.legacy->CreateMWT(p, name)

auto ShaderNodeMixModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto dataType    = out.GetConstant<std::string>("data_type0");
  auto factorMode  = out.GetConstant<std::string>("factor_mode0");
  auto blendType   = out.GetConstant<std::string>("blend_type0");
  auto clampFactor = out.GetConstant<std::string>("clamp_factor0");
  auto clampResult = out.GetConstant<std::string>("clamp_result0");

  // Route correct ports based on data type
  std::string portA = "A0";
  std::string portB = "B0";
  if (dataType == "VECTOR") {
    portA = "A1";
    portB = "B1";
  } else if (dataType == "RGBA") {
    portA = "A2";
    portB = "B2";
  } else if (dataType == "ROTATION") {
    portA = "A3";
    portB = "B3";
  }

  std::string portFactor = (factorMode == "NON_UNIFORM") ? "Factor1" : "Factor0";

  // Result assignment start
  out.legacy->AddTokenVector({
      WildcardToken(Out::RIGHT, "Result0"),
      TextToken(" = ")
  });

  if (clampResult == "True") {
    out.legacy->AddTokenVector({ TextToken("clamp(") });
  }

  // Mix function call start
  out.legacy->AddTokenVector({
      TextToken("mix("),
      WildcardToken(Out::LEFT, portA),
      TextToken(", ")
  });

  // Blended value (second argument of mix)
  if (blendType == "MIX") {
    out.legacy->AddTokenVector({ WildcardToken(Out::LEFT, portB) });
  } else {
    std::string prefix = "";
    std::string infix  = "";
    std::string suffix = "";

    if (blendType == "DARKEN") {
      prefix = "min("; infix = ", "; suffix = ")";
    } else if (blendType == "LIGHTEN") {
      prefix = "max("; infix = ", "; suffix = ")";
    } else if (blendType == "MULTIPLY") {
      infix = " * ";
    } else if (blendType == "SCREEN") {
      prefix = "(vec4(1.0) - (vec4(1.0) - ";
      infix  = ") * (vec4(1.0) - ";
      suffix = "))";
    } else if (blendType == "DODGE") {
      infix  = " / (vec4(1.0) - ";
      suffix = ")";
    } else if (blendType == "BURN") {
      prefix = "(vec4(1.0) - (vec4(1.0) - ";
      infix  = ") / ";
      suffix = ")";
    }

    if (!prefix.empty()) {
      out.legacy->AddTokenVector({ TextToken(prefix) });
    }
    out.legacy->AddTokenVector({ WildcardToken(Out::LEFT, portA) });
    out.legacy->AddTokenVector({ TextToken(infix) });
    out.legacy->AddTokenVector({ WildcardToken(Out::LEFT, portB) });
    if (!suffix.empty()) {
      out.legacy->AddTokenVector({ TextToken(suffix) });
    }
  }

  // Factor parameter (third argument of mix)
  out.legacy->AddTokenVector({ TextToken(", ") });

  if (clampFactor == "True") {
    out.legacy->AddTokenVector({
        TextToken("clamp("),
        WildcardToken(Out::LEFT, portFactor),
        TextToken(", 0.0, 1.0)")
    });
  } else {
    out.legacy->AddTokenVector({ WildcardToken(Out::LEFT, portFactor) });
  }

  out.legacy->AddTokenVector({ TextToken(")") });

  // Close statement and optional result clamping
  if (clampResult == "True") {
    out.legacy->AddTokenVector({ TextToken(", 0.0, 1.0);") });
  } else {
    out.legacy->AddTokenVector({ TextToken(";") });
  }

  return out.GetStatus();
}

#undef TextToken
#undef WildcardToken

} // namespace msk::blender