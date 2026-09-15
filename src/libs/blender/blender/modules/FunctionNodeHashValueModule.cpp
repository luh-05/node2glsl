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

auto FunctionNodeHashValueModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto data_type = out.GetConstant<std::string>("data_type0");

  if (data_type == "STRING") {
    out.legacy->AddTokenVector({
        WildcardToken(Out::RIGHT, "Hash0"),
        TextToken(" = 0.0;")
    });
  } else if (data_type == "INT") {
    out.legacy->AddTokenVector({
        WildcardToken(Out::RIGHT, "Hash0"),
        TextToken(" = glsl_hash_int("),
        WildcardToken(Out::LEFT, "Value0"),
        TextToken(", "),
        WildcardToken(Out::LEFT, "Seed0"),
        TextToken(");")
    });
  } else if (data_type == "VECTOR") {
    out.legacy->AddTokenVector({
        WildcardToken(Out::RIGHT, "Hash0"),
        TextToken(" = glsl_hash_vector("),
        WildcardToken(Out::LEFT, "Value0"),
        TextToken(", "),
        WildcardToken(Out::LEFT, "Seed0"),
        TextToken(");")
    });
  } else if (data_type == "RGBA") {
    out.legacy->AddTokenVector({
        WildcardToken(Out::RIGHT, "Hash0"),
        TextToken(" = glsl_hash_rgba("),
        WildcardToken(Out::LEFT, "Value0"),
        TextToken(", "),
        WildcardToken(Out::LEFT, "Seed0"),
        TextToken(");")
    });
  } else if (data_type == "ROTATION") {
    out.legacy->AddTokenVector({
        WildcardToken(Out::RIGHT, "Hash0"),
        TextToken(" = glsl_hash_rotation("),
        WildcardToken(Out::LEFT, "Value0"),
        TextToken(", "),
        WildcardToken(Out::LEFT, "Seed0"),
        TextToken(");")
    });
  } else if (data_type == "MATRIX") {
    out.legacy->AddTokenVector({
        WildcardToken(Out::RIGHT, "Hash0"),
        TextToken(" = glsl_hash_matrix("),
        WildcardToken(Out::LEFT, "Value0"),
        TextToken(", "),
        WildcardToken(Out::LEFT, "Seed0"),
        TextToken(");")
    });
  } else { // "FLOAT" or default
    out.legacy->AddTokenVector({
        WildcardToken(Out::RIGHT, "Hash0"),
        TextToken(" = glsl_hash_float("),
        WildcardToken(Out::LEFT, "Value0"),
        TextToken(", "),
        WildcardToken(Out::LEFT, "Seed0"),
        TextToken(");")
    });
  }

  return out.GetStatus();
}

#undef TextToken
#undef WildcardToken

} // namespace msk::blender