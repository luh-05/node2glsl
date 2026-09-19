#include "blender/modules/modules.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

auto FunctionNodeInputBool::GenerateTokenString(Out &&out) -> absl::Status {
  auto boolean_var = out.GetConstant<std::string>("boolean0");

  out + Out::RIGHT / "Boolean0" + "=" + boolean_var + ";";

  return out.GetStatus();
}

} // namespace msk::blender
