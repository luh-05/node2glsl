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

auto ShaderNodeFloatCurveModule::GenerateTokenString(Out &&out) -> absl::Status {
  out.legacy->AddTokenVector({
      WildcardToken(Out::RIGHT, "Value1"),
      TextToken(" = glsl_float_curve("),
      WildcardToken(Out::LEFT, "Factor0"),
      TextToken(", "),
      WildcardToken(Out::LEFT, "Value0"),
      TextToken(");")
  });

  return out.GetStatus();
}

#undef TextToken
#undef WildcardToken

} // namespace msk::blender