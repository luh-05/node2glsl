#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <string>
#include <format>
#include <mir/codegen.hpp>

namespace msk::blender {

auto ShaderNodeMapRangeModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto data_type = out.GetConstant<std::string>("data_type0");
  auto interp    = out.GetConstant<std::string>("interpolation_type0");
  auto clamp_val = out.GetConstant<std::string>("clamp0");

  std::string p_val, p_fmin, p_fmax, p_tmin, p_tmax, p_steps, p_out;

  if (data_type == "FLOAT") {
    p_val   = "Value0";
    p_fmin  = "From Min0";
    p_fmax  = "From Max0";
    p_tmin  = "To Min0";
    p_tmax  = "To Max0";
    p_steps = "Steps0";
    p_out   = "Result0";
  } 
  else if (data_type == "FLOAT_VECTOR") {
    p_val   = "Vector0";
    p_fmin  = "From Min1";
    p_fmax  = "From Max1";
    p_tmin  = "To Min1";
    p_tmax  = "To Max1";
    p_steps = "Steps1";
    p_out   = "Vector0";
  } 
  else {
    return absl::InvalidArgumentError(
        std::format("Unsupported data_type for Map Range: '{}'", data_type));
  }

  if (interp == "LINEAR") {
    if (clamp_val == "True") {
      out + Out::RIGHT / p_out
          + " = clamp("
          + Out::LEFT / p_tmin
          + " + ("
          + Out::LEFT / p_val
          + " - "
          + Out::LEFT / p_fmin
          + ") / ("
          + Out::LEFT / p_fmax
          + " - "
          + Out::LEFT / p_fmin
          + ") * ("
          + Out::LEFT / p_tmax
          + " - "
          + Out::LEFT / p_tmin
          + "), min("
          + Out::LEFT / p_tmin
          + ", "
          + Out::LEFT / p_tmax
          + "), max("
          + Out::LEFT / p_tmin
          + ", "
          + Out::LEFT / p_tmax
          + "));";
    } else {
      out + Out::RIGHT / p_out
          + " = "
          + Out::LEFT / p_tmin
          + " + ("
          + Out::LEFT / p_val
          + " - "
          + Out::LEFT / p_fmin
          + ") / ("
          + Out::LEFT / p_fmax
          + " - "
          + Out::LEFT / p_fmin
          + ") * ("
          + Out::LEFT / p_tmax
          + " - "
          + Out::LEFT / p_tmin
          + ");";
    }
  } 
  
  else if (interp == "STEPPED") {
    if (clamp_val == "True") {
      out + Out::RIGHT / p_out
          + " = clamp("
          + Out::LEFT / p_tmin
          + " + floor(("
          + Out::LEFT / p_val
          + " - "
          + Out::LEFT / p_fmin
          + ") / ("
          + Out::LEFT / p_fmax
          + " - "
          + Out::LEFT / p_fmin
          + ") * ("
          + Out::LEFT / p_steps
          + " + 1.0)) / "
          + Out::LEFT / p_steps
          + " * ("
          + Out::LEFT / p_tmax
          + " - "
          + Out::LEFT / p_tmin
          + "), min("
          + Out::LEFT / p_tmin
          + ", "
          + Out::LEFT / p_tmax
          + "), max("
          + Out::LEFT / p_tmin
          + ", "
          + Out::LEFT / p_tmax
          + "));";
    } else {
      out + Out::RIGHT / p_out
          + " = "
          + Out::LEFT / p_tmin
          + " + floor(("
          + Out::LEFT / p_val
          + " - "
          + Out::LEFT / p_fmin
          + ") / ("
          + Out::LEFT / p_fmax
          + " - "
          + Out::LEFT / p_fmin
          + ") * ("
          + Out::LEFT / p_steps
          + " + 1.0)) / "
          + Out::LEFT / p_steps
          + " * ("
          + Out::LEFT / p_tmax
          + " - "
          + Out::LEFT / p_tmin
          + ");";
    }
  } 
  
