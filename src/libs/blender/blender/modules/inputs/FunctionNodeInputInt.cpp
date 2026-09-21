#include "blender/modules/modules.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

auto GenerateTokenStringFunctionNodeInputInt(Out &out) -> absl::Status {
  auto integer_var = out.GetConstant<std::string>("integer0");

  out + Out::RIGHT / "Integer0" + "=" + integer_var + ";";

  return out.GetStatus();
}

} // namespace msk::blender
