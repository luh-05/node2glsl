#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>
#include <string>
#include <vector>

namespace msk::blender {

// #define TextToken(text) out.legacy->CreateMTT(text)
// #define WildcardToken(p, name) out.legacy->CreateMWT(p, name)

auto ShaderNodeMixModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto dataType = out.GetConstant<std::string>("data_type0");
  bool use_clamp = (out.GetConstant<std::string>("clamp_factor0") == "True");
  auto factorMode  = out.GetConstant<std::string>("factor_mode0");
  bool clamp_result = (out.GetConstant<std::string> ("clamp_result0")=="True");
  // ich habe die naming conventions vergessen
  /*
man nehme die mix funktion von glsl
glsl: result = x * ( 1-a ) + y * a
*/

/*
A0 = Factor
B0 = A
C0 = b*/
  clamp_result 
  if (dataType == "FLOAT") {
    
    if(use_clamp){
    out + Out::RIGHT / "Value0" + "=" 
    + "mix("
    + Out::LEFT / "B0" + ", "
    + Out::LEFT / "C0" + ", "
    + "clamp("
    + Out::LEFT / "A0" + "," + "0.0" + ", " + "1.0" + ")" + ")"
    + ";";
    
    }
    else{
      out + Out::RIGHT / "Value0" + "=" 
    + "mix("
    + Out::LEFT / "B0" + ", "
    + Out::LEFT / "C0" + ", "
    + Out::LEFT / "A0" + ";";
    }

  } 
  else if (dataType == "VECTOR") {
      // if(factorMode == "UNIFORM"){

      // }
      // else if (factorMode == TBA ){

      // }
      // else {

      // }
    if(use_clamp){
      out + Out::RIGHT / "Value0" + "=" 
      + "mix("
      + Out::LEFT / "B0" + ", "
      + Out::LEFT / "C0" + ", "
      + "clamp("
      + Out::LEFT / "A0" + "," + "0.0" + ", " + "1.0" + ")"
      + ";";
    
    }
    else{
      out + Out::RIGHT / "Value0" + "=" 
      + "mix("
      + Out::LEFT / "B0" + ", "
      + Out::LEFT / "C0" + ", "
      + Out::LEFT / "A0" + ";";
    }
  } else if(dataType == "RGBA")//COLOR 
  {
    if(clamp_result){

    }
    else{

    }
  }

    else if (dataType == "ROTATION")//vllt nicht glsl fähig direkt
    {


    }

    else {
      
    // throw falscher input fehler
    return absl::InvalidArgumentError(
        std::format("Illegal value of data type constant: '{}'", dataType));}
  }

  // std::string portFactor = (factorMode == "NON_UNIFORM") ? "Factor1" :
  // "Factor0";

  // Result assignment start
  // out.legacy->AddTokenVector({
  //     WildcardToken(Out::RIGHT, "Result0"),
  //     TextToken(" = ")
  // });

  if (clampResult == "True") {
    // out.legacy->AddTokenVector({ TextToken("clamp(") });
  }

  // Mix function call start
  // out.legacy->AddTokenVector({
  //     TextToken("mix("),
  //     WildcardToken(Out::LEFT, portA),
  //     TextToken(", ")
  // });

  // Blended value (second argument of mix)
  if (blendType == "MIX") {
    // out.legacy->AddTokenVector({ WildcardToken(Out::LEFT, portB) });
  } else {
    std::string prefix = "";
    std::string infix = "";
    std::string suffix = "";

    if (blendType == "DARKEN") {
      prefix = "min(";
      infix = ", ";
      suffix = ")";
    } else if (blendType == "LIGHTEN") {
      prefix = "max(";
      infix = ", ";
      suffix = ")";
    } else if (blendType == "MULTIPLY") {
      infix = " * ";
    } else if (blendType == "SCREEN") {
      prefix = "(vec4(1.0) - (vec4(1.0) - ";
      infix = ") * (vec4(1.0) - ";
      suffix = "))";
    } else if (blendType == "DODGE") {
      infix = " / (vec4(1.0) - ";
      suffix = ")";
    } else if (blendType == "BURN") {
      prefix = "(vec4(1.0) - (vec4(1.0) - ";
      infix = ") / ";
      suffix = ")";
    }

    if (!prefix.empty()) {
      // out.legacy->AddTokenVector({ TextToken(prefix) });
    }
    // out.legacy->AddTokenVector({ WildcardToken(Out::LEFT, portA) });
    // out.legacy->AddTokenVector({ TextToken(infix) });
    // out.legacy->AddTokenVector({ WildcardToken(Out::LEFT, portB) });
    if (!suffix.empty()) {
      // out.legacy->AddTokenVector({ TextToken(suffix) });
    }
  }

  // Factor parameter (third argument of mix)
  // out.legacy->AddTokenVector({ TextToken(", ") });

  if (clampFactor == "True") {
    // out.legacy->AddTokenVector({
    //     TextToken("clamp("),
    //     WildcardToken(Out::LEFT, portFactor),
    //     TextToken(", 0.0, 1.0)")
    // });
  } else {
    // out.legacy->AddTokenVector({ WildcardToken(Out::LEFT, portFactor) });
  }

  // out.legacy->AddTokenVector({ TextToken(")") });

  // Close statement and optional result clamping
  if (clampResult == "True") {
    // out.legacy->AddTokenVector({ TextToken(", 0.0, 1.0);") });
  } else {
    // out.legacy->AddTokenVector({ TextToken(";") });
  }

  return out.GetStatus();
}

} // namespace msk::blender