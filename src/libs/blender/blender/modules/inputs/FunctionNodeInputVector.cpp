#include "blender/modules/modules.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

auto GenerateTokenStringFunctionNodeInputVector(Out &out) -> absl::Status {
  auto x = out.GetConstant<std::string>("Value0");
  auto y = out.GetConstant<std::string>("Value1");
  auto z = out.GetConstant<std::string>("Value2");

  out + Out::RIGHT / "Vector0" + " = vec3(" +
      std::format("{}, {}, {}", x, y, z) + ");";

  return out.GetStatus();
}

} // namespace msk::blender
