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

auto ShaderNodeMapRangeModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto data_type = out.GetConstant<std::string>("data_type0");
  auto interp_type = out.GetConstant<std::string>("interpolation_type0");
  auto clamp = out.GetConstant<std::string>("clamp0");

  // Port names dependent on data type
  std::string val_in;
  std::string from_min;
  std::string from_max;
  std::string to_min;
  std::string to_max;
  std::string steps;
  std::string result_out;

  if (data_type == "FLOAT_VECTOR") {
    val_in = "Vector0";
    from_min = "From Min1";
    from_max = "From Max1";
    to_min = "To Min1";
    to_max = "To Max1";
    steps = "Steps1";
    result_out = "Vector1";
  } else {
    // Default / Float fallback
    val_in = "Value0";
    from_min = "From Min0";
    from_max = "From Max0";
    to_min = "To Min0";
    to_max = "To Max0";
    steps = "Steps0";
    result_out = "Result0";
  }

  // Dynamic target function name construct
  std::string func_name = "map_range_";

  if (interp_type == "LINEAR") {
    func_name += "linear";
  } else if (interp_type == "STEPPED") {
    func_name += "stepped";
  } else if (interp_type == "SMOOTHSTEP") {
    func_name += "smoothstep";
  } else if (interp_type == "SMOOTHERSTEP") {
    func_name += "smootherstep";
  }

  if (clamp == "True") {
    func_name += "_clamp";
  }

  // STEPPED interpolation requires the extra "Steps" parameter
  if (interp_type == "STEPPED") {
    out.legacy->AddTokenVector({
        WildcardToken(Out::RIGHT, result_out),
        TextToken(" = " + func_name + "("),
        WildcardToken(Out::LEFT, val_in),
        TextToken(", "),
        WildcardToken(Out::LEFT, from_min),
        TextToken(", "),
        WildcardToken(Out::LEFT, from_max),
        TextToken(", "),
        WildcardToken(Out::LEFT, to_min),
        TextToken(", "),
        WildcardToken(Out::LEFT, to_max),
        TextToken(", "),
        WildcardToken(Out::LEFT, steps),
        TextToken(");")
    });
  } else {
    out.legacy->AddTokenVector({
        WildcardToken(Out::RIGHT, result_out),
        TextToken(" = " + func_name + "("),
        WildcardToken(Out::LEFT, val_in),
        TextToken(", "),
        WildcardToken(Out::LEFT, from_min),
        TextToken(", "),
        WildcardToken(Out::LEFT, from_max),
        TextToken(", "),
        WildcardToken(Out::LEFT, to_min),
        TextToken(", "),
        WildcardToken(Out::LEFT, to_max),
        TextToken(");")
    });
  }

  return out.GetStatus();
}

#undef TextToken
#undef WildcardToken

} // namespace msk::blender