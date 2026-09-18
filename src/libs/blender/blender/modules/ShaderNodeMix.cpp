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
TODO: //Die inputs heissen überall anders

auto ShaderNodeMix::GenerateTokenString(Out &&out) -> absl::Status {
  auto dataType = out.GetConstant<std::string>("data_type0");
  bool clamp_factor = (out.GetConstant<std::string>("clamp_factor0") == "True");
  auto factorMode = out.GetConstant<std::string>("factor_mode0");
  bool clamp_result = (out.GetConstant<std::string>("clamp_result0") == "True");
  auto blend_type = (out.GetConstant<std::string>("blend_type0"));

  // ich habe die naming conventions vergessen
  /*
man nehme die mix funktion von glsl
glsl: result = x * ( 1-a ) + y * a
*/

  /*
  A0 = Factor
  B0 = A
  C0 = b*/

  if (dataType == "FLOAT") {

    if (clamp_factor) {
      out + Out::RIGHT / "Value0" + "=" + "mix(" + Out::LEFT / "B0" + ", " +
          Out::LEFT / "C0" + ", " + "clamp(" + Out::LEFT / "A0" + "," + "0.0" +
          ", " + "1.0" + ")" + ")" + ";";
    } else {
      out + Out::RIGHT / "Value0" + "=" + "mix(" + Out::LEFT / "B0" + ", " +
          Out::LEFT / "C0" + ", " + Out::LEFT / "A0" + ";";
    }
  }

  else if (dataType == "VECTOR") {
TODO://Uniform / Non Uniform nur relevant falls beim export jetzt mehrere inputs rauskommen
    if (clamp_factor) {
      out + Out::RIGHT / "Value0" + "=" + "mix(" + Out::LEFT / "B0" + ", " +
          Out::LEFT / "C0" + ", " + "clamp(" + Out::LEFT / "A0" + "," + "0.0" +
          ", " + "1.0" + ")" + ";";

    } else {
      out + Out::RIGHT / "Value0" + "=" + "mix(" + Out::LEFT / "B0" + ", " +
          Out::LEFT / "C0" + ", " + Out::LEFT / "A0" + ";";
    }

    return out.GetStatus();
  }

  else if (dataType == "RGBA") // COLOR
  /*macht blender das automatisch zu einem vektor?*/
  {
    return absl::UnimplementedError(
        std::format("Not yet implemented: {}", dataType));
  }

  else if (dataType == "ROTATION") // vllt nicht glsl fähig direkt
  {
    TODO: //warten auf Totos Export um zu gucken wie viele Inputs das hat
    if (clamp_factor) {
      out + Out::RIGHT / "Value0" + "=" + "mix(" + Out::LEFT / "B0" + ", " +
          Out::LEFT / "C0" + ", " + "clamp(" + Out::LEFT / "A0" + "," + "0.0" +
          ", " + "1.0" + ")" + ";";

    } else {
      out + Out::RIGHT / "Value0" + "=" + "mix(" + Out::LEFT / "B0" + ", " +
          Out::LEFT / "C0" + ", " + Out::LEFT / "A0" + ";";
    }

    return out.GetStatus();
  
  }

  else {

    // throw falscher input fehler
    return absl::InvalidArgumentError(
        std::format("Illegal value of data type constant: '{}'", dataType));
  }

  
}


} // namespace msk::blender
