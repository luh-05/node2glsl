#include "blender/modules/modules.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

auto GenerateTokenStringShaderNodeValue(Out &&out) -> absl::Status {
  auto value_var = out.GetConstant<std::string>("Value0");

  out + Out::RIGHT / "Value1" + "=" + value_var + ";";

  return out.GetStatus();
}

} // namespace msk::blender
