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

auto GenerateTokenStringShaderNodeMix(Out &out) -> absl::Status {
  auto dataType = out.GetConstant<std::string>("data_type0");
  bool clamp_factor = (out.GetConstant<std::string>("clamp_factor0") == "True");
  auto factorMode = out.GetConstant<std::string>("factor_mode0");
  bool clamp_result = (out.GetConstant<std::string>("clamp_result0") == "True");
  auto blend_type = (out.GetConstant<std::string>("blend_type0"));

  if (dataType == "FLOAT") {

    if (clamp_factor) {
      out + Out::RIGHT / "Result0" + "=" + "mix(" + Out::LEFT / "A0" + ", " +
          Out::LEFT / "B0" + ", " + "clamp(" + Out::LEFT / "Factor0" + ", " + "0.0" +
          ", " + "1.0" + ")" + ")" + ";";
    } else {
      out + Out::RIGHT / "Result0" + "=" + "mix(" + Out::LEFT / "A0" + ", " +
          Out::LEFT / "B0" + ", " + Out::LEFT / "Factor0" + ");";
    }
    return out.GetStatus();
  }

  else if (dataType == "VECTOR") {

    // Uniform: Factor0, Non Uniform: Factor1
    if (clamp_factor&&factorMode=="UNIFORM") {
      out + Out::RIGHT / "Result1" + "=" + "mix(" + Out::LEFT / "A1" + ", " +
          Out::LEFT / "B1" + ", " + "clamp(" + Out::LEFT / "Factor0" + ", " + "0.0" +
          ", " + "1.0" + "))" + ";"; //Uni

      } else if (!clamp_factor&&factorMode=="UNIFORM") {
      out + Out::RIGHT / "Result1" + "=" + "mix(" + Out::LEFT / "A1" + ", " +
          Out::LEFT / "B1" + ", " + Out::LEFT / "Factor0" + ");";
    }
    else if (clamp_factor&&factorMode=="NON_UNIFORM") {
      out + Out::RIGHT / "Result1" + "=" + "mix(" + Out::LEFT / "A1" + ", " +
          Out::LEFT / "B1" + ", " + "clamp(" + Out::LEFT / "Factor1" + ", " + "0.0" +
          ", " + "1.0" + "))" + ";"; //NonUni
    } else if (!clamp_factor&&factorMode=="NON_UNIFORM") {
      out + Out::RIGHT / "Result1" + "=" + "mix(" + Out::LEFT / "A1" + ", " +
          Out::LEFT / "B1" + ", " + Out::LEFT / "Factor1" + ");";
    }
    else {
      return absl::InvalidArgumentError(
          std::format("Illegal value of factor mode constant: '{}'", factorMode));
    } 

    return out.GetStatus();
  }

  else if (dataType == "RGBA") // COLOR
  {
    return absl::UnimplementedError(
        std::format("Not yet implemented: {}", dataType));
  }

  else if (dataType == "ROTATION") // vllt nicht glsl fähig direkt
  {

    if (clamp_factor) {
      out + Out::RIGHT / "Result3" + "=" + "mix(" + Out::LEFT / "A3" + ", " +
          Out::LEFT / "B3" + ", " + "clamp(" + Out::LEFT / "Factor0" + ", " + "0.0" +
          ", " + "1.0" + "))" + ";";

    } else {
      out + Out::RIGHT / "Result3" + "=" + "mix(" + Out::LEFT / "A3" + ", " +
          Out::LEFT / "B3" + ", " + Out::LEFT / "Factor0" + ");";
    }

    return out.GetStatus();

  }

  else {

    return absl::InvalidArgumentError(
        std::format("Illegal value of data type constant: '{}'", dataType));
  }

  return absl::InvalidArgumentError(
      std::format("Illegal value of data type constant: '{}'", dataType));
}

} // namespace msk::blender