  else if (interp == "SMOOTHSTEP") {
    if (clamp_val == "True") {
      out + Out::RIGHT / p_out
          + " = clamp("
          + Out::LEFT / p_tmin
          + " + smoothstep(0.0, 1.0, ("
          + Out::LEFT / p_val
          + " - "
          + Out::LEFT / p_fmin
          + ") / ("
          + Out::LEFT / p_fmax
          + " - "
          + Out::LEFT / p_fmin
          + ")) * ("
          + Out::LEFT / p_tmax
          + " - "
          + Out::LEFT / p_tmin
          + "), min("
          + Out::LEFT / p_tmin
          + ", "
          + Out::LEFT / p_tmax
          + "), max("
          + Out::LEFT / p_tmin
          + ", "
          + Out::LEFT / p_tmax
          + "));";
    } else {
      out + Out::RIGHT / p_out
          + " = "
          + Out::LEFT / p_tmin
          + " + smoothstep(0.0, 1.0, ("
          + Out::LEFT / p_val
          + " - "
          + Out::LEFT / p_fmin
          + ") / ("
          + Out::LEFT / p_fmax
          + " - "
          + Out::LEFT / p_fmin
          + ")) * ("
          + Out::LEFT / p_tmax
          + " - "
          + Out::LEFT / p_tmin
          + ");";
    }
  } 
  
  else if (interp == "SMOOTHERSTEP") {

    if (clamp_val == "True") {
      out + Out::RIGHT / p_out
          + " = clamp("
          + Out::LEFT / p_tmin
          + " + ("
          + "clamp((" + Out::LEFT / p_val + " - " + Out::LEFT / p_fmin + ") / (" + Out::LEFT / p_fmax + " - " + Out::LEFT / p_fmin + "), 0.0, 1.0)"
          + " * " 
          + "clamp((" + Out::LEFT / p_val + " - " + Out::LEFT / p_fmin + ") / (" + Out::LEFT / p_fmax + " - " + Out::LEFT / p_fmin + "), 0.0, 1.0)"
          + " * " 
          + "clamp((" + Out::LEFT / p_val + " - " + Out::LEFT / p_fmin + ") / (" + Out::LEFT / p_fmax + " - " + Out::LEFT / p_fmin + "), 0.0, 1.0)"
          + " * (" 
          + "clamp((" + Out::LEFT / p_val + " - " + Out::LEFT / p_fmin + ") / (" + Out::LEFT / p_fmax + " - " + Out::LEFT / p_fmin + "), 0.0, 1.0)"
          + " * (" 
          + "clamp((" + Out::LEFT / p_val + " - " + Out::LEFT / p_fmin + ") / (" + Out::LEFT / p_fmax + " - " + Out::LEFT / p_fmin + "), 0.0, 1.0)"
          + " * 6.0 - 15.0) + 10.0)) * ("
          + Out::LEFT / p_tmax
          + " - "
          + Out::LEFT / p_tmin
          + "), min("
          + Out::LEFT / p_tmin
          + ", "
          + Out::LEFT / p_tmax
          + "), max("
          + Out::LEFT / p_tmin
          + ", "
          + Out::LEFT / p_tmax
          + "));";
    } else {
      out + Out::RIGHT / p_out
          + " = "
          + Out::LEFT / p_tmin
          + " + ("
          + "clamp((" + Out::LEFT / p_val + " - " + Out::LEFT / p_fmin + ") / (" + Out::LEFT / p_fmax + " - " + Out::LEFT / p_fmin + "), 0.0, 1.0)"
          + " * " 
          + "clamp((" + Out::LEFT / p_val + " - " + Out::LEFT / p_fmin + ") / (" + Out::LEFT / p_fmax + " - " + Out::LEFT / p_fmin + "), 0.0, 1.0)"
          + " * " 
          + "clamp((" + Out::LEFT / p_val + " - " + Out::LEFT / p_fmin + ") / (" + Out::LEFT / p_fmax + " - " + Out::LEFT / p_fmin + "), 0.0, 1.0)"
          + " * (" 
          + "clamp((" + Out::LEFT / p_val + " - " + Out::LEFT / p_fmin + ") / (" + Out::LEFT / p_fmax + " - " + Out::LEFT / p_fmin + "), 0.0, 1.0)"
          + " * (" 
          + "clamp((" + Out::LEFT / p_val + " - " + Out::LEFT / p_fmin + ") / (" + Out::LEFT / p_fmax + " - " + Out::LEFT / p_fmin + "), 0.0, 1.0)"
          + " * 6.0 - 15.0) + 10.0)) * ("
          + Out::LEFT / p_tmax
          + " - "
          + Out::LEFT / p_tmin
          + ");";
    }
  } 
  
  else {
    return absl::InvalidArgumentError(
        std::format("Unknown interpolation type for Map Range: '{}'", interp));
  }

  return out.GetStatus();
}

} // namespace msk::blender
