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

auto ShaderNodeClampModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto clamp_type = out.GetConstant<std::string>("clamp_type0");

  if (clamp_type == "RANGE") {
    out.legacy->AddTokenVector({
        WildcardToken(Out::RIGHT, "Result0"),
        TextToken(" = clamp("),
        WildcardToken(Out::LEFT, "Value0"),
        TextToken(", min("),
        WildcardToken(Out::LEFT, "Min0"),
        TextToken(", "),
        WildcardToken(Out::LEFT, "Max0"),
        TextToken("), max("),
        WildcardToken(Out::LEFT, "Min0"),
        TextToken(", "),
        WildcardToken(Out::LEFT, "Max0"),
        TextToken("));")
    });
  } else {
    out.legacy->AddTokenVector({
        WildcardToken(Out::RIGHT, "Result0"),
        TextToken(" = clamp("),
        WildcardToken(Out::LEFT, "Value0"),
        TextToken(", "),
        WildcardToken(Out::LEFT, "Min0"),
        TextToken(", "),
        WildcardToken(Out::LEFT, "Max0"),
        TextToken(");")
    });
  }

  return out.GetStatus();
}

#undef TextToken
#undef WildcardToken

} // namespace msk::blender