#include "blender/modules/modules.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

auto GenerateTokenStringFunctionNodeInputString(Out &out) -> absl::Status {
  // auto boolean_var = out.GetConstant<std::string>("string0");

  // out + Out::RIGHT / "String0" + "= 0.0f;";

  return out.GetStatus();
}

} // namespace msk::blender
