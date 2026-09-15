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

auto FunctionNodeFloatToInt::GenerateTokenString(Out &&out) -> absl::Status {
  auto mode = out.GetConstant<std::string>("rounding_mode0");
  std::string func_expr;
  std::string close_expr;

  if (mode == "ROUND") {
    func_expr = "int(round(";
    close_expr = "));";
  } else if (mode == "FLOOR") {
    func_expr = "int(floor(";
    close_expr = "));";
  } else if (mode == "CEILING" || mode == "CEIL") {
    func_expr = "int(ceil(";
    close_expr = "));";
  } else if (mode == "TRUNCATE" || mode == "TRUNC") {
    func_expr = "int(";
    close_expr = ");";
  } else {
    func_expr = "int(";
    close_expr = ");";
  }

  out.legacy->AddTokenVector({
      WildcardToken(Out::RIGHT, "Integer0"),
      TextToken(" = "),
      TextToken(func_expr),
      WildcardToken(Out::LEFT, "Float0"),
      TextToken(close_expr)
  });

  return out.GetStatus();
}

#undef TextToken
#undef WildcardToken

} // namespace msk::blender